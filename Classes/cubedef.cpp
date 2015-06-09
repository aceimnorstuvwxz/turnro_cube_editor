//
//  cubedef.cpp
//  cube3d
//
//  Created by chenbingfeng on 15/6/10.
//
//

#include "cubedef.h"

void CubePack::loadTestCubes()
{
    cubeLength = 20.f;
    for (int i = -50; i < 50; i++) {
        for (int j = -50; j < 50; j++) {
            cubes.push_back(Cube{i,j,0, static_cast<uint8_t>(256),static_cast<uint8_t>(256),static_cast<uint8_t>(256),static_cast<uint8_t>(256)});
        }
    }
}

void CubePack::loadFromFile(const std::string &filepath)
{

}

void CubePack::loadFromString(const std::string &jsonstr)
{

}

CubeCenter CubeCenter::_instance;