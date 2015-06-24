//
//  TRBaseScene.h
//  mcdrunning
//
//  Created by chenbingfeng on 15/6/22.
//
// 定制的Scene基类

#ifndef __mcdrunning__TRBaseScene__
#define __mcdrunning__TRBaseScene__

#include <cocos2d.h>
#include "McdLayer.h"
#include <unordered_map>

class McdLayer;
class TRBaseScene:public cocos2d::Scene
{
public:
    typedef std::function<void(void)> COMMON_CALLBACK;

    virtual bool init() override;
    CREATE_FUNC(TRBaseScene);


    void addCommonBg();
    void addCommonBackBtn(COMMON_CALLBACK callback);
    void addCommonLabel(const cocos2d::Vec2& pos, const std::string& text, int size=25);
    cocos2d::ui::Button* addCommonBtn(const cocos2d::Vec2& pos, const std::string& text, COMMON_CALLBACK callback);
    cocos2d::Vec2 genPos(const cocos2d::Vec2& pos);
    cocos2d::Vec3 genPos3D(const cocos2d::Vec2& pos, float z);

protected:
    McdLayer* _defaultLayer;
    COMMON_CALLBACK _backCallBack;
    int _btnAI = 0;
    std::unordered_map<int, COMMON_CALLBACK> _cbMap;
};

#endif /* defined(__mcdrunning__TRBaseScene__) */
