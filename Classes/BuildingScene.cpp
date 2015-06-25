// 湍流游戏

#include "BuildingScene.h"
#include "uiconf.h"
#include "EditState.h"
#include "format.h"
#include "Msg.h"
#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/filereadstream.h"

#include <cstdio>
#include <vector>

#include <sys/stat.h> //mkdir

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

    reloadMetaCubes();

    return true;
}

void BuildingScene::reloadMetaCubes()
{
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

    //
}

void BuildingScene::copyTemplateWorkspace()
{
    auto fileListString = FileUtils::getInstance()->getStringFromFile("MicroCube/filelist.json");
    CCLOG(fileListString.c_str());
    rjson::Document doc;
    doc.Parse(fileListString.c_str());
    std::vector<std::string> dirs;
    std::vector<std::string> files;
    auto& dirlist = doc["directories"];
    for (auto iter = dirlist.Begin(); iter != dirlist.End(); iter++) {
        std::string str = iter->GetString();
        CCLOG(str.c_str());
        dirs.push_back(str);
    }
    auto& filelist = doc["files"];
    for (auto iter = filelist.Begin(); iter != filelist.End(); iter++) {
        std::string str = iter->GetString();
        CCLOG(str.c_str());
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
        CCLOG(dst.c_str());
        FILE* fp = fopen((EditState::s()->getBasePath()+files[i]).c_str(), "w");
        assert(fp != NULL);
        fwrite(data.getBytes(), sizeof(unsigned char), data.getSize(), fp);
        fclose(fp);
    }
}

