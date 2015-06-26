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

    auto _camera = Camera::createPerspective(60, size.width/size.height, 1, 1000);
    _camera->setPosition3D({0, 0, 150});
    _camera->lookAt({0,0,0});
    _camera->setCameraFlag(CameraFlag::USER1);
    this->addChild(_camera);
    this->setCameraMask((unsigned short)CameraFlag::USER1);

    auto sp = CubeSprite::create({0,0,0}, 0);
    sp->setPosition3D({0,0,0});
    this->addChild(sp);
    sp->setCameraMask(this->getCameraMask());
    sp->runAction(RepeatForever::create(RotateBy::create(2.f, {0,360,0})));

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
    }
}

