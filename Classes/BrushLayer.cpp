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

    _camera = Camera::createPerspective(60, size.width/size.height, 1, 200);
    // 实体CUBE放在Z=0处，为了保持在scene内尽量不被遮挡，camera必须离这些cube很近。
    _camera->setPosition3D({0, 0, 20});
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
        for (auto iter = _cubeMap.begin(); iter != _cubeMap.end(); iter++) {
            CubeSprite* sp =  iter->second;
            auto spos = _camera->project(sp->getPosition3D());
            auto dis = mousePos - spos;
            if (dis.length() < clickScope) {
                CCLOG("click cube %d", sp->getMetaCubeId());
                setSelectedCubeId(iter->second->getMetaCubeId());
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


    // 这个custommand的作用见draw()
    /* 此方法无效
    _customCommand.func = [](){
        glDepthMask(GL_TRUE);
        glClearDepth(1.0);
        glClear(GL_DEPTH_BUFFER_BIT);
    };*/

    // 准心
    auto draw = DrawNode::create();
    this->addChild(draw);

    Vec2 center = {size.width/2, size.height/2};
    draw->drawLine(center-Vec2{20,0}, center+Vec2{20,0}, {0,1.0,0,0.5});
    draw->drawLine(center-Vec2{0,20}, center+Vec2{0,20}, {0,1.0,0,0.5});
    return true;
}


static const float SCALE = 0.07f;
static const float PY = -20*7;
static const float SCP_X = 15;
static const float SCP_Y = 15;
static const int N_LINE_MAX = 32;
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
        sp->setPosition3D(SCALE * Vec3{0.f - N_LINE_MAX/2 * SCP_X + (tmpIndex%N_LINE_MAX) * SCP_X, PY - SCP_Y*(tmpIndex/N_LINE_MAX), 0});
        sp->setCameraMask(this->getCameraMask());
        sp->setScale(SCALE);
        this->addChild(sp);
        _cubeMap[iter->second.id] = sp;
    }
    // 0是必有的metacube
    setSelectedCubeId(0);
}

void BrushLayer::setSelectedCubeId(int metaCubeId)
{
    CubeSprite* lastSelected = _cubeMap[_selectedMetaCubeId];
    lastSelected->stopAllActions();
    CubeSprite* nowSelected = _cubeMap[metaCubeId];
    nowSelected->runAction(RepeatForever::create(RotateBy::create(2.f, {0,360,0})));
    _selectedMetaCubeId = metaCubeId;
}

void BrushLayer::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    // 为了让此Layer不与其他Layer的内部模型之间进行深度测试（在合理安排Layer的先后addChild次序的情况下，能够保证本Layer之内的元素始终在其它之上）

    // 发送一个 RenderCommand，它execute时，执行glClear(GL_DEPTH_BUFFER_BIT)
    // 因为renderCommad会sort而无效。
//    renderer->addCommand(&_customCommand);
}

