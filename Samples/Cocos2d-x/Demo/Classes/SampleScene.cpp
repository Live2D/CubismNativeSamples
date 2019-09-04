/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "SampleScene.h"
#include "LAppView.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"

USING_NS_CC;

namespace
{
    SampleScene* instance = NULL;
}

SampleScene::SampleScene()
    : _pView(NULL)
    , _changeItem(NULL)
    , _closeItem(NULL)
{
}

Scene* SampleScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = SampleScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

SampleScene * SampleScene::getInstance()
{
    return instance;
}

// on "init" you need to initialize your instance
bool SampleScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    // Live2DManager実体化の前に必要となる
    instance = this;

    Size winSize = Director::getInstance()->getWinSize();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    // add a "close" icon to exit the progress. it's an autorelease object
    _closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(SampleScene::menuCloseCallback, this));

    _closeItem->setPosition(Point(origin.x + visibleSize.width - _closeItem->getContentSize().width / 2,
                                 origin.y + _closeItem->getContentSize().height / 2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(_closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);

    Sprite* pSprite = Sprite::create(LAppDefine::BackImageName);

    // position the sprite on the center of the screen
    pSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    pSprite->setScale(2);

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);

    _changeItem = MenuItemImage::create(
        "icon_gear.png",
        "icon_gear.png",
        CC_CALLBACK_1(SampleScene::menuChangeCallback, this));

    _changeItem->setPosition(Point(origin.x + visibleSize.width - _changeItem->getContentSize().width / 2,
                                   getContentSize().height - _changeItem->getContentSize().height / 2));

    // create menu, it's an autorelease object
    Menu* pChangeMenu = Menu::create(_changeItem, NULL);
    pChangeMenu->setPosition(Point::ZERO);
    this->addChild(pChangeMenu, 1);

    // add "Live2DModel"
    _pView = LAppView::createDrawNode();
    this->addChild(_pView);

    // Live2DManager実体化
    LAppLive2DManager::GetInstance();

    if (LAppDefine::DebugLogEnable)
    {
        LAppPal::PrintLog("==============================================");
        LAppPal::PrintLog("            Live2D SDK Sample App             ");
        LAppPal::PrintLog("==============================================");
    }

    if (LAppDefine::DebugDrawRectEnable)
    {
        auto* pDebugRects = DrawNode::create();
        this->addChild(pDebugRects);
        _pView->setDebugRectsNode(pDebugRects);
    }

    // initialize random seed.
    srand(static_cast<unsigned int>(time(NULL)));

    // update有効
    this->scheduleUpdate();

    return true;
}

void SampleScene::update(float delta)
{
    Node::update(delta);

    {
        Size winSize = Director::getInstance()->getWinSize();
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Point origin = Director::getInstance()->getVisibleOrigin();

        if (_changeItem)
        {
            _changeItem->setPosition(Point(origin.x + visibleSize.width - _changeItem->getContentSize().width / 2,
                getContentSize().height - _changeItem->getContentSize().height / 2));
        }

        if (_closeItem)
        {
            _closeItem->setPosition(Point(origin.x + visibleSize.width - _closeItem->getContentSize().width / 2,
                origin.y + _closeItem->getContentSize().height / 2));
        }
    }
}

void SampleScene::onExit()
{
    Node::onExit();

    // LAppModelがRenderingSpriteを使用していることがある。これはCocosのExit以降に開放することが出来ないので、
    // モデルはここですべて消えてもらう
    LAppLive2DManager::GetInstance()->ReleaseAllModel();
}

void SampleScene::menuCloseCallback(Ref* pSender)
{
    LAppLive2DManager::ReleaseInstance();

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void SampleScene::menuChangeCallback(Ref* pSender)
{
    LAppLive2DManager::GetInstance()->NextScene();
}
