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
    const float MOVE_SCALE = 1.f;
    const float UP_DOWN_MAX = 0.99f;
    const float ROTATE_SCALE = 0.01f;
    const float SCALL_MOVE_SCALE = 0.1f;

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

    bool _up = false;
    bool _down = false;
    bool _left = false;
    bool _right = false;
    bool _zoomIn = false;
    bool _zoomOut = false;

    virtual void update(float dt) override;
    void up();
    void left();
    void down();
    void right();
};

#endif /* defined(__cube3d__EditorScene__) */
