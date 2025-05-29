/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <functional>

#include <CubismFramework.hpp>
#include <CubismModelSettingJson.hpp>

#include "LAppDefine.hpp"
#include "CubismTextureManager.hpp"
#include "Rendering/D3D9/CubismRenderer_D3D9.hpp"
#include "Rendering/D3D9/CubismOffscreenSurface_D3D9.hpp"
#include "LAppModel_Common.hpp"

/**
 * @brief CubismUserModelを継承するサンプルクラス
 *
 * CubismUserModelを継承するサンプルクラス
 * このクラスを拡張し、独自の処理を実装する。
 *
 */
class CubismUserModelExtend : public LAppModel_Common
{
public:
    CubismUserModelExtend(const std::string modelDirectoryName, const std::string _currentModelDirectory); ///< コンストラクタ
    virtual ~CubismUserModelExtend(); ///< デストラクタ

    /**
    * @brief model3.jsonからモデルを生成する
    *
    * model3.jsonの記述に従ってモデル生成、モーション、物理演算などのコンポーネント生成を行う
    *
    * @param[in]   setting     ICubismModelSettingのインスタンス
    *
    */
    void SetupModel();

    /**
    * @brief モデルの更新
    *
    * モデルの状態や描画を更新する
    */
    void ModelOnUpdate();

    /**
    * @brief レンダラを再構築する
    *
    */
    void ReloadRenderer();

    /**
    * @brief   デバイスロスト時に呼ばれる
    */
    void OnDeviceLost();

    /**
    * @brief   別ターゲットに描画する際に使用するバッファの取得
    */
    Csm::Rendering::CubismOffscreenSurface_D3D9& GetRenderBuffer();

private:
    /**
    * @brief パスを作成
    *
    * アセットのパスを作成する
    */
    std::string MakeAssetPath(const std::string & assetFileName) const;

    /**
    * @brief ディレクトリパスの設定
    *
    * モデルのディレクトリパスを設定する
    */
    void SetAssetDirectory(const std::string & path);

    /**
    * @brief アセットのロードを行う
    *
    * 指定されたファイル名からアセットのロードを行う
    */
    void LoadAsset(const std::string & fiileName, const std::function<void(Csm::csmByte*, Csm::csmSizeInt)>& afterLoadCallback) const;

    /**
   * @brief 引数で指定したモーションの再生を開始する
   *
   * @param[in] group                       モーショングループ名
   * @param[in] no                          グループ内の番号
   * @param[in] priority                    優先度
   * @return 開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」
   */
    Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority);

    /**
    * @brief 解放
    *
    * モデルの設定データの解放の処理を行う
    */
    void ReleaseModelSetting();

    /**
    * @brief モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す
    *
    * @param[in]  matrix  View-Projection行列
    */
    void Draw(Csm::CubismMatrix44& matrix);

    /**
    * @brief モデルのパラメータ情報の更新
    *
    * モデルのパラメータの情報を更新する
    */
    void ModelParamUpdate();

    /**
    * @brief テクスチャをロードする
    *
    */
    void SetupTextures();

    /**
    * @brief モーションデータをグループ名から一括でロードする。
    *
    * モーションデータの名前は内部でModelSettingから取得する。
    *
    * @param[in]   group  モーションデータのグループ名
    */
    void PreloadMotionGroup(const Csm::csmChar * group);

    std::string _modelDirName; ///< モデルセッティングが置かれたディレクトリの名称
    std::string _currentModelDirectory; ///< モデルセッティングが置かれたディレクトリ

    Csm::csmFloat32 _userTimeSeconds; ///< デルタ時間の積算値[秒]
    Csm::CubismModelSettingJson* _modelJson; ///< モデルセッティング情報
    Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///< モデルに設定されたまばたき機能用パラメータID
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> _motions; ///< 読み込まれているモーションのリスト
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> _expressions; ///< 読み込まれている表情のリスト

    CubismTextureManager* _textureManager;         ///< テクスチャマネージャー

    const Csm::CubismId* _idParamAngleX; ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamAngleY; ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamAngleZ; ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamBodyAngleX; ///< パラメータID: ParamBodyAngleX
    const Csm::CubismId* _idParamEyeBallX; ///< パラメータID: ParamEyeBallX
    const Csm::CubismId* _idParamEyeBallY; ///< パラメータID: ParamEyeBallXY

    Csm::csmVector<Csm::csmUint64> _bindTextureId; ///< テクスチャID

    Csm::Rendering::CubismOffscreenSurface_D3D9   _renderBuffer;  ///< フレームバッファ以外の描画先
};
