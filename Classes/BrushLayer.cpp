// 湍流游戏

#include "BrushLayer.h"
#include "CubeSprite.h"
#include "EditState.h"

USING_NS_CC;

bool BrushLayer::init()
{
    assert(McdLayer::init());

    // camera
    auto size = Director::getInstance()->getVisibleSize();
    // todo use orthographic camera
    //    auto _camera = Camera::createOrthographic(size.width/2, size.height/2, 1, 1000);

    _camera = Camera::createPerspective(60, size.width/size.height, 1, 1000);
    _camera->setPosition3D({0, 0, 150});
    _camera->lookAt({0,0,0});
    _camera->setCameraFlag(CameraFlag::USER1);
    this->addChild(_camera);
    this->setCameraMask((unsigned short)CameraFlag::USER1);

    // interact
    auto listener = EventListenerMouse::create();
    listener->onMouseDown = [this](Event *event){
        const float clickScope = 20;
        EventMouse* e = (EventMouse*)event;
        auto mousePos = Vec2{e->getLocationInView().x, 0.f - e->getLocationInView().y};
        if (_selectedCube) {
            auto spos = _camera->project(_selectedCube->getPosition3D());
            auto dis = mousePos -  spos;
            if (dis.length() < clickScope) {
                CCLOG("click _selected cube");
                _isCubesVisible = !_isCubesVisible;
                hideOrShowAllMetaCubes();
            }
        }
        if (_isCubesVisible) {
            for (auto iter = _cubeMap.begin(); iter != _cubeMap.end(); iter++) {
                CubeSprite* sp =  iter->second;
                auto spos = _camera->project(sp->getPosition3D());
                auto dis = mousePos - spos;
                if (dis.length() < clickScope) {
                    CCLOG("click cube %d", sp->getMetaCubeId());
                    setNewSelectedCube(iter->second->getMetaCubeId());
                    _isCubesVisible = false;
                    hideOrShowAllMetaCubes();
                }
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void BrushLayer::reload()
{
    // clear
    for (auto iter = _cubeMap.begin(); iter != _cubeMap.end(); iter++)
    {
        this->removeChild(iter->second);
    }
    _cubeMap.clear();

    // load
    auto metaMap = EditState::s()->getMetaCubeMap();
    int tmpIndex = 0;
    for (auto iter = metaMap->begin(); iter != metaMap->end(); iter++, tmpIndex++) {
        auto sp = CubeSprite::create({0,0,0}, iter->second.id);
        float scp =15;
        sp->setPosition3D({0.f - 2 * scp + tmpIndex%5 * scp, 0-3*scp + tmpIndex/5 * scp, 0});
        sp->setCameraMask(this->getCameraMask());
        this->addChild(sp);
        _cubeMap[iter->second.id] = sp;
        sp->runAction(RepeatForever::create(RotateBy::create(2.f, {0,360,0})));
        sp->runAction(FadeOut::create(0.33));

        //first one as selected
        if (tmpIndex == 0) {
            setNewSelectedCube(iter->second.id);
        }
    }
}

void BrushLayer::setNewSelectedCube(int metaCubeId)
{
    //clear
    if (_selectedCube) {
        this->removeChild(_selectedCube);
        _selectedCube = nullptr;
    }
    //load
    float scp =15;
    _selectedCube = CubeSprite::create({0,0,0}, metaCubeId);
    _selectedCube->setPosition3D({0.f, 0-4*scp, 0});
    _selectedCube->setCameraMask(this->getCameraMask());
    this->addChild(_selectedCube);
    _selectedCube->runAction(RepeatForever::create(RotateBy::create(2.f, {0,360,0})));
    _selectedMetaCubeId = metaCubeId;
}

void BrushLayer::hideOrShowAllMetaCubes()
{
    for (auto iter = _cubeMap.begin(); iter != _cubeMap.end(); iter++) {
        if (_isCubesVisible) {
            iter->second->runAction(FadeIn::create(0.33f));
        } else {
            iter->second->runAction(FadeOut::create(0.33f));
        }
    }
}

