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
    cocos2d::Vec3 _posRaw;
    int _metaCubeId;
    cocos2d::Vec4 _color;
    cocos2d::Mesh* _renderMesh;
    cocos2d::MeshCommand* _meshCommand;
    cocos2d::BlendFunc _blendFunc;
    cocos2d::Texture2D* _texture;

    bool _selected = false;
    
    //data init, actually not necessary.
    CubeSprite():
    _posRaw(0,0,0),
    _renderMesh(nullptr),
    _meshCommand(nullptr)
    {};

//    virtual void update(float dt) override;
};

#endif /* defined(__cube3d__CubeSprite__) */
