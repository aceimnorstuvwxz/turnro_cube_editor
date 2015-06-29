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
    int getMetaCubeId(){return _metaCubeId; }
    void select() { _selected = true; }
    void unselect() { _selected = false; }

protected:
    //data
    cocos2d::Vec3 _posRaw = {0,0,0};
    int _metaCubeId = 0;
    cocos2d::Vec4 _color;
    cocos2d::Mesh* _renderMesh = nullptr;
    cocos2d::MeshCommand* _meshCommand = nullptr;
    cocos2d::PrimitiveCommand* _edgePrimitiveCommand = nullptr;
    cocos2d::CustomCommand* _edgeCustomCommand = nullptr;
    cocos2d::BlendFunc _blendFunc;
    cocos2d::Texture2D* _texture;
    cocos2d::GLProgramState* _edgeGlProgramState = nullptr;
    cocos2d::Primitive* _edgePrimitive = nullptr;


    bool _selected = false;

    CubeSprite(){};

//    virtual void update(float dt) override;
};

#endif /* defined(__cube3d__CubeSprite__) */
