//
//  EditorScene.h
//  cube3d
//
//  Created by chenbingfeng on 15/6/9.
//
//

#ifndef __cube3d__EditorScene__
#define __cube3d__EditorScene__

#include "cocos2d.h"
#include <iostream>

class EditorScene : public cocos2d::Scene
{
public:
    virtual bool init();
    CREATE_FUNC(EditorScene);

private:

    cocos2d::Layer* _3dLayer;
    cocos2d::Layer* _uiLayer;
    cocos2d::Camera* _camera;
    cocos2d::Label* _stateLabel;
    cocos2d::EventListenerMouse* _mouseListener;
    cocos2d::EventListenerKeyboard* _keyboardListener;

    bool _isFPS = false;
    cocos2d::Vec2 _fpsAnchor;
    float _rotateY = 0.f;
    float _rotateX = 0.f;
    void _updateStateLabel(){
        std::stringstream str;
        str << (_isFPS ? "FPS":"") <<std::endl;
        _stateLabel->setString(str.str());
    }

    virtual void update(float dt) override;
    void up();
    void left();
    void down();
    void right();
};

#endif /* defined(__cube3d__EditorScene__) */
