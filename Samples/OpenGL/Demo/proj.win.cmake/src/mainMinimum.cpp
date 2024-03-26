/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include <functional>

#include <stb_image.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "LAppDefine.hpp"
#include "LAppAllocator.hpp"
#include "LAppTextureManager.hpp"
#include "LAppPal.hpp"
#include "TouchManager.hpp"
#include "CubismUserModelExtend.hpp"
#include "CubismSampleViewMatrix.hpp"
#include "MouseActionManager.hpp"

#include <CubismFramework.hpp>
#include <CubismDefaultParameterId.hpp>
#include <CubismModelSettingJson.hpp>
#include <Model/CubismUserModel.hpp>
#include <Physics/CubismPhysics.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Utils/CubismString.hpp>
#include <Math/CubismViewMatrix.hpp>

/**
*@brief モデルデータのディレクトリ名
* このディレクトリ名と同名の.model3.jsonを読み込む
*/
static const Csm::csmChar* _modelDirectoryName = "Hiyori";

static Csm::CubismUserModel* _userModel; ///< ユーザーが実際に使用するモデル
static Csm::CubismModelSettingJson* _modelJson; ///< モデルの設定情報
static Csm::CubismModel* _model; ///< Mocデータから作成されるモデルデータ

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


Csm::csmFloat32 _dragX;                         ///< マウスドラッグのX位置
Csm::csmFloat32 _dragY;                         ///< マウスドラッグのY位置
Csm::csmFloat32 _accelerationX;                 ///< X軸方向の加速度
Csm::csmFloat32 _accelerationY;                 ///< Y軸方向の加速度
Csm::csmFloat32 _accelerationZ;                 ///< Z軸方向の加速度

static Csm::CubismFramework::Option _cubismOption; ///< CubismFrameworkに関するオプション
static LAppAllocator _cubismAllocator; ///< メモリのアロケーター

static LAppTextureManager* _textureManager; ///< テクスチャの管理

static std::string _currentModelDirectory; ///< 現在のモデルのディレクトリ名
const Csm::csmChar* _currentModelDirectoryChar; ///< 現在のモデルのディレクトリ名のconst csmCharポインタ型

static GLFWwindow* _window; ///< ウィンドウオブジェクト

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
    _cubismOption.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    Csm::CubismFramework::Initialize();
}

/**
* @brief システムの初期化
*
* 基盤システムの初期化処理を行う
*/
static bool InitializeSystem()
{
    LAppPal::PrintLogLn("START");

    // GLFWの初期化
    if (glfwInit() == GL_FALSE)
    {
        LAppPal::PrintLogLn("Can't initilize GLFW");

        return GL_FALSE;
    }

    // Windowの生成
    _window = glfwCreateWindow(LAppDefine::RenderTargetWidth, LAppDefine::RenderTargetHeight, "SIMPLE_SAMPLE", NULL, NULL);
    if (_window == NULL)
    {
        LAppPal::PrintLogLn("Can't create GLFW window.");

        glfwTerminate();
        return GL_FALSE;
    }

    // Windowのコンテキストをカレントに設定
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        LAppPal::PrintLogLn("Can't initilize glew.");

        glfwTerminate();
        return GL_FALSE;
    }

    //テクスチャサンプリング設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //透過設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // コールバック関数の登録
    glfwSetMouseButtonCallback(_window, EventHandler::OnMouseCallBack);
    glfwSetCursorPosCallback(_window, EventHandler::OnMouseCallBack);

    // ウィンドウサイズ記憶
    glfwGetWindowSize(_window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    // Cubism SDK の初期化
    InitializeCubism();

    // ドラッグ入力管理クラスの初期化
    MouseActionManager::GetInstance()->Initialize(windowWidth, windowHeight);

    return GL_TRUE;
}

/**
* @brief 解放
*
* 解放の処理を行う
*/
void Release()
{
    // レンダラの解放
    _userModel->DeleteRenderer();

    // モデルデータの解放
    delete _userModel;

    // テクスチャマネージャーの解放
    delete _textureManager;

    // Windowの削除
    glfwDestroyWindow(_window);

    // OpenGLの処理を終了
    glfwTerminate();

    // MouseActionManagerの解放
    MouseActionManager::ReleaseInstance();

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

    // ユーザーモデルをMouseActionManagerへ渡す
    MouseActionManager::GetInstance()->SetUserModel(_userModel);
}

void Run()
{
    //メインループ
    while (glfwWindowShouldClose(_window) == GL_FALSE)
    {
        int width, height;

        // ビューポートを現在のウィンドウサイズに設定
        glViewport(0, 0, windowWidth, windowHeight);

        // ウィンドウサイズ記憶
        glfwGetWindowSize(_window, &width, &height);
        if ((windowWidth != width || windowHeight != height) && width > 0 && height > 0)
        {
            //AppViewの初期化
            MouseActionManager::GetInstance()->ViewInitialize(width, height);
            // サイズを保存しておく
            windowWidth = width;
            windowHeight = height;

            // ビューポート変更
            glViewport(0, 0, width, height);
        }

        // 時間更新
        LAppPal::UpdateTime();

        // 画面の初期化
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);

        // モデルの更新及び描画
        static_cast<CubismUserModelExtend*>(_userModel)->ModelOnUpdate(_window);

        // バッファの入れ替え
        glfwSwapBuffers(_window);

        // Poll for and process events
        glfwPollEvents();
    }
}

int main()
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
