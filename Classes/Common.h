//
//  Common.h
//  cube3d
//
//  Created by chenbingfeng on 15/6/9.
//
//

#ifndef __cube3d__Common__
#define __cube3d__Common__

#include <string>

namespace config {
    const std::string window_name = "Cube3d: cube scene editor";
    const int window_w = 1000;
    const int window_h = 800;
}

inline cocos2d::Vec2 operator*(const cocos2d::Vec2& v2, const cocos2d::Size& size)
{
    return {v2.x * size.width, v2.y * size.height};
}

inline cocos2d::Vec2 operator*(const cocos2d::Size& size, const cocos2d::Vec2& v2)
{
    return v2*size;
}
inline void scale2size(cocos2d::Sprite* sprite, const cocos2d::Size& size)
{
    sprite->setScaleX(size.width* 1.f/sprite->getContentSize().width);
    sprite->setScaleY(size.height*1.f/sprite->getContentSize().height);
}
#endif /* defined(__cube3d__Common__) */
