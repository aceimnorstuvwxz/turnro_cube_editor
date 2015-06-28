//
//  intersection.h
//  cube3d
//
//  Created by chenbingfeng on 15/6/28.
//
//

#ifndef __cube3d__intersection__
#define __cube3d__intersection__

#include "cocos2d.h"

bool triangle_intersection( const cocos2d::Vec3   V1,  // Triangle vertices
                          const cocos2d::Vec3   V2,
                          const cocos2d::Vec3   V3,
                          const cocos2d::Vec3    O,  //Ray origin
                          const cocos2d::Vec3    D,  //Ray direction
                          float* out );

#endif /* defined(__cube3d__intersection__) */
