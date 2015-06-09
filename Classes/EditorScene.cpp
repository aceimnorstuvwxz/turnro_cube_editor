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

    auto sp3d = Sprite3D::create("3d/jingbix.c3b");
    sp3d->setPosition3D({0, 0, 10});
    _3dLayer->addChild(sp3d);
    sp3d->setScale(4.f);

    // cubepack
    CubeCenter::get()->initWithTest();
    for (int i = 0; i < CubeCenter::get()->cubePack.cubes.size(); i++) {
        Cube cube =CubeCenter::get()->cubePack.cubes[i];
        auto sp3d = Sprite3D::create("3d/jingbix.c3b");
        float length_scale = 10.f;
        sp3d->setPosition3D(Vec3{static_cast<float>(cube.x), static_cast<float>(cube.y), static_cast<float>(cube.z)}*length_scale);
        sp3d->setScale(4.f);
        _3dLayer->addChild(sp3d);
    }

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
            case EventKeyboard::KeyCode::KEY_C:
            case EventKeyboard::KeyCode::KEY_CAPITAL_C:
                _isFPS = !_isFPS;
                CCLOG("key C, switch _isFPS");
                break;

            case EventKeyboard::KeyCode::KEY_SPACE:
                CCLOG("key space, shot");
                //TODO
                break;

            case EventKeyboard::KeyCode::KEY_W:
            case EventKeyboard::KeyCode::KEY_CAPITAL_W:
                CCLOG("key W");
                //todo
                break;

            case EventKeyboard::KeyCode::KEY_S:
            case EventKeyboard::KeyCode::KEY_CAPITAL_S:
                CCLOG("key s");
                //todo
                break;

            case EventKeyboard::KeyCode::KEY_A:
            case EventKeyboard::KeyCode::KEY_CAPITAL_A:
                CCLOG("key a");
                //todo
                break;

            case EventKeyboard::KeyCode::KEY_D:
            case EventKeyboard::KeyCode::KEY_CAPITAL_D:
                CCLOG("key d");
                //todo
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
        _rotateY -= 0.02f*diff.x;
        _rotateX -= 0.02f*diff.y;
        if (_rotateX > 0.95f*0.5f*PI) _rotateX = 0.95f*0.5f*PI;
        if (_rotateX < -0.95f*0.5f*PI) _rotateX = -0.95f*0.5f*PI;
        CCLOG("%f, %f, %f, %f", diff.x, diff.y, _rotateX, _rotateY);

        Quaternion qua = {Vec3(1.f,0.f,0.f), _rotateX};
        Quaternion quaRes = {Vec3(0.f, 1.f, 0.f), _rotateY};
        quaRes.multiply(qua);
        _camera->setRotationQuat(quaRes);
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
    _mouseListener->onMouseScroll = [=](Event *event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);

    scheduleUpdate();
    return true;
}


void EditorScene::update(float dt)
{
    _updateStateLabel();
    Quaternion qua = {Vec3(1.f,0.f,0.f), _rotateX};
    Quaternion quaRes = {Vec3(0.f, 1.f, 0.f), _rotateY};
    quaRes.multiply(qua);
    _camera->setRotationQuat(quaRes);
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