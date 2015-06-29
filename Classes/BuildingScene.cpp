// 湍流游戏


#include <cstdio>
#include <vector>

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/filereadstream.h"
#include "../rapidjson/filewritestream.h"

#include <sys/stat.h> //mkdir

#include "BuildingScene.h"
#include "uiconf.h"
#include "EditState.h"
#include "format.h"
#include "Msg.h"
#include "CubeSprite.h"
#include "EditState.h"

#include "intersection.h"

USING_NS_CC;

class StateInfoLayer: public McdLayer
{
public:
    virtual bool init() override {
        assert(McdLayer::init());

        _lbUnitFile = Label::createWithTTF(EditState::s()->getExtraUnitFilePath(), uic::font_zh, 14);
        _lbUnitFile->setPosition(genPos({0.5f, 0.975f}));
        _lbUnitFile->setTextColor(Color4B::WHITE);
        this->addChild(_lbUnitFile);

        schedule(schedule_selector(StateInfoLayer::onUpdateState), 0.33f, kRepeatForever, 0);

        return true;
    }
    CREATE_FUNC(StateInfoLayer);

private:
    Label* _lbUnitFile;

    void onUpdateState(float dt) {
        _lbUnitFile->setString(EditState::s()->getExtraUnitFilePath());
    }

};




bool BuildingScene::init()
{
    assert(TRBaseScene::init());

    this->addChild(StateInfoLayer::create());

    initSceneLayer(); // Must before BrushLayer, make it always draw later. (zOrder is not working...)

    _brushLayer = BrushLayer::create(); // Must before reloadMetaCubes()
    this->addChild(_brushLayer);

    reloadMetaCubes();

    initMenuButtons();

    loadUnitCubes();

    scheduleUpdate();

    return true;
}

void BuildingScene::reloadMetaCubes()
{

    //clear old
    EditState::s()->getMetaCubeMap()->clear();

    FILE* fp = fopen(EditState::s()->getCubeMetaFilePath().c_str(), "r"); // TODO if Windows using "rb"

    if (fp == NULL) {
        CCLOG("Metacubes not exists, copy template workspace there.");
        copyTemplateWorkspace();
        // reopen
        fp = fopen(EditState::s()->getCubeMetaFilePath().c_str(), "r");
    }

    assert(fp != NULL);
    
    char readBuffer[65536];
    rjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    // extract metacubes
    // defaults
    assert(doc.HasMember("default_shader"));
    auto& defaultShader = doc["default_shader"];
    assert(defaultShader.Size() == 2);
    int shaderIndex = 0;
    for (auto iter = defaultShader.Begin(); iter != defaultShader.End(); iter++, shaderIndex++) {
        if (shaderIndex == 0) {
            EditState::s()->setDefaultVertexShader(iter->GetString());
        } else {
            EditState::s()->setDefaultFragmentShader(iter->GetString());
        }
    }
    assert(doc.HasMember("list"));
    auto& list = doc["list"];
    auto getStr = [](const rjson::Value& obj, const char* key) {
        if (obj.HasMember(key)) {
            assert(obj[key].IsString());
            return std::string(obj[key].GetString());
        }
        return std::string("");
    };
    for (auto iter = list.Begin(); iter != list.End(); iter++) {
        MetaCube metacube;
        //id
        assert(iter->HasMember("id"));
        metacube.id = (*iter)["id"].GetInt();
        //name
        metacube.name = getStr(*iter, "name");
        //comment
        metacube.comment = getStr(*iter, "comment");
        //texture
        metacube.texture = getStr(*iter, "texture");
        //color
        if (iter->HasMember("color")) {
            auto& color = (*iter)["color"];
            assert(color.IsArray() && color.Size() == 4);
            int tmpIndex = 0;
            float colordata[4];
            for (auto i = color.Begin(); i != color.End(); i++, tmpIndex++) {
                float a = i->GetDouble();
                assert(a >= 0.f && a <= 1.f);
                colordata[tmpIndex] = a;
            }
            metacube.color = {colordata[0], colordata[1], colordata[2], colordata[3]};
        }
        //shader
        if (iter->HasMember("shader")) {
            auto& shader = (*iter)["shader"];
            assert(shader.IsArray() && shader.Size() == 2);
            int tmpIndex = 0;
            for (auto i = shader.Begin(); i != shader.End(); i++, tmpIndex++) {
                assert(i->IsString());
                metacube.shader[tmpIndex] = i->GetString();
            }
        }
        //unreal
        if (iter->HasMember("unreal")) {
            auto& unreal = (*iter)["unreal"];
            assert(unreal.IsInt());
            metacube.unreal = unreal.GetInt() != 0;
        }

        metacube.reload();
        (*(EditState::s()->getMetaCubeMap()))[metacube.id] = metacube;
    }

    //extract metacubes from file is done. Now we will display them.
    _brushLayer->reload();
}

void BuildingScene::loadUnitCubes()
{

    FILE* fp = fopen(EditState::s()->getUnitFilePath().c_str(), "r"); // TODO if Windows using "rb"

    if (fp == NULL) {
        CCLOG("Unit file do not exist, load no unit cubes");
        return;
    }

    assert(fp != NULL);

    char readBuffer[65536];
    rjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    // extract unit cubes
    assert(doc.HasMember("list"));
    auto& list = doc["list"];
    for (auto iter = list.Begin(); iter != list.End(); iter++) {
        assert(iter->HasMember("pos") && iter->HasMember("mid"));
        auto& posArr = (*iter)["pos"];
        assert(posArr.Size() == 3);
        int mid = (*iter)["mid"].GetInt();
        Vec3 rawPos = {0, 0, 0};
        int tmpIndex = 0;
        for (auto jter = posArr.Begin(); jter != posArr.End(); jter++, tmpIndex++) {
            if (tmpIndex == 0) {
                rawPos.x = jter->GetInt();
            } else if(tmpIndex ==1 ){
                rawPos.y = jter->GetInt();
            } else {
                rawPos.z = jter->GetInt();
            }
        }
        addCube(CubeSprite::create(rawPos, mid));
    }
}

void BuildingScene::saveUnitCubes()
{
    // buildup json data
    rjson::Document doc;
    doc.SetObject();
    rjson::Value list;
    list.SetArray();
    for (auto iter = _cubes.begin(); iter != _cubes.end(); iter++) {
        rjson::Value obj;
        obj.SetObject();
//        rjson::Value mid;
//        mid.SetInt((*iter)->getMetaCubeId());
        obj.AddMember("mid", (*iter)->getMetaCubeId(), doc.GetAllocator());
        rjson::Value pos;
        pos.SetArray();
        Vec3 rawPos = (*iter)->getRawPos();
        pos.PushBack((int)rawPos.x, doc.GetAllocator()).PushBack((int)rawPos.y, doc.GetAllocator()).PushBack((int)rawPos.z, doc.GetAllocator());
        obj.AddMember("pos", pos, doc.GetAllocator());
        list.PushBack(obj, doc.GetAllocator());
    }
    doc.AddMember("list", list, doc.GetAllocator());

    FILE* fp = fopen(EditState::s()->getUnitFilePath().c_str(), "w");
    assert(fp != NULL);

    char writeBuffer[65536];
    rjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    rjson::Writer<rjson::FileWriteStream> writer(os);
    doc.Accept(writer);
    fclose(fp);
}

void BuildingScene::copyTemplateWorkspace()
{
    auto fileListString = FileUtils::getInstance()->getStringFromFile("MicroCube/filelist.json");
    CCLOG("%s", fileListString.c_str());
    rjson::Document doc;
    doc.Parse(fileListString.c_str());
    std::vector<std::string> dirs;
    std::vector<std::string> files;
    auto& dirlist = doc["directories"];
    for (auto iter = dirlist.Begin(); iter != dirlist.End(); iter++) {
        std::string str = iter->GetString();
        CCLOG("%s", str.c_str());
        dirs.push_back(str);
    }
    auto& filelist = doc["files"];
    for (auto iter = filelist.Begin(); iter != filelist.End(); iter++) {
        std::string str = iter->GetString();
        CCLOG("%s", str.c_str());
        files.push_back(str);
    }
    // create base dir
    mkdir(EditState::s()->getBasePath().c_str(), 0777);
    for (int i = 0; i < dirs.size(); i++) {
        std::string dst = EditState::s()->getBasePath()+dirs[i];
        mkdir(dst.c_str(), 0777);
    }
    for (int i = 0; i < files.size(); i++) {
        auto data = FileUtils::getInstance()->getDataFromFile("MicroCube/" + files[i]);
        std::string dst = EditState::s()->getBasePath()+files[i];
        CCLOG("%s", dst.c_str());
        FILE* fp = fopen((EditState::s()->getBasePath()+files[i]).c_str(), "w");
        assert(fp != NULL);
        fwrite(data.getBytes(), sizeof(unsigned char), data.getSize(), fp);
        fclose(fp);
    }

}

void BuildingScene::initMenuButtons()
{
    // 加地板
    addCommonBtn({0.1f,0.95f}, Msg::s()["unreal_bottom"], [this](){ addUnrealWall(UY, 30);
    });

    // 加墙壁
    addCommonBtn({0.2f,0.95f}, Msg::s()["unreal_wall"], [this](){ addUnrealWall(UZ, 30); });

    // 加入中心点
    addCommonBtn({0.3f,0.95f}, Msg::s()["add_center"], [this](){ addCenterAnchor();
    });

    // 保存
    addCommonBtn({0.9f,0.95f}, Msg::s()["save"], [this](){ saveUnitCubes(); });

}

void BuildingScene::initSceneLayer()
{
    _sceneLayer = Layer::create();
    this->addChild(_sceneLayer);

    auto size = Director::getInstance()->getVisibleSize();

    _sceneCamera = Camera::createPerspective(60, size.width/size.height, 1, 1000);
    _sceneCamera->setPosition3D({0, 0, 200});
    _sceneCamera->lookAt({0,0,0});
    _sceneCamera->setCameraFlag(CameraFlag::USER2);
    _sceneLayer->addChild(_sceneCamera);
    _sceneLayer->setCameraMask((unsigned short)CameraFlag::USER2);

    // Mouse and keyboard

    // keyboard
    auto _keyboardListener = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed = [&](EventKeyboard::KeyCode code, Event* event){
        switch (code) {
            case EventKeyboard::KeyCode::KEY_SPACE:
            CCLOG("key space, shot");
            addAnCubeAlignSelectedFace();
            break;

            case EventKeyboard::KeyCode::KEY_W:
            case EventKeyboard::KeyCode::KEY_CAPITAL_W:
            CCLOG("key W down");
            _front = true;
            break;

            case EventKeyboard::KeyCode::KEY_S:
            case EventKeyboard::KeyCode::KEY_CAPITAL_S:
            CCLOG("key S down");
            _back = true;
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
            _down = true;
            break;

            case EventKeyboard::KeyCode::KEY_E:
            case EventKeyboard::KeyCode::KEY_CAPITAL_E:
            CCLOG("key E down");
            _up = true;
            break;

            case EventKeyboard::KeyCode::KEY_C:
            case EventKeyboard::KeyCode::KEY_CAPITAL_C:
                CCLOG("key C down");
                deleteTheSelectedCube();
                break;

            case EventKeyboard::KeyCode::KEY_V:
            case EventKeyboard::KeyCode::KEY_CAPITAL_V:
                CCLOG("key v down");
                _mouseDeleting = true;
                break;

            case EventKeyboard::KeyCode::KEY_Z:
            case EventKeyboard::KeyCode::KEY_CAPITAL_Z:
                CCLOG("key z down");
                undoOrder();
                break;

            case EventKeyboard::KeyCode::KEY_B:
            case EventKeyboard::KeyCode::KEY_CAPITAL_B:
                CCLOG("key b down");
                _mourseMetaCubeSelect = true;
                break;

            case EventKeyboard::KeyCode::KEY_UP_ARROW:
                CCLOG("key arrow up");
                _rotateUp = true;
                break;

            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                CCLOG("key arrow down");
                _rotateDown = true;
                break;

            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                CCLOG("key arrow left");
                _rotateLeft = true;
                break;

            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                CCLOG("key arrow right");
                _rotateRight = true;
                break;

            default:
            break;
        }
    };
    _keyboardListener->onKeyReleased = [&](EventKeyboard::KeyCode code, Event* event){
        switch (code) {
            case EventKeyboard::KeyCode::KEY_SPACE:
                CCLOG("key space, shot");
                break;

            case EventKeyboard::KeyCode::KEY_W:
            case EventKeyboard::KeyCode::KEY_CAPITAL_W:
            CCLOG("key W up");
            _front = false;
            break;

            case EventKeyboard::KeyCode::KEY_S:
            case EventKeyboard::KeyCode::KEY_CAPITAL_S:
            CCLOG("key S up");
            _back = false;
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
            _down = false;
            break;

            case EventKeyboard::KeyCode::KEY_E:
            case EventKeyboard::KeyCode::KEY_CAPITAL_E:
            CCLOG("key E up");
            _up = false;
            break;

            case EventKeyboard::KeyCode::KEY_V:
            case EventKeyboard::KeyCode::KEY_CAPITAL_V:
                CCLOG("key v up");
                _mouseDeleting = false;
                break;

            case EventKeyboard::KeyCode::KEY_B:
            case EventKeyboard::KeyCode::KEY_CAPITAL_B:
                CCLOG("key b up");
                _mourseMetaCubeSelect = false;
                break;

            case EventKeyboard::KeyCode::KEY_UP_ARROW:
                CCLOG("key arrow up");
                _rotateUp = false;
                break;

            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                CCLOG("key arrow down");
                _rotateDown = false;
                break;

            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                CCLOG("key arrow left");
                _rotateLeft = false;
                break;

            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                CCLOG("key arrow right");
                _rotateRight = false;
                break;

            default:
            break;
        }
        
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

    // mouse
    auto _mouseListener = EventListenerMouse::create();

    _mouseListener->onMouseMove = [&](Event *event){
        EventMouse* e = (EventMouse*)event;
        if (_isFPS){
            auto now = e->getLocation();
            Vec2 diff = now - _fpsAnchor;
            _fpsAnchor = now;
            _rotateY -= ROTATE_SCALE*diff.x;
            _rotateX -= ROTATE_SCALE*diff.y;
            if (_rotateX > UP_DOWN_MAX*0.5f*PI) _rotateX = UP_DOWN_MAX*0.5f*PI;
            if (_rotateX < -UP_DOWN_MAX*0.5f*PI) _rotateX = -UP_DOWN_MAX*0.5f*PI;
        } else {
            // 在右键松开时（不再通过鼠标改变镜头方向时），显示鼠标选择（与准星选择的表现）。
            showMouseSelection(e->getLocationInView());
        }
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

        auto size = Director::getInstance()->getVisibleSize();
        switch(e->getMouseButton()){
            case MOUSE_BUTTON_LEFT:
                if (size.height + e->getLocationInView().y > 0.1f * size.height &&
                    size.height + e->getLocationInView().y < 0.9f * size.height) {
                    if (_mouseDeleting ) {
                        deleteAnCubeByMouseCursor(e->getLocationInView());
                    }else if (_mourseMetaCubeSelect) {
                        selectMetaCubeByCursor(e->getLocationInView());
                    }
                    else {
                        addAndCubeByMouseCursor(e->getLocationInView());
                    }
                }
                break;

            case MOUSE_BUTTON_RIGHT:
            {
                _isFPS = true;
                _fpsAnchor = e->getLocation();
                auto ro = _sceneCamera->getRotation3D();
                break;
            }
            default:
            break;
        }
    };
    _mouseListener->onMouseScroll = [&](Event *event){
        EventMouse* e = (EventMouse*)event;
        float diff = e->getScrollY();
        Vec3 dir = _sceneCamera->getRotationQuat() * Vec3{0.f, 0.f, -1.f};
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + SCALL_MOVE_SCALE * -diff * dir);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);

}

cocos2d::Vec3 BuildingScene::rawPos2Real(cocos2d::Vec3 raw)
{
    return 10.f*raw;
}

void BuildingScene::addCube(CubeSprite* cube)
{
    assert(cube);
    _cubes.push_back(cube);
    cube->setPosition3D(rawPos2Real(cube->getRawPos()));
    cube->setCameraMask(_sceneLayer->getCameraMask());
    _sceneLayer->addChild(cube);

    PrimitiveOrder order;
    Vec3 rawPos = cube->getRawPos();
    order.funcUndo = [this, rawPos](){
        this->removeCubeForUndo(rawPos);
    };
    execOrder(order);
}

void BuildingScene::addCubeForUndo(const cocos2d::Vec3& rawPos, const int& metaCubeId)
{
    CubeSprite* cs = CubeSprite::create(rawPos, metaCubeId);
    _cubes.push_back(cs);
    cs->setPosition3D(rawPos2Real(cs->getRawPos()));
    cs->setCameraMask(_sceneLayer->getCameraMask());
    _sceneLayer->addChild(cs);
}

void BuildingScene::removeCube(CubeSprite* cube)
{
    assert(cube);
    for (auto iter = _cubes.begin(); iter != _cubes.end(); ) {
        if (*iter == cube) {
            _cubes.erase(iter);
        } else {
            iter++;
        }
    }
    _sceneLayer->removeChild(cube);

    PrimitiveOrder order;
    Vec3 rawPos = cube->getRawPos();
    int id = cube->getMetaCubeId();
    order.funcUndo = [this, rawPos, id](){
        this->addCubeForUndo(rawPos, id);
    };
    execOrder(order);
}

void BuildingScene::removeCubeForUndo(const cocos2d::Vec3& rawPos)
{
    for (auto iter = _cubes.begin(); iter != _cubes.end(); ) {
        if ((*iter)->getRawPos() == rawPos ) {
            _sceneLayer->removeChild(*iter);
            iter = _cubes.erase(iter);
        } else {
            iter++;
        }
    }
}

void BuildingScene::addUnrealWall(UnrealType t, int width)
{
    assert(width >= 0);
    for (int i = 0-width/2; i < width/2; i++) {
        for (int j = 0-width/2; j < width/2; j++) {
            Vec3 rawPos = {0,0,0};
            switch (t) {
                case UX:
                rawPos = Vec3{0, 1.0f*i, 1.0f*j};
                break;

                case UY:
                rawPos = Vec3{1.f*i, 0 ,1.f*j};
                break;

                case UZ:
                rawPos = Vec3{1.f*i, 1.f*j, 0};
                break;
            }
            addCube(CubeSprite::create(rawPos, _brushLayer->getSelectedCubeId()));
        }
    }
//    addCube(CubeSprite::create({0,0,0}, 0));
}

void BuildingScene::addCenterAnchor()
{
    // check
    for (auto iter = _cubes.begin(); iter != _cubes.end(); iter++) {
        if ( (*iter)->getRawPos() == Vec3{0,0,0} ) {
            return;
        }
    }

    addCube(CubeSprite::create({0,0,0}, _brushLayer->getSelectedCubeId()));
}

void BuildingScene::update(float dt)
{
    Quaternion qua2 = {Vec3(1.f,0.f,0.f), _rotateX};
    Quaternion qua = {Vec3(0.f, 1.f, 0.f), _rotateY};
    qua.multiply(qua2);
    _sceneCamera->setRotationQuat(qua);

    qua.normalize();
    if (_up && !_down) {
        Vec3 dir = Vec3{0.f,1.f,0.f};
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_down && !_up) {
        Vec3 dir = Vec3{0.f,-1.f,0.f};
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_left && !_right) {
        Vec3 dir = qua * Vec3{-1.f,0.f,0.f};
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_right && !_left) {
        Vec3 dir = qua * Vec3{1.f,0.f,0.f};
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_zoomIn && !_zoomOut) {
        Vec3 dir = qua * Vec3{0.f,0.f,-1.f};
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_zoomOut && !_zoomIn) {
        Vec3 dir = qua * Vec3{0.f,0.f,1.f};
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_front && !_back) {
        Vec3 dir = qua * Vec3{0.f,0.f,-1.f};
        dir.y = 0.f;
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_back && !_front) {
        Vec3 dir = qua * Vec3{0.f,0.f,1.f};
        dir.y = 0.f;
        dir.normalize();
        _sceneCamera->setPosition3D(_sceneCamera->getPosition3D() + MOVE_SCALE * dir);
    }
    if (_rotateUp || _rotateDown) {
        if (_rotateUp && _rotateDown) {
            // skip
        } else if (_rotateUp){
            _rotateX += ROTATE_SCALE*ROTATE_KEY_STEP;
        } else {
            _rotateX -= ROTATE_SCALE*ROTATE_KEY_STEP;
        }
        if (_rotateX > UP_DOWN_MAX*0.5f*PI) _rotateX = UP_DOWN_MAX*0.5f*PI;
        if (_rotateX < -UP_DOWN_MAX*0.5f*PI) _rotateX = -UP_DOWN_MAX*0.5f*PI;
    }
    if (_rotateLeft || _rotateRight) {
        if (_rotateLeft && _rotateRight) {
            // skip
        } else if (_rotateLeft){
            _rotateY += ROTATE_SCALE*ROTATE_KEY_STEP;
        } else {
            _rotateY -= ROTATE_SCALE*ROTATE_KEY_STEP;
        }
    }
    // 基于准星的加入与删除，通过_centerSelect来开启或关闭。
    if (_centerSelect) updateCenterSelection();
}

inline bool intersectionQuad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Vec3& origin, const Vec3& dir, float* distance){
    float da = 0;
    float db = 0;

    if (triangle_intersection(a,c,b,origin,dir,&da) || triangle_intersection(a,c,d,origin,dir,&db)) {
        if (distance) {
            *distance = std::max(da, db);
        }
        return true;
    }
    return false;
}

CubeSprite* BuildingScene::getIntersection(const Vec3& origin2, const Vec3& dir, int* face)
{
//    Vec3 dir = _sceneCamera->getRotationQuat() * Vec3{0.f,0.f,-1.f};
//    Vec3 origin = _sceneCamera->getPosition3D() * (1.f / EditState::CUBE_WIDTH); // 这样就不用转换rawPos了。
    Vec3 origin = origin2 * (1.f/EditState::CUBE_WIDTH);
    float distMin = std::numeric_limits<float>::max();
    CubeSprite* retCube = nullptr;
    int retFace = 0;

    float tmpdis = 0;
    for (auto iter = _cubes.begin(); iter != _cubes.end(); iter++) {
        Vec3 rawPos = (*iter)->getRawPos();

        Vec3 a = rawPos + Vec3{0.5f,0.5f,0.5f};
        Vec3 b = rawPos + Vec3{0.5f, 0.5f,-0.5f};
        Vec3 c =rawPos + Vec3{-0.5f, 0.5f, -0.5f};
        Vec3 d = rawPos + Vec3{-0.5f,0.5f,0.5f};
        Vec3 e = rawPos + Vec3{0.5f,-0.5f,0.5f};
        Vec3 f = rawPos + Vec3{0.5f, -0.5f,-0.5f};
        Vec3 g =rawPos + Vec3{-0.5f, -0.5f, -0.5f};
        Vec3 h = rawPos + Vec3{-0.5f,-0.5f,0.5f};

        if (intersectionQuad(a,b,c,d,origin,dir,&tmpdis)) {
            if (tmpdis < distMin) {
                distMin = tmpdis;
                retCube = *iter;
                retFace = FYP;
            }
        }
        if (intersectionQuad(e,f,g,h,origin,dir,&tmpdis)) {
            if (tmpdis < distMin) {
                distMin = tmpdis;
                retCube = *iter;
                retFace = FYN;
            }
        }
        if (intersectionQuad(a,d,h,e,origin,dir,&tmpdis)) {
            if (tmpdis < distMin) {
                distMin = tmpdis;
                retCube = *iter;
                retFace = FZP;
            }
        }
        if (intersectionQuad(b,c,g,f,origin,dir,&tmpdis)) {
            if (tmpdis < distMin) {
                distMin = tmpdis;
                retCube = *iter;
                retFace = FZN;
            }
        }
        if (intersectionQuad(a,e,f,b,origin,dir,&tmpdis)) {
            if (tmpdis < distMin) {
                distMin = tmpdis;
                retCube = *iter;
                retFace = FXP;
            }
        }
        if (intersectionQuad(c,d,h,g,origin,dir,&tmpdis)) {
            if (tmpdis < distMin) {
                distMin = tmpdis;
                retCube = *iter;
                retFace = FXN;
            }
        }
    }

    if (face) *face = retFace;

    return retCube;
}

void BuildingScene::updateCenterSelection()
{
    Vec3 dir = _sceneCamera->getRotationQuat() * Vec3{0.f,0.f,-1.f};

    CubeSprite* cp = getIntersection(_sceneCamera->getPosition3D(), dir, &_lastSelectedFace);
    if (cp ==_lastSelected) return;
    if (_lastSelected) _lastSelected->unselect();
    _lastSelected = nullptr;
    if (cp) {
        cp->select();
        _lastSelected = cp;
        auto r = cp->getRawPos();
//        CCLOG("Intersection = {%f %f %f} %d", r.x, r.y, r.z, face);
    }
}

void BuildingScene::addCubeBySelectInter(const cocos2d::Vec3& alignRawPos, int face)
{
    Vec3 toadd = face == FXP ? Vec3{1.f,0.f,0.f} :
    face == FXN ? Vec3{-1.f,0.f,0.f} :
    face == FYP ? Vec3{0.f,1.f,0.f} :
    face == FYN ? Vec3{0.f,-1.f,0.f} :
    face == FZP ? Vec3{0.f,0.f,1.f} : Vec3{0.f,0.f,-1.f};
    auto newCube = CubeSprite::create(alignRawPos + toadd, _brushLayer->getSelectedCubeId());
    addCube(newCube);
}
void BuildingScene::addAnCubeAlignSelectedFace()
{
    CCLOG("add cube align selected face");
    if (_lastSelected == nullptr) return;

    Vec3 pos = _lastSelected->getRawPos();
    addCubeBySelectInter(pos, _lastSelectedFace);
}

void BuildingScene::deleteTheSelectedCube()
{
    CCLOG("delete the selected cube");
    if (_lastSelected == nullptr) return;
    _lastSelected->unselect();
    removeCube(_lastSelected);
    _lastSelected = nullptr;
}

void BuildingScene::deleteTheMouseSelectedCube()
{
    CCLOG("delete the mouse selected cube");
    if (_lastMouseSelected == nullptr) return;
    _lastMouseSelected->unselect();
    removeCube(_lastMouseSelected);
    _lastMouseSelected = nullptr;
}

void BuildingScene::selectMetaCubeByCursor(const cocos2d::Vec2& cursor)
{
    CCLOG("select metacube by cursor");
    int face;
    auto cp = getMouseSelection(cursor, &face);
    if (cp) {
        _brushLayer->setSelectedCubeId(cp->getMetaCubeId());
    }
}

void BuildingScene::addAndCubeByMouseCursor(const cocos2d::Vec2& cursor)
{
    CCLOG("add cube by mouse cursor");
    int face;
    CubeSprite* cp = getMouseSelection(cursor, &face);
    if (cp) {
        addCubeBySelectInter(cp->getRawPos(), face);
    }
}

void BuildingScene::deleteAnCubeByMouseCursor(const cocos2d::Vec2& cursor)
{
    CCLOG("delete cube by mouse cursor");
    int face;
    CubeSprite* cp = getMouseSelection(cursor, &face);
    if (cp) {
        removeCube(cp);
    }
}


void BuildingScene::showMouseSelection(const cocos2d::Vec2& cursor)
{
    CubeSprite* cp = getMouseSelection(cursor, nullptr);
    if (_lastMouseSelected == cp) return;
    if (_lastMouseSelected && _lastSelected != _lastMouseSelected) _lastMouseSelected->unselect();
    if (cp) cp->select();
    _lastMouseSelected = cp;
}

CubeSprite* BuildingScene::getMouseSelection(const cocos2d::Vec2& cursor, int* face)
{
    Vec3 dest = _sceneCamera->unproject(Vec3{cursor.x, -cursor.y, 100});
    Vec3 src = _sceneCamera->getPosition3D();
    Vec3 dir = src - dest;
    dir.normalize();
    CubeSprite* cp = getIntersection(src, dir, face);
    return cp;
}


void BuildingScene::execOrder(PrimitiveOrder& order)
{
//    order.exec();
    _orderQueue.push_back(order);
    if (_orderQueue.size() >= MAX_SAVE_ORDER) {
        _orderQueue.pop_front();
    }
}

void BuildingScene::undoOrder()
{
    if (_orderQueue.empty()) return;
    auto order = _orderQueue.back();
    order.undo();
    _orderQueue.pop_back();
}