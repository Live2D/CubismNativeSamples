/*
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

//Cubism
#include "LAppLive2DManager.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include <Rendering/CubismRenderer.hpp>
#ifdef CSM_TARGET_ANDROID_ES2
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#endif

//cocos2d
#include "base/CCDirector.h"

using namespace Csm;
using namespace LAppDefine;

USING_NS_CC;

namespace {
LAppLive2DManager* s_instance = NULL;
}

LAppLive2DManager* LAppLive2DManager::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppLive2DManager();
    }

    return s_instance;
}

void LAppLive2DManager::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

LAppLive2DManager::LAppLive2DManager()
    : _sceneIndex(0)
    , _viewMatrix(NULL)
{
    assert(CubismFramework::IsStarted());

    CubismFramework::Initialize();

#ifdef CSM_TARGET_ANDROID_ES2
    char *exts = (char*)glGetString(GL_EXTENSIONS);
    if(strstr(exts, "GL_NV_shader_framebuffer_fetch ")){
        Rendering::CubismRenderer_OpenGLES2::SetExtShaderMode( true , true );
    }
#endif

    ChangeScene(_sceneIndex);
}

LAppLive2DManager::~LAppLive2DManager()
{
    ReleaseAllModel();
    CubismFramework::Dispose();
}

void LAppLive2DManager::ReleaseAllModel()
{
    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
            delete _models[i];
    }
    
    _models.Clear();
}

LAppModel* LAppLive2DManager::GetModel(csmUint32 no) const
{
    if(no < _models.GetSize())
    {
        return _models[no];
    }

    return NULL;
}

csmUint32 LAppLive2DManager::GetModelCount() const
{
    return _models.GetSize();
}

void LAppLive2DManager::RecreateRenderer() const
{
    Rendering::CubismRenderer::StaticRelease();

    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        LAppModel* model = GetModel(i);
        model->ReloadRnederer();
    }
}

void LAppLive2DManager::SetViewMatrix(Csm::CubismMatrix44* matrix)
{
    _viewMatrix = matrix;
}


void LAppLive2DManager::OnDrag(csmFloat32 x, csmFloat32 y) const
{
    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        LAppModel* model = GetModel(i);

        model->SetDragging(x, y);
    }
}

void LAppLive2DManager::OnTap(csmFloat32 x, csmFloat32 y)
{
    if (DebugLogEnable) LAppPal::PrintLog("[APP]tap point: {x:%.2f y:%.2f}", x, y);

    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        if (_models[i]->HitTest(HitAreaNameHead, x, y))
        {
            if (DebugLogEnable) LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameHead);
            _models[i]->SetRandomExpression();
        }
        else if (_models[i]->HitTest(HitAreaNameBody, x, y))
        {
            if (DebugLogEnable) LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameBody);
            _models[i]->StartRandomMotion(MotionGroupTapBody, PriorityNormal);
        }
    }
}

void LAppLive2DManager::OnUpdate() const
{
    CubismMatrix44 projection;
    Director* director = Director::getInstance();
    Size window = director->getWinSize();
    projection.Scale(1, window.width / window.height);

    if (_viewMatrix != NULL)
    {
        projection.MultiplyByMatrix(_viewMatrix);
    }

    const CubismMatrix44    saveProjection = projection;
    for (csmUint32 i = 0; i < _models.GetSize(); ++i)
    {
        LAppModel* model = GetModel(i);
        projection = saveProjection;

        model->Update();
        model->Draw(projection);///< 参照渡しなのでprojectionは変質する
    }
}

void LAppLive2DManager::NextScene()
{
    csmInt32 no = (_sceneIndex + 1) % ModelDirSize;
    ChangeScene(no);
}

void LAppLive2DManager::ChangeScene(Csm::csmInt32 index)
{
    _sceneIndex = index;
    if (DebugLogEnable) LAppPal::PrintLog("[APP]model index: %d", _sceneIndex);

    // ModelDir[]に保持したディレクトリ名から
    // model3.jsonのパスを決定する.
    // ディレクトリ名とmodel3.jsonの名前を一致させておくこと.
    std::string dir = ModelDir[index];
    dir += "/";
    std::string modelJsonName = ModelDir[index];
    modelJsonName += ".model3.json";

    ReleaseAllModel();
    _models.PushBack(new LAppModel());
    _models[0]->LoadAssets(dir.c_str(), modelJsonName.c_str());
}
