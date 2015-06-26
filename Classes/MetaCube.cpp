//
//  MetaCube.cpp
//  cube3d
//
//  Created by chenbingfeng on 15/6/26.
//
//

#include "MetaCube.h"
#include "EditState.h"
USING_NS_CC;

void MetaCube::reload()
{
    //clear
    if (_programState) {
        _programState->release();
        _programState = nullptr;
    }
    
    if (_texture) {
        _texture->release();
        _texture = nullptr;
    }

    // ProgramState
    auto _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
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

    // Mesh
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> colors;
    std::vector<float> texs;

    float vertices[24*8] = {
        0.500000,  0.500000,  0.500000,  0.000000,  0.000000,  1.000000,  0.625000,  0.250000,
        -0.500000,  0.500000,  0.500000,  0.000000,  0.000000,  1.000000,  0.375000,  0.250000,
        -0.500000, -0.500000,  0.500000,  0.000000,  0.000000,  1.000000,  0.375000,  0.000000,
        0.500000, -0.500000,  0.500000,  0.000000,  0.000000,  1.000000,  0.625000,  0.000000,
        0.500000,  0.500000,  0.500000,  0.000000,  1.000000,  0.000000,  0.625000,  0.250000,
        0.500000,  0.500000, -0.500000,  0.000000,  1.000000,  0.000000,  0.625000,  0.500000,
        -0.500000,  0.500000,  0.500000,  0.000000,  1.000000,  0.000000,  0.375000,  0.250000,
        -0.500000,  0.500000, -0.500000,  0.000000,  1.000000,  0.000000,  0.375000,  0.500000,
        0.500000, -0.500000, -0.500000,  0.000000,  0.000000, -1.000000,  0.625000,  0.750000,
        -0.500000, -0.500000, -0.500000,  0.000000,  0.000000, -1.000000,  0.375000,  0.750000,
        -0.500000,  0.500000, -0.500000,  0.000000,  0.000000, -1.000000,  0.375000,  0.500000,
        0.500000,  0.500000, -0.500000,  0.000000,  0.000000, -1.000000,  0.625000,  0.500000,
        0.500000, -0.500000,  0.500000,  0.000000, -1.000000,  0.000000,  0.625000,  1.000000,
        -0.500000, -0.500000,  0.500000,  0.000000, -1.000000,  0.000000,  0.375000,  1.000000,
        -0.500000, -0.500000, -0.500000,  0.000000, -1.000000,  0.000000,  0.375000,  0.750000,
        0.500000, -0.500000, -0.500000,  0.000000, -1.000000,  0.000000,  0.625000,  0.750000,
        0.500000,  0.500000,  0.500000,  1.000000,  0.000000,  0.000000,  0.625000,  0.250000,
        0.500000, -0.500000,  0.500000,  1.000000,  0.000000,  0.000000,  0.625000,  0.000000,
        0.500000,  0.500000, -0.500000,  1.000000,  0.000000,  0.000000,  0.875000,  0.250000,
        0.500000, -0.500000, -0.500000,  1.000000,  0.000000,  0.000000,  0.875000,  0.000000,
        -0.500000,  0.500000,  0.500000, -1.000000,  0.000000,  0.000000,  0.375000,  0.250000,
        -0.500000, -0.500000, -0.500000, -1.000000,  0.000000,  0.000000,  0.125000,  0.000000,
        -0.500000, -0.500000,  0.500000, -1.000000,  0.000000,  0.000000,  0.375000,  0.000000,
        -0.500000,  0.500000, -0.500000, -1.000000,  0.000000,  0.000000,  0.125000,  0.250000
    };

    // 24 vertex
    float* vp = vertices;
    for (int i = 0; i < 24; i++, vp+=8) {
        positions.push_back(vp[0]*_cubeLength);
        positions.push_back(vp[1]*_cubeLength);
        positions.push_back(vp[2]*_cubeLength);
        normals.push_back(vp[3]);
        normals.push_back(vp[4]);
        normals.push_back(vp[5]);
        texs.push_back(vp[6]);
        texs.push_back(vp[7]);
    }

    // 12 triangles
    std::vector<unsigned short> triangleIndex = {
        0,   1,   2,   0,   2,   3,   4,   5,   6,   6,   5,   7,
        8,   9,  10,   8,  10,  11,  12,  13,  14,  12,  14,  15,
        16,  17,  18,  17,  19,  18,  20,  21,  22,  21,  20,  23
    };

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

    // Texture
    if (this->texture.size() > 0) {
        _texture = Director::getInstance()->getTextureCache()->addImage(EditState::s()->getBasePath() + this->texture);
        _texture->retain(); // if error here, check if the Path to image is right,and if the image is exist.
    }
}

cocos2d::GLProgramState* MetaCube::getProgramState()
{
    return _programState;
}

cocos2d::Mesh* MetaCube::getRenderMesh()
{
    return _renderMesh;
}

Texture2D* MetaCube::getTexture()
{
    return _texture;
}