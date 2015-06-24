// TurnroGame

#ifndef __simwar__Msg__
#define __simwar__Msg__

#include <string>
#include <fstream>
#include <iostream>
#include "cocos2d.h"

#ifdef __APPLE__
#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#endif

#ifdef ANDROID
#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#endif

#ifdef _WIN32
#include "rapidjson\rapidjson.h"	
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#endif

class Msg
{
public:
    static Msg& s(){
        return _instance;
    }

    std::string operator[](const std::string& key){
        if (!_loaded) {
            load();
        }
        return _doc[key.c_str()].GetString();
    }



private:
    Msg(){};
    static Msg _instance;
    bool _loaded = false;
    rjson::Document _doc;

    void load(){
        _loaded = true;
        std::ifstream fin;
        auto data = cocos2d::FileUtils::getInstance()->getDataFromFile("message.json");
        unsigned char * pc = (unsigned char *)malloc(sizeof(unsigned char) * (data.getSize() + 1));
        for (int i = 0; i < data.getSize(); i++) {
            pc[i] = data.getBytes()[i];
        }
        pc[data.getSize()] = '\0';

        _doc.Parse((char*)pc);

        free(pc);
    }
};

#endif /* defined(__simwar__Msg__) */
