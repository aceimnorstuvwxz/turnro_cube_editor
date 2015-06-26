// 永恒湍流打败一切

#ifndef __cube3d__EditState__
#define __cube3d__EditState__

// 暗黑编辑器核心状态搜集器
#include "cocos2d.h"
#include <string>

class MetaCube
{
public:
    int id;
    std::string name;
    std::string comment;
    cocos2d::Vec4 color;
    std::string texture;
    std::string shader[2];
    bool unreal;
    MetaCube():id(0),unreal(false){}
    MetaCube(int id, const std::string& name, const std::string& comment, const cocos2d::Vec4& color, const std::string& texture, const std::string& vsh, const std::string& fsh, bool unreal):id(id), name(name), comment(comment), color(color), texture(texture), unreal(unreal){
        this->shader[0] = vsh;
        this->shader[1] = fsh;
    }
};

class EditState
{
public:
    static EditState* s(){ return &_instance; };

    std::string getBasePath() const;
    std::string getCubeMetaFilePath() const;

    void setUnitName(const std::string& unitName);
    std::string getExtraUnitFilePath() const;
    std::string getUnitFilePath() const;

    void setDefaultVertexShader(const std::string& vsh);
    void setDefaultFragmentShader(const std::string& fsh);
    std::unordered_map<int, MetaCube>* getMetaCubeMap();
    MetaCube* getMetaCube(int id);
private:
    EditState(){};
    static EditState _instance;

    std::string _unitName;

    std::string _defaultVertexShader;
    std::string _defaultFragmentShader;
    std::unordered_map<int, MetaCube> _metaCubeMap;
};

#endif /* defined(__cube3d__EditState__) */
