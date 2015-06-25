// 永恒湍流·陈清扬

#include "EditState.h"
#include "Msg.h"

USING_NS_CC;
// 神圣不朽的单例实体，必须永远守护。
EditState EditState::_instance;

std::string EditState::getBasePath()
{
    return FileUtils::getInstance()->getWritablePath() + "MicroCube"+ "/";
}

std::string EditState::getCubeMetaFilePath()
{
    return getBasePath() + "metacubes.json";
}

void EditState::setUnitName(const std::string& unitName)
{
    _unitName = unitName;
}

std::string EditState::getExtraUnitFilePath()
{
    return getBasePath() + _unitName + ".cubex";
}

std::string EditState::getUnitFilePath()
{
    return getBasePath() + _unitName + ".cube";
}