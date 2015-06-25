// 永恒湍流打败一切

#ifndef __cube3d__EditState__
#define __cube3d__EditState__

// 暗黑编辑器核心状态搜集器
#include "cocos2d.h"
#include <string>

class EditState
{
public:
    static EditState* s(){ return &_instance; };

    std::string getBasePath();
    std::string getCubeMetaFilePath();

    void setUnitName(const std::string& unitName);
    std::string getExtraUnitFilePath();
    std::string getUnitFilePath();

private:
    EditState(){};
    static EditState _instance;

    std::string _unitName;
};

#endif /* defined(__cube3d__EditState__) */
