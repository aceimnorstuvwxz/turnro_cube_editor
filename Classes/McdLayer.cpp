//
//  McdLayer.cpp
//  mcdrunning
//
//  Created by chenbingfeng on 15/5/31.
//
//

#include "McdLayer.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

ui::Button* McdLayer::decorateButton(const Btn_info& btnInfo , int tag)
{
    auto button = ui::Button::create(btnInfo.img, btnInfo.img_p);
    button->setPressedActionEnabled(true);
    //    button->setAnchorPoint({0,0});
    auto size = Director::getInstance()->getVisibleSize();
    button->setPosition(btnInfo.pos * size);
    button->setTag(tag);
    button->addTouchEventListener(CC_CALLBACK_2(McdLayer::touchEvent, this));
    button->setScale(btnInfo.scale);
    this->addChild(button, 1);
    return button;
}

void McdLayer::touchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type){

    auto button = static_cast<cocos2d::ui::Button*>(pSender);
    int tag = button->getTag();
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED) {
        return;
    }
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/click.wav");
    this->onButtonClick(tag);
}

cocos2d::Sprite* McdLayer::decorateImg(const std::string& img, const cocos2d::Vec2& pos)
{
    auto sp = Sprite::create(img);
    sp->setPosition(pos * Director::getInstance()->getVisibleSize());
    this->addChild(sp, 2);
    return sp;
}

void McdLayer::withInOut(cocos2d::Node* node)
{
    _inoutNodes.push_back(node);
    node->setOpacity(0);
}

void McdLayer::turnIn(float dt)
{
    auto fadeIn = FadeIn::create(dt);
    for (auto node : _inoutNodes) {
        node->runAction(fadeIn->clone());
    }
}

void McdLayer::turnOut(float dt)
{
    auto fadeOut = FadeOut::create(dt);
    for (auto node : _inoutNodes){
        node->runAction(fadeOut->clone());
    }
}