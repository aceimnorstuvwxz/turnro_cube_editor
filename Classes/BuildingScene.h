// 次世代湍流 宇宙无敌

#ifndef __cube3d__BuildingScene__
#define __cube3d__BuildingScene__

#include "cocos2d.h"
#include "TRBaseScene.h"

class BuildingScene: public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(BuildingScene);

private:
    void reloadMetaCubes();
};

#endif /* defined(__cube3d__BuildingScene__) */
