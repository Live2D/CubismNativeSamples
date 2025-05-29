/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include <functional>

#include <Rendering/D3D9/CubismRenderer_D3D9.hpp>

#include "LAppDefine.hpp"
#include "LAppAllocator_Common.hpp"
#include "LAppPal.hpp"
#include "CubismUserModelExtend.hpp"
#include "MouseActionManager_Common.hpp"
#include "CubismDirectXRenderer.hpp"
#include "CubismDirectXView.hpp"
#include "CubismTextureManager.hpp"

#include <CubismFramework.hpp>
#include <CubismModelSettingJson.hpp>
#include <Model/CubismUserModel.hpp>
#include <Physics/CubismPhysics.hpp>
#include <Utils/CubismString.hpp>

/**
*@brief モデルデータのディレクトリ名
* このディレクトリ名と同名の.model3.jsonを読み込む
*/
static const Csm::csmChar* _modelDirectoryName = "Hiyori";

static Csm::CubismUserModel* _userModel; ///< ユーザーが実際に使用するモデル

Csm::csmFloat32 _userTimeSeconds; ///< デルタ時間の積算値[秒]
Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///< モデルに設定されたまばたき機能用パラメータID
Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> _motions; ///< 読み込まれているモーションのリスト
Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> _expressions; ///< 読み込まれている表情のリスト

Csm::CubismPose* _pose;                      ///< ポーズ管理
Csm::CubismBreath* _breath;                    ///< 呼吸
Csm::CubismPhysics* _physics;                   ///< 物理演算
Csm::CubismEyeBlink* _eyeBlink;                  ///< 自動まばたき
Csm::CubismTargetPoint*_dragManager;               ///< マウスドラッグ
Csm::CubismModelMatrix* _modelMatrix;               ///< モデル行列
Csm::CubismMotionManager* _motionManager;             ///< モーション管理
Csm::CubismMotionManager* _expressionManager;         ///< 表情管理
Csm::CubismModelUserData* _modelUserData;             ///< ユーザデータ

bool _captured;                              ///< クリックしているか
float _mouseX;                               ///< マウスX座標
float _mouseY;                               ///< マウスY座標
bool _isEnd;                                 ///< APP終了しているか

Csm::csmFloat32 _dragX;                         ///< マウスドラッグのX位置
Csm::csmFloat32 _dragY;                         ///< マウスドラッグのY位置
Csm::csmFloat32 _accelerationX;                 ///< X軸方向の加速度
Csm::csmFloat32 _accelerationY;                 ///< Y軸方向の加速度
Csm::csmFloat32 _accelerationZ;                 ///< Z軸方向の加速度

static Csm::CubismFramework::Option _cubismOption; ///< CubismFrameworkに関するオプション
static LAppAllocator_Common _cubismAllocator; ///< メモリのアロケーター

static CubismTextureManager* _textureManager; ///< テクスチャの管理

static std::string _currentModelDirectory; ///< 現在のモデルのディレクトリ名
const Csm::csmChar* _currentModelDirectoryChar; ///< 現在のモデルのディレクトリ名のconst csmCharポインタ型

int windowWidth, windowHeight; ///< ウィンドウサイズの保存

/**
* @brief Cubism SDKの初期化
*
* Cubism SDKの初期化処理を行う
*/
static void InitializeCubism()
{
    //setup cubism
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    _cubismOption.LoadFileFunction = LAppPal::LoadFileAsBytes;
    _cubismOption.ReleaseBytesFunction = LAppPal::ReleaseBytes;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

    // モデルロード前に必ず呼び出す必要がある
    Live2D::Cubism::Framework::Rendering::CubismRenderer_D3D9::InitializeConstantSettings(CubismDirectXRenderer::GetInstance()->BackBufferNum, CubismDirectXRenderer::GetInstance()->_device);

    LAppPal::UpdateTime();
}

LRESULT WINAPI MsgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:    // 終了
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:  // ウインドウ描画時
        ValidateRect(wnd, nullptr);
        return 0;

    case WM_SIZE:   // ウィンドウサイズ変更
        if (MouseActionManager_Common::GetInstance() && CubismDirectXRenderer::GetInstance()->_device && CubismDirectXView::GetInstance())
        {
            // _device作成前にCreateWindowをやった時もここに来るので、_deviceのNullチェックは必須
            // Resize指示を出す
            CubismDirectXRenderer::GetInstance()->RecreateDevice();
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (!MouseActionManager_Common::GetInstance())
        {
            return 0;
        }

        _mouseX = static_cast<float>(LOWORD(lParam));
        _mouseY = static_cast<float>(HIWORD(lParam));

        {
            _captured = true;
            MouseActionManager_Common::GetInstance()->OnTouchesBegan(_mouseX, _mouseY);
        }
        return 0;

    case WM_LBUTTONUP:
        if (MouseActionManager_Common::GetInstance() == nullptr)
        {
            return 0;
        }

        _mouseX = static_cast<float>(LOWORD(lParam));
        _mouseY = static_cast<float>(HIWORD(lParam));

        {
            if (_captured)
            {
                _captured = false;
                MouseActionManager_Common::GetInstance()->OnTouchesEnded(_mouseX, _mouseY);
            }
        }
        return 0;

    case WM_MOUSEMOVE:
        _mouseX = static_cast<float>(LOWORD(lParam));
        _mouseY = static_cast<float>(HIWORD(lParam));

        if (!_captured)
        {
            return 0;
        }
        if (MouseActionManager_Common::GetInstance() == nullptr)
        {
            return 0;
        }

        MouseActionManager_Common::GetInstance()->OnTouchesMoved(_mouseX, _mouseY);
        return 0;

    default:
        break;
    }
    return DefWindowProc(wnd, msg, wParam, lParam);
}

/**
* @brief システムの初期化
*
* 基盤システムの初期化処理を行う
*/
static bool InitializeSystem()
{
    LAppPal::PrintLogLn("START");

    CubismDirectXRenderer* directXRenderer = CubismDirectXRenderer::GetInstance();

    // ウィンドウクラス
    directXRenderer->_windowClass = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, directXRenderer->ClassName, NULL };
    RegisterClassEx(&directXRenderer->_windowClass);

    // タイトルバー、ウィンドウ枠の分サイズを増やす
    RECT rect;
    {
        SetRect(&rect, 0, 0, LAppDefine::RenderTargetWidth, LAppDefine::RenderTargetHeight);
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
        rect.right = rect.right - rect.left;
        rect.bottom = rect.bottom - rect.top;
        rect.top = 0;
        rect.left = 0;
    }

    //ウインドウの生成
    directXRenderer->_windowHandle = CreateWindow(directXRenderer->ClassName, directXRenderer->ClassName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom, NULL, NULL, directXRenderer->_windowClass.hInstance, NULL);
    if (directXRenderer->_windowHandle == NULL)
    {
        LAppPal::PrintLogLn("Fail Create Window");
        return false;
    }

    //ウインドウ表示
    ShowWindow(directXRenderer->_windowHandle, SW_SHOWDEFAULT);
    UpdateWindow(directXRenderer->_windowHandle);

    // Direct3D9 init
    if (!(directXRenderer->_direct3D = Direct3DCreate9(D3D_SDK_VERSION)))
    {
        LAppPal::PrintLogLn("Fail Direct3D Initialize");
        return false;
    }

    // デフォルトバックバッファ作成枚数
    const csmInt32 BackBufferNum = 1;

    // デバイス設定
    memset(&directXRenderer->_presentParameters, 0, sizeof(directXRenderer->_presentParameters));
    directXRenderer->_presentParameters.Windowed = TRUE;                          // ウインドウモード
    directXRenderer->_presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;       // バックバッファのスワップエフェクト Direct3Dにスワップエフェクトをまかせる
    directXRenderer->_presentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;       // バックバッファのフォーマット D3DFMT_UNKNOWNだと今表示されているモニタの設定と同じ
    directXRenderer->_presentParameters.BackBufferCount = BackBufferNum;          // バックバッファ数
    directXRenderer->_presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;    // マルチサンプリング無し
    directXRenderer->_presentParameters.MultiSampleQuality = 0;                   // マルチサンプリング無し
    directXRenderer->_presentParameters.EnableAutoDepthStencil = TRUE;            // ZはD3d任せ
    directXRenderer->_presentParameters.AutoDepthStencilFormat = D3DFMT_D16;      // Z精度はushort
    directXRenderer->_presentParameters.hDeviceWindow = directXRenderer->_windowHandle;
    directXRenderer->_presentParameters.Flags = 0;
    directXRenderer->_presentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; // 今のリフレッシュレートをそのまま使う
    directXRenderer->_presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;   // モニタの垂直回帰を待つ

    HRESULT createResult = directXRenderer->_direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, directXRenderer->_windowHandle,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &(directXRenderer->_presentParameters),
        &directXRenderer->_device);

    if (FAILED(createResult))
    {
        LAppPal::PrintLogLn("Fail Direct3D Create Device %x", createResult);
        return false;
}

    // メンバーコピー
    directXRenderer->_presentParametersFull = directXRenderer->_presentParameters;
    directXRenderer->_presentParametersFull.Windowed = FALSE; // 違いはこれとサイズ
    // フルスクリーン化した場合の最適解像度をチェック
    directXRenderer->CheckFullScreen(D3DFMT_X8R8G8B8, LAppDefine::RenderTargetWidth, LAppDefine::RenderTargetHeight);

    //MouseActionManager_Commonの初期化
    MouseActionManager_Common::GetInstance()->Initialize(windowWidth, windowHeight);

    // Cubism SDK の初期化
    InitializeCubism();

    return true;
}

void Release()
{
    // レンダラの解放
    _userModel->DeleteRenderer();

    // モデルデータの解放
    delete _userModel;

    // テクスチャマネージャーの解放
    delete _textureManager;
    _textureManager = nullptr;

    // CubismDirectXRendererの解放
    CubismDirectXRenderer::GetInstance()->ReleaseInstance();

    // MouseActionManager_Commonの解放
    MouseActionManager_Common::ReleaseInstance();

    // Cubism SDK の解放
    Csm::CubismFramework::Dispose();
}

/**
* @brief ディレクトリパスの設定
*
* モデルのディレクトリパスを設定する
*/
void SetAssetDirectory(const std::string& path)
{
    _currentModelDirectory = path;
}

/**
* @brief モデルの読み込み
*
* モデルデータの読み込み処理を行う
*
* @param[in] modelDirectory モデルのディレクトリ名
*/
void LoadModel(const std::string modelDirectoryName)
{
    // モデルのディレクトリを指定
    SetAssetDirectory(LAppDefine::ResourcesPath + modelDirectoryName + "/");

    // モデルデータの新規生成
    _userModel = new CubismUserModelExtend(modelDirectoryName, _currentModelDirectory);

    // モデルデータの読み込み及び生成とセットアップを行う
    static_cast<CubismUserModelExtend*>(_userModel)->SetupModel();

    // ユーザーモデルをMouseActionManager_Commonへ渡す
    MouseActionManager_Common::GetInstance()->SetUserModel(_userModel);

    // モデルのデータをCubismDirectXRendererへ渡す
    CubismDirectXRenderer::GetInstance()->_model = static_cast<CubismUserModelExtend*>(_userModel);
}

void Run()
{
    MSG msg;
    CubismDirectXRenderer* directXRenderer = CubismDirectXRenderer::GetInstance();

    // デフォルトのメインフレームバッファへレンダリングする
    CubismDirectXView::GetInstance()->SwitchRenderingTarget(CubismDirectXView::SelectTarget_None);
    // 背景クリア色を設定
    float clearColor[3] = { 1.0f, 1.0f, 1.0f };
    CubismDirectXView::GetInstance()->SetRenderTargetClearColor(clearColor[0], clearColor[1], clearColor[2]);

    do
    {
        //メッセージループ
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 時間更新
            LAppPal::UpdateTime();

            // 画面クリアなど
            directXRenderer->StartFrame();

            // 描画
            CubismDirectXView::GetInstance()->Render(_userModel);

            // フレーム末端処理
            directXRenderer->EndFrame(_userModel);

            // アプリケーション終了メッセージでウィンドウを破棄する
            if (directXRenderer->_isEnd && directXRenderer->_windowHandle != nullptr)
            {
                // ウィンドウ破壊
                DestroyWindow(directXRenderer->_windowHandle);
                directXRenderer->_windowHandle = nullptr;
            }
        }
    } while (msg.message != WM_QUIT && !(directXRenderer->_isEnd));
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    // 初期化
    InitializeSystem();
    // モデルの読み込み
    LoadModel(_modelDirectoryName);

    // メインループ
    Run();

    // 解放処理
    Release();

    return 0;
}
