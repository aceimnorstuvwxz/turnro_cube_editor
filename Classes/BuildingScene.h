// 次世代湍流 宇宙无敌

#ifndef __cube3d__BuildingScene__
#define __cube3d__BuildingScene__

#include <vector>
#include <string>
#include <unordered_map>

#include "cocos2d.h"
#include "TRBaseScene.h"
#include "BrushLayer.h"


class BuildingScene: public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(BuildingScene);

private:
    void reloadMetaCubes();


    //brush layer
    BrushLayer* _brushLayer;

//    void initMetaCubesLayer();
    //todo

    //inter
    void copyTemplateWorkspace();
};

#endif /* defined(__cube3d__BuildingScene__) */
