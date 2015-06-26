//
//  MetaCube.h
//  cube3d
//
//  Created by chenbingfeng on 15/6/26.
//
//

#ifndef __cube3d__MetaCube__
#define __cube3d__MetaCube__

#include <stdio.h>
#include "cocos2d.h"
#include <string>

class MetaCube
{
public:
    int id;
    std::string name;
    std::string comment;
    cocos2d::Vec4 color;
    std::string texture;
    std::string shader[2];
    bool unreal;
    MetaCube():id(0),unreal(false){}
    MetaCube(int id, const std::string& name, const std::string& comment, const cocos2d::Vec4& color, const std::string& texture, const std::string& vsh, const std::string& fsh, bool unreal):id(id), name(name), comment(comment), color(color), texture(texture), unreal(unreal){
        this->shader[0] = vsh;
        this->shader[1] = fsh;
    }
    void reload();
    cocos2d::GLProgramState* getProgramState();
    cocos2d::Mesh* getRenderMesh();

private:
    cocos2d::GLProgramState* _programState = nullptr;
    cocos2d::Mesh* _renderMesh = nullptr;
};

#endif /* defined(__cube3d__MetaCube__) */
