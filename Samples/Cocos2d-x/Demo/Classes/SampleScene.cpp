/*
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "SampleScene.h"
#include "LAppView.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"

USING_NS_CC;

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

// on "init" you need to initialize your instance
bool SampleScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(SampleScene::menuCloseCallback, this));

    closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
                                 origin.y + closeItem->getContentSize().height / 2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);

    Sprite* pSprite = Sprite::create(LAppDefine::BackImageName);

    // position the sprite on the center of the screen
    pSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    pSprite->setScale(2);

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);

    MenuItemImage* pChangeItem = MenuItemImage::create(
        "icon_gear.png",
        "icon_gear.png",
        CC_CALLBACK_1(SampleScene::menuChangeCallback, this));

    pChangeItem->setPosition(Point(origin.x + visibleSize.width - pChangeItem->getContentSize().width / 2,
                                   getContentSize().height - pChangeItem->getContentSize().height / 2));

    // create menu, it's an autorelease object
    Menu* pChangeMenu = Menu::create(pChangeItem, NULL);
    pChangeMenu->setPosition(Point::ZERO);
    this->addChild(pChangeMenu, 1);

    // add "Live2DModel"
    auto* pLive2DSprite = LAppView::createDrawNode();
    this->addChild(pLive2DSprite);

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
        pLive2DSprite->setDebugRectsNode(pDebugRects);
    }

    // initialize random seed.
    srand(static_cast<unsigned int>(time(NULL)));


    return true;
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
