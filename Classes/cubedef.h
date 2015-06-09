//
//  cubedef.h
//  cube3d
//
//  Created by chenbingfeng on 15/6/10.
//
//

#ifndef __cube3d__cubedef__
#define __cube3d__cubedef__

#include <vector>
#include "cocos2d.h"

class Cube
{
public:
    const int x,y,z;
    const cocos2d::Color4B color;
    Cube(int x, int y, int z, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha):color{r,g,b,alpha},x(x),y(y),z(z){};
    Cube(const Cube& right):color{right.color},x(right.x),y(right.y),z(right.z){};
    cocos2d::Sprite3D* sprite;
};

class CubePack
{
public:
    float cubeLength;
    //TODO add color blend
    std::vector<Cube> cubes;// change to hash with position
    void loadFromFile(const std::string& filepath);
    void loadFromString(const std::string& jsonstr);
    void loadTestCubes();
};

class CubeCenter
{
public:
    static CubeCenter* get(){
        return &_instance;
    }
    CubePack cubePack;
    void initWithTest(){
        cubePack.loadTestCubes();
    }

private:
    CubeCenter(){};
    static CubeCenter _instance;
};

#endif /* defined(__cube3d__cubedef__) */
