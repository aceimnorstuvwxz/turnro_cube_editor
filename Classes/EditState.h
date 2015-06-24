// 永恒湍流打败一切

#ifndef __cube3d__EditState__
#define __cube3d__EditState__

// 暗黑编辑器核心状态搜集器
#include "cocos2d.h"
#include <string>

class EditState
{
public:
    EditState* s(){ return &_instance; };

    std::string getBasePath();
    std::string getCubeMetaFilePath();

private:
    EditState(){};
    static EditState _instance;
};

#endif /* defined(__cube3d__EditState__) */
