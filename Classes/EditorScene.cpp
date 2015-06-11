//
//  EditorScene.cpp
//  cube3d
//
//  Created by chenbingfeng on 15/6/9.
//
//

#include "EditorScene.h"
#include "McdLayer.h"
#include "cubedef.h"
#include "CubeSprite.h"

USING_NS_CC;
#define PI 3.1415926f

class EditorUILayer: public McdLayer
{
public:
    virtual bool init() override{
        if (!McdLayer::init()) return false;

//        decorateButton(_btnInfo, BTN_A);
        return true;
    };

    CREATE_FUNC(EditorUILayer);

protected:
    const Btn_info _btnInfo = {"images/btn_a.png","images/btn_a.png",{0.1f,0.1f},1.f};

    enum _BTN_TAG{
        BTN_A
    };
    void onButtonClick(int tag) override{

    }
};

bool EditorScene::init()
{
    if (!Scene::init()){
        return false;
    }

    auto size = Director::getInstance()->getVisibleSize();
    // 3d
    _3dLayer = Layer::create();
    this->addChild(_3dLayer, 1);
    auto bg = BillBoard::create("images/logo_only.png");
    bg->setPosition3D({0,0,0});
    _3dLayer->addChild(bg);
    bg->setScale(0.5f);


//    // cubepack
//    CubeCenter::get()->initWithTest();
//    for (int i = 0; i < CubeCenter::get()->cubePack.cubes.size(); i++) {
//        Cube cube =CubeCenter::get()->cubePack.cubes[i];
//        auto sp3d = Sprite3D::create("3d/jingbix.c3b");
//        float length_scale = 10.f;
//        sp3d->setPosition3D(Vec3{static_cast<float>(cube.x), static_cast<float>(cube.y), static_cast<float>(cube.z)}*length_scale);
//        sp3d->setScale(4.f);
//        _3dLayer->addChild(sp3d);
//    }

    // cube sprite
    auto cubeSprite = CubeSprite::create(Vec3(0,0,0), Color4B::WHITE);
    cubeSprite->setPosition3D({0,0,20});
    cubeSprite->setScale(2.f);
    _3dLayer->addChild(cubeSprite);

    // camera
    _camera = Camera::createPerspective(60, size.width/size.height, 1, 1000);
    _camera->setPosition3D({0, 0, 200});
    _camera->lookAt({0,0,0});
    _camera->setCameraFlag(CameraFlag::USER1);
    _3dLayer->addChild(_camera);
    _3dLayer->setCameraMask((unsigned short)CameraFlag::USER1);

    // UI layer
    _uiLayer = EditorUILayer::create();
    this->addChild(_uiLayer, 2);

    // state label
    _stateLabel = Label::createWithTTF("state string", "fonts/consola.ttf", 14);
    _stateLabel->setPosition(size * Vec2{0.1f,0.9f});
    _uiLayer->addChild(_stateLabel, 0);

    // keyboard
    _keyboardListener = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed = [&](EventKeyboard::KeyCode code, Event* event){
        switch (code) {
            case EventKeyboard::KeyCode::KEY_SPACE:
                CCLOG("key space, shot");
                //TODO
                break;

            case EventKeyboard::KeyCode::KEY_W:
            case EventKeyboard::KeyCode::KEY_CAPITAL_W:
                CCLOG("key W down");
                _up = true;
                break;

            case EventKeyboard::KeyCode::KEY_S:
            case EventKeyboard::KeyCode::KEY_CAPITAL_S:
                CCLOG("key S down");
                _down = true;
                break;

            case EventKeyboard::KeyCode::KEY_A:
            case EventKeyboard::KeyCode::KEY_CAPITAL_A:
                CCLOG("key A down");
                _left = true;
                break;

            case EventKeyboard::KeyCode::KEY_D:
            case EventKeyboard::KeyCode::KEY_CAPITAL_D:
                CCLOG("key D down");
                _right = true;
                break;

            case EventKeyboard::KeyCode::KEY_Q:
            case EventKeyboard::KeyCode::KEY_CAPITAL_Q:
                CCLOG("key Q down");
                _zoomOut = true;
                break;

            case EventKeyboard::KeyCode::KEY_E:
            case EventKeyboard::KeyCode::KEY_CAPITAL_E:
                CCLOG("key E down");
                _zoomIn = true;
                break;

            case EventKeyboard::KeyCode::KEY_UP_ARROW:
                up();
                break;
            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                down();
                break;
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                left();
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                right();
                break;

            default:
                break;
        }
    };
    _keyboardListener->onKeyReleased = [&](EventKeyboard::KeyCode code, Event* event){
        switch (code) {
            case EventKeyboard::KeyCode::KEY_SPACE:
                CCLOG("key space, shot");
                //TODO
                break;

            case EventKeyboard::KeyCode::KEY_W:
            case EventKeyboard::KeyCode::KEY_CAPITAL_W:
                CCLOG("key W up");
                _up = false;
                break;

            case EventKeyboard::KeyCode::KEY_S:
            case EventKeyboard::KeyCode::KEY_CAPITAL_S:
                CCLOG("key S up");
                _down = false;
                break;

            case EventKeyboard::KeyCode::KEY_A:
            case EventKeyboard::KeyCode::KEY_CAPITAL_A:
                CCLOG("key A up");
                _left = false;
                break;

            case EventKeyboard::KeyCode::KEY_D:
            case EventKeyboard::KeyCode::KEY_CAPITAL_D:
                CCLOG("key D up");
                _right = false;
                break;
            case EventKeyboard::KeyCode::KEY_Q:
            case EventKeyboard::KeyCode::KEY_CAPITAL_Q:
                CCLOG("key Q up");
                _zoomOut = false;
                break;

            case EventKeyboard::KeyCode::KEY_E:
            case EventKeyboard::KeyCode::KEY_CAPITAL_E:
                CCLOG("key E up");
                _zoomIn = false;
                break;

//            case EventKeyboard::KeyCode::KEY_UP_ARROW:
//                up();
//                break;
//            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
//                down();
//                break;
//            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
//                left();
//                break;
//            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
//                right();
//                break;

            default:
                break;
        }

    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

    // mouse
    _mouseListener = EventListenerMouse::create();

    _mouseListener->onMouseMove = [&](Event *event){
        if (!_isFPS) return;

        EventMouse* e = (EventMouse*)event;
        auto now = e->getLocation();
        Vec2 diff = now - _fpsAnchor;
        _fpsAnchor = now;
        _rotateY -= ROTATE_SCALE*diff.x;
        _rotateX -= ROTATE_SCALE*diff.y;
        if (_rotateX > UP_DOWN_MAX*0.5f*PI) _rotateX = UP_DOWN_MAX*0.5f*PI;
        if (_rotateX < -UP_DOWN_MAX*0.5f*PI) _rotateX = -UP_DOWN_MAX*0.5f*PI;

    };
    _mouseListener->onMouseUp = [&](Event *event){
        EventMouse* e = (EventMouse*)event;
        switch(e->getMouseButton()){
            case MOUSE_BUTTON_LEFT:
                break;

            case MOUSE_BUTTON_RIGHT:
                _isFPS = false;
                break;

            default:
                break;
        }
    };
    _mouseListener->onMouseDown = [&](Event *event){
        EventMouse* e = (EventMouse*)event;
        switch(e->getMouseButton()){
            case MOUSE_BUTTON_LEFT:
                break;

            case MOUSE_BUTTON_RIGHT:
            {
                _isFPS = true;
                _fpsAnchor = e->getLocation();
                auto ro = _camera->getRotation3D();
                CCLOG("%f,%f,%f", ro.x, ro.y, ro.z);
                break;
            }
            default:
                break;
        }
    };
    _mouseListener->onMouseScroll = [&](Event *event){
        EventMouse* e = (EventMouse*)event;
        CCLOG("%f, %f", e->getScrollX(), e->getScrollY());
        float diff = e->getScrollY();
        Vec3 dir = _camera->getRotationQuat() * Vec3{0.f, 0.f, -1.f};
        dir.normalize();
        _camera->setPosition3D(_camera->getPosition3D() + SCALL_MOVE_SCALE * -diff * dir);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);

    scheduleUpdate();
    return true;
}


void EditorScene::update(float dt)
{
    _updateStateLabel();
    Quaternion qua2 = {Vec3(1.f,0.f,0.f), _rotateX};
    Quaternion qua = {Vec3(0.f, 1.f, 0.f), _rotateY};
    qua.multiply(qua2);
    _camera->setRotationQuat(qua);

    qua.normalize();
    if (_up && !_down) {
        Vec3 dir = qua * Vec3{0.f,1.f,0.f};
        dir.normalize();
        _camera->setPosition3D(_camera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_down && !_up) {
        Vec3 dir = qua * Vec3{0.f,-1.f,0.f};
        dir.normalize();
        _camera->setPosition3D(_camera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_left && !_right) {
        Vec3 dir = qua * Vec3{-1.f,0.f,0.f};
        dir.normalize();
        _camera->setPosition3D(_camera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_right && !_left) {
        Vec3 dir = qua * Vec3{1.f,0.f,0.f};
        dir.normalize();
        _camera->setPosition3D(_camera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_zoomIn && !_zoomOut) {
        Vec3 dir = qua * Vec3{0.f,0.f,-1.f};
        dir.normalize();
        _camera->setPosition3D(_camera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_zoomOut && !_zoomIn) {
        Vec3 dir = qua * Vec3{0.f,0.f,1.f};
        dir.normalize();
        _camera->setPosition3D(_camera->getPosition3D() + MOVE_SCALE * dir);
    }
}

void EditorScene::up()
{
    _rotateX += PI * 1.f/30.f;
}

void EditorScene::down()
{
    _rotateX -= PI * 1.f/30.f;
}

void EditorScene::right()
{
    _rotateY -=PI * 1.f/30.f;
}
void EditorScene::left()
{
    _rotateY +=PI * 1.f/30.f;
}