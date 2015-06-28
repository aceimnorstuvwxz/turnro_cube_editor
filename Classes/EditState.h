// 永恒湍流打败一切

#ifndef __cube3d__EditState__
#define __cube3d__EditState__

// 暗黑编辑器核心状态搜集器
#include "cocos2d.h"
#include <string>
#include <map>
#include "MetaCube.h"

class EditState
{
public:
    static const float CUBE_WIDTH;
    static EditState* s(){ return &_instance; };

    std::string getBasePath() const;
    std::string getCubeMetaFilePath() const;

    void setUnitName(const std::string& unitName);
    std::string getExtraUnitFilePath() const;
    std::string getUnitFilePath() const;

    void setDefaultVertexShader(const std::string& vsh);
    void setDefaultFragmentShader(const std::string& fsh);
    std::map<int, MetaCube>* getMetaCubeMap();
    MetaCube* getMetaCube(int id);
private:
    EditState(){};
    static EditState _instance;

    std::string _unitName;

    std::string _defaultVertexShader;
    std::string _defaultFragmentShader;
    // map,使id遍历有序。
    std::map<int, MetaCube> _metaCubeMap;
};

#endif /* defined(__cube3d__EditState__) */
