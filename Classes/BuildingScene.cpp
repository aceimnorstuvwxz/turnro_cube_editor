// 湍流游戏


#include <cstdio>
#include <vector>

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/filereadstream.h"

#include <sys/stat.h> //mkdir

#include "BuildingScene.h"
#include "uiconf.h"
#include "EditState.h"
#include "format.h"
#include "Msg.h"
#include "CubeSprite.h"
#include "EditState.h"


USING_NS_CC;

class StateInfoLayer: public McdLayer
{
public:
    virtual bool init() override {
        assert(McdLayer::init());

        _lbUnitFile = Label::createWithTTF(EditState::s()->getExtraUnitFilePath(), uic::font_zh, 14);
        _lbUnitFile->setPosition(genPos({0.5f, 0.95f}));
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

    _brushLayer = BrushLayer::create();
    this->addChild(_brushLayer);

    reloadMetaCubes();

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

