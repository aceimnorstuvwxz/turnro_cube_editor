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
        return sprite;
    }

    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CubeSprite::init(const cocos2d::Vec3& pos_raw, int metaCubeId)
{

    assert(Node::init());

    _metaCubeId = metaCubeId;
    _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
    MetaCube* metaCube = EditState::s()->getMetaCube(metaCubeId);
    // 这个共享机制，还可以根据普通2D的Sprite来修改。
    this->setGLProgramState(metaCube->getProgramState());
    _color = EditState::s()->getMetaCube(metaCubeId)->color;
    _renderMesh = metaCube->getRenderMesh();
    _texture = metaCube->getTexture();
    _posRaw = pos_raw;

    
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
    _meshCommand->setDepthWriteEnabled(true); // must!

    Vec4 color = _color;

    // 在选择时，统一反色。
    // 妄图通过uniform来控制inverse来表示选中，而实际上由于GLProgram的公用，且此处的设置并不在时间上只作用于本CUBE的绘制，所以无效。
    // getGLProgramState()->setUniformInt("u_inverse", _selected ? 1 : 0);
    // hacked。这里用color.a == 0来在fsh中判断是否是texture还是color，当texture时，咱们用color.r来传alpha值。
    if (_texture){
        getGLProgramState()->setUniformTexture("u_texture", _texture);
        // 用W位来告诉shader是否用texture.
        color.w = 0;
        color.x = getOpacity() * 1.f / 256.f;
        // 通过y位来向shader传递是否选中的消息。
        color.y = _selected ? 1.f : 0.f;
    } else {
        // 选中时反转颜色（只对存色的cube有效)
        if (_selected) color = Vec4{1.f - color.x, 1.f - color.y, 1.f - color.z, color.w};
        color.w = color.w * (getOpacity() * 1.f / 256.f);
    }
    _meshCommand->setDisplayColor(color);
    renderer->addCommand(_meshCommand);
    Node::draw(renderer, transform, flags);
}