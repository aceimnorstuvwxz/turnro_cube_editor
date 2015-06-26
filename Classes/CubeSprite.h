// 湍流国际

#ifndef __cube3d__CubeSprite__
#define __cube3d__CubeSprite__

#include "cocos2d.h"

class CubeSprite: public cocos2d::Node
{
public:
    static CubeSprite* create(const cocos2d::Vec3& pos_raw, int metaCubeId);

    virtual bool init(const cocos2d::Vec3& pos_raw, int metaCubeId);

    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;

    cocos2d::Vec3 getRawPos(){ return _posRaw; }

protected:
    //data
    float _cubeLength;
    cocos2d::Vec3 _posRaw;
    int _metaCubeId;
    cocos2d::Vec4 _color;
    cocos2d::Mesh* _renderMesh;
    cocos2d::GLProgramState* _programState;
    cocos2d::MeshCommand* _meshCommand;
    cocos2d::BlendFunc _blendFunc;
    
    //data init, actually not necessary.
    CubeSprite():
    _cubeLength(10),
    _posRaw(0,0,0),
    _renderMesh(nullptr),
    _programState(nullptr),
    _meshCommand(nullptr)
    {};

//    virtual void update(float dt) override;
};

#endif /* defined(__cube3d__CubeSprite__) */
