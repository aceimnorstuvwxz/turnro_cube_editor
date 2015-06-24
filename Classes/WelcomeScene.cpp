// 世界是我的表象，世界是我的意志。

#include "WelcomeScene.h"
#include "format.h"
#include "Msg.h"
#include <string>
#include <vector>
#include <iostream>
#include "uiconf.h"

// 欢迎页，梦开始的地方，欢迎被渲染。
USING_NS_CC;

bool WelcomeScene::init()
{
    assert(TRBaseScene::init());

    // 显示的结构
    addCommonLabel({.5f,.5f}, Msg::s()["welcome"]);
    addCommonLabel({.5f, .23f}, Msg::s()["welcome_desc"], 10);
    _lbInput = Label::createWithTTF("", uic::font_zh, 10);
    _lbInput->setPosition(genPos({.5f,.2f}));
    _defaultLayer->addChild(_lbInput);

    // 键盘的纹理
    // keyboard
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode code, Event* event){
        if (code == EventKeyboard::KeyCode::KEY_SHIFT) {
            _isUpcase = true;
        }
        this->onKeyPressed(code);
    };
    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode code, Event* event){
        if (code == EventKeyboard::KeyCode::KEY_SHIFT) {
            _isUpcase = false;
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    return true;
}

void WelcomeScene::onKeyPressed(const EventKeyboard::KeyCode& code)
{
    if (code == EventKeyboard::KeyCode::KEY_KP_ENTER ||
        code == EventKeyboard::KeyCode::KEY_RETURN ||
        code == EventKeyboard::KeyCode::KEY_ENTER) {
        //enter
        onInputFinish();
    } else if (code == EventKeyboard::KeyCode::KEY_ESCAPE){
        //esc
        _input.clear();
    } else if (code == EventKeyboard::KeyCode::KEY_BACKSPACE ||
               code == EventKeyboard::KeyCode::KEY_DELETE){
        //delete
        _input.pop_back();
    } else {
        //input
        if (code == EventKeyboard::KeyCode::KEY_MINUS) {
            _input += "_";
        }
        if (code >= EventKeyboard::KeyCode::KEY_0 && code <= EventKeyboard::KeyCode::KEY_9) {
            char c = (char)((int)code - (int)EventKeyboard::KeyCode::KEY_0 + 48);
            _input += c;
        }
        if (code >= EventKeyboard::KeyCode::KEY_A && code <= EventKeyboard::KeyCode::KEY_Z) {
            char c = (char)((int)code - (int)EventKeyboard::KeyCode::KEY_A + (_isUpcase ? 65 : 97));
            _input += c;
        }
    }
    _lbInput->setString(_input);
}

void WelcomeScene::onInputFinish()
{
    auto path = FileUtils::getInstance()->getWritablePath();
    CCLOG("%s", path.c_str());

}