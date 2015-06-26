// 次世代湍流 宇宙无敌

#ifndef __cube3d__BuildingScene__
#define __cube3d__BuildingScene__

#include "cocos2d.h"
#include "TRBaseScene.h"
#include <vector>
#include <string>
#include <unordered_map>

class MetaCube
{
public:
    int id;
    std::string name;
    std::string comment;
    cocos2d::Color4B color;
    std::string texture;
    std::string shader[2];
    bool unreal;
    MetaCube():id(0),unreal(false){}
    MetaCube(int id, const std::string& name, const std::string& comment, const cocos2d::Color4B& color, const std::string& texture, const std::string& vsh, const std::string& fsh, bool unreal):id(id), name(name), comment(comment), color(color), texture(texture), unreal(unreal){
        this->shader[0] = vsh;
        this->shader[1] = fsh;
    }
};

class BuildingScene: public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(BuildingScene);

private:
    void reloadMetaCubes();

    std::string _defauterShader[2];
    std::unordered_map<int, MetaCube> _metaCubeMap;

    //metacubes select board
    cocos2d::Layer* _metaSelectLayer;
    //todo

    //inter
    void copyTemplateWorkspace();
};

#endif /* defined(__cube3d__BuildingScene__) */
