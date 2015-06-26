// 湍流国际

// 这个类的形式必须使用修改过的cocos2d。

#include "CubeSprite.h"
#include "EditState.h"
#include <vector>
USING_NS_CC;

CubeSprite* CubeSprite::create(const cocos2d::Vec3& pos_raw, int metaCubeId)
{
    auto sprite = new (std::nothrow) CubeSprite();

    if (sprite && sprite->init(pos_raw, metaCubeId)) {
        sprite->autorelease();
    } else {
        CC_SAFE_DELETE(sprite);
    }
    return sprite;
}

bool CubeSprite::init(const cocos2d::Vec3& pos_raw, int metaCubeId)
{
    if (!Node::init()) return false;

    _metaCubeId = metaCubeId;
    _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
    MetaCube* metaCube = EditState::s()->getMetaCube(metaCubeId);
    this->setGLProgramState(metaCube->getProgramState());
    _color = EditState::s()->getMetaCube(metaCubeId)->color;
    _renderMesh = metaCube->getRenderMesh();
    
    return true;
}

void CubeSprite::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    CC_SAFE_DELETE(_meshCommand);
    _meshCommand = new MeshCommand();
    _meshCommand->genMaterialID(0, getGLProgramState(), _renderMesh->getVertexBuffer(), _renderMesh->getIndexBuffer(), _blendFunc);
    _meshCommand->init(_globalZOrder, 0, getGLProgramState(), _blendFunc, _renderMesh->getVertexBuffer(), _renderMesh->getIndexBuffer(), (GLenum)_renderMesh->getPrimitiveType(), (GLenum)_renderMesh->getIndexFormat(), _renderMesh->getIndexCount(), transform, flags);
    _meshCommand->setCullFaceEnabled(true);
    _meshCommand->setDepthTestEnabled(true);
    _meshCommand->setTransparent(true);
//    Color4F color(getDisplayedColor());
//    color.a = getDisplayedOpacity() / 255.0f;
    _meshCommand->setDisplayColor(_color);
    renderer->addCommand(_meshCommand);
    Node::draw(renderer, transform, flags);
}