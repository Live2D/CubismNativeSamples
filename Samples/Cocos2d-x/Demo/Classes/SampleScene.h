/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#ifndef __SAMPLE_SCENE_H__
#define __SAMPLE_SCENE_H__

#include "cocos2d.h"

class SampleScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuChangeCallback(cocos2d::Ref* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(SampleScene);

};

#endif // __SAMPLE_SCENE_H__
