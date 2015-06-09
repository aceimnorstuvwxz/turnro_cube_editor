//
//  McdLayer.h
//  mcdrunning
//
//  Created by chenbingfeng on 15/5/31.
//
//

#ifndef __mcdrunning__McdLayer__
#define __mcdrunning__McdLayer__

#include <vector>

#include "cocos2d.h"
#include "Common.h"
#include "ui/CocosGUI.h"

class McdLayer : public cocos2d::Layer
{
public:
    typedef struct _Btn_info{
        std::string img;
        std::string img_p;
        cocos2d::Vec2 pos;
        float scale;
        _Btn_info(const std::string& img0, const std::string& img1, const cocos2d::Vec2& pos, float scale){
            this->img = img0;
            this->img_p = img1;
            this->pos = pos;
            this->scale = scale;
        }
    } Btn_info;
protected:
    virtual cocos2d::ui::Button* decorateButton(const Btn_info& btnInfo , int tag);
    virtual cocos2d::Sprite* decorateImg(const std::string& img, const cocos2d::Vec2& pos);
    virtual void touchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
    virtual void onButtonClick(int tag) = 0;

    void withInOut(cocos2d::Node* node);
    void turnIn(float dt);
    void turnOut(float dt);
    void turnIn(){ turnIn(_default_turn_in_time); }
    void turnOut(){ turnOut(_default_turn_out_time); };

private:
    std::vector<cocos2d::Node*> _inoutNodes;
    const float _default_turn_in_time = 0.3f;
    const float _default_turn_out_time = 0.3f;
};

#endif /* defined(__mcdrunning__McdLayer__) */
