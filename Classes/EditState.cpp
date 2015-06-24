// 永恒湍流·陈清扬

#include "EditState.h"
#include "Msg.h"

USING_NS_CC;
// 神圣不朽的单例实体，必须永远守护。
EditState EditState::_instance;

std::string EditState::getBasePath()
{
    return FileUtils::getInstance()->getWritablePath() + Msg::s()["workspace"] + "/";
}

std::string EditState::getCubeMetaFilePath()
{
    return getBasePath() + "cubemeta.json";
}