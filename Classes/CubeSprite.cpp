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
    // create shader program
    // TODO 将shader共享。。。
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/cube.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/cube.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_position", GLProgram::VERTEX_ATTRIB_POSITION);
    glprogram->bindAttribLocation("a_color", GLProgram::VERTEX_ATTRIB_COLOR);
    glprogram->bindAttribLocation("a_normal", GLProgram::VERTEX_ATTRIB_NORMAL);
    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
    this->setGLProgramState(_programState);

    // create the mesh
    // TODO make it shared...

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> colors;
    std::vector<float> texs;

    _color = EditState::s()->getMetaCube(metaCubeId)->color;
    // 8 vertex
    float half = _cubeLength * 0.5f;
    auto tfunc = [&positions, &texs, &half, &colors, &normals](int x, int y, int z){
        positions.push_back(x == -1 ? -half :
                            x == 0 ? 0 : half);
        positions.push_back(y == -1 ? -half :
                            y == 0 ? 0 : half);
        positions.push_back(z == -1 ? -half :
                            z == 0 ? 0 : half);

        if (x * y * z == 0) {
            texs.push_back(0.f);
            texs.push_back(0.f);
        } else {
            texs.push_back(1.f);
            texs.push_back(1.f);
        }

    };
    tfunc(1,1,1);
    tfunc(-1,1,1);
    tfunc(-1,-1,1);
    tfunc(1,-1,1);
    tfunc(1,1,-1);
    tfunc(-1,1,-1);
    tfunc(-1,-1,-1);
    tfunc(1,-1,-1);
    tfunc(0,0,1);
    tfunc(1,0,0);
    tfunc(0,0,-1);
    tfunc(-1,0,0);
    tfunc(0,1,0);
    tfunc(0,-1,0);

    // 12 triangles
    std::vector<unsigned short> triangleIndex;
    auto tgf = [&triangleIndex](unsigned short a, unsigned short b, unsigned short c){
        triangleIndex.push_back(a);
        triangleIndex.push_back(b);
        triangleIndex.push_back(c);
    };
    tgf(8,0,1);
    tgf(8,1,2);
    tgf(8,2,3);
    tgf(8,3,0);
    tgf(9,0,3);
    tgf(9,3,7);
    tgf(9,7,4);
    tgf(9,4,0);
    tgf(10,5,4);
    tgf(10,4,7);
    tgf(10,7,6);
    tgf(10,6,5);
    tgf(11,1,5);
    tgf(11,5,6);
    tgf(11,6,2);
    tgf(11,2,1);
    tgf(12,4,5);
    tgf(12,5,1);
    tgf(12,1,0);
    tgf(12,0,4);
    tgf(12,4,5);
    tgf(13,3,2);
    tgf(13,2,6);
    tgf(13,6,7);
    tgf(13,7,3);

    _renderMesh = Mesh::create(positions, normals, texs, triangleIndex);
    _renderMesh->retain();

    long offset = 0;
    auto attributeCount = _renderMesh->getMeshVertexAttribCount();
    CCLOG("attributeCount = %ld", attributeCount);
    for (auto k = 0; k < attributeCount; k++) {
        auto meshattribute = _renderMesh->getMeshVertexAttribute(k);
        _programState->setVertexAttribPointer(s_attributeNames[meshattribute.vertexAttrib],
                                               meshattribute.size,
                                               meshattribute.type,
                                               GL_FALSE,
                                               _renderMesh->getVertexSizeInBytes(),
                                               (GLvoid*)offset);
        offset += meshattribute.attribSizeBytes;
    }

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