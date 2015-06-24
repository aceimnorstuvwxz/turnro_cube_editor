//
//  LogoScene.cpp
//  mcdrunning
//
//  Created by chenbingfeng on 15/5/30.
//
//

#include "LogoScene.h"
#include "Common.h"
#include "EditorScene.h"
#include "WelcomeScene.h"
#include <vector>

USING_NS_CC;

Scene* LogoScene::createScene()
{
    auto scene = Scene::create();

    auto layer = LogoScene::create();

    scene->addChild(layer);

    return scene;
}

bool LogoScene::init()
{
    if (!Layer::init()){
        return false;
    }
    Size windowsSize = Director::getInstance()->getVisibleSize();
    auto center =Vec2{windowsSize.width, windowsSize.height} * 0.5f;
    auto& imagePairs = logoImages;

    // bg
    auto spbg = Sprite::create("images/white_bg.png");
    scale2size(spbg, windowsSize);
    spbg->setPosition(center);
    this->addChild(spbg, 0);

    std::vector<Sprite*> sprites;
    sprites.reserve(imagePairs.size());
    float delaySum = 0.f;
    for (int i = 0; i < imagePairs.size(); i++) {
        auto& pair = imagePairs[i];
        CCLOG("%s %f", pair.first.c_str(), pair.second);
        auto sp = Sprite::create(pair.first);
        sp->setPosition(center);
//        scale2size(sp, windowsSize);
        sp->setOpacity(0);
        this->addChild(sp, 1);
        auto actWait = DelayTime::create(delaySum);
        auto actWait2 = DelayTime::create(pair.second*full_show);
        delaySum += pair.second;
        float fadeTime = pair.second * (1.f - full_show) * 0.5f;
        auto fadeIn = FadeIn::create(fadeTime);
        auto fadeOut = FadeOut::create(fadeTime);
        auto seq = Sequence::create(actWait, fadeIn, actWait2, fadeOut, NULL);
//        if ( i == imagePairs.size() - 1){
//            seq = Sequence::create(actWait, fadeIn, actWait2, NULL);
//        }
        sp->runAction(seq);
    }
    scheduleOnce([&delaySum](float){Director::getInstance()->replaceScene(WelcomeScene::create());}, delaySum, "editor_scene");

    return true;
}