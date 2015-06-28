// 永恒湍流·陈清扬

#include "EditState.h"
#include "Msg.h"

USING_NS_CC;
// 神圣不朽的单例实体，必须永远守护。
EditState EditState::_instance;
// 永恒的个体宽度，不变。
const float EditState::CUBE_WIDTH = 10.f;

std::string EditState::getBasePath() const
{
    return FileUtils::getInstance()->getWritablePath() + "MicroCube"+ "/";
}

std::string EditState::getCubeMetaFilePath() const
{
    return getBasePath() + "metacubes.json";
}

void EditState::setUnitName(const std::string& unitName)
{
    _unitName = unitName;
}

std::string EditState::getExtraUnitFilePath() const
{
    return getBasePath() + _unitName + ".cubex";
}

std::string EditState::getUnitFilePath() const
{
    return getBasePath() + _unitName + ".cube";
}

void EditState::setDefaultVertexShader(const std::string& vsh)
{
    _defaultVertexShader = vsh;
}
void EditState::setDefaultFragmentShader(const std::string& fsh)
{
    _defaultFragmentShader = fsh;
}
std::map<int, MetaCube>* EditState::getMetaCubeMap()
{
    return &_metaCubeMap;
}
MetaCube* EditState::getMetaCube(int id)
{
    return &_metaCubeMap[id];
}