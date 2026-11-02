#include "Config.h"

Config::Config()
{
    string curPath = "";
    // 读取配置文件
    string appPath(1024, 0);
    // 获取可执行程序
    GetModuleFileName(0, &appPath[0], appPath.length());
    int pos = appPath.rfind("\\");
    curPath = appPath.substr(0, pos);
    string configPath = curPath + this->config;

    try
    {
        // 读取文件
        fstream file;
        file.open(configPath.c_str(), ios::in);
        if(!file)
        {
            throw "config file not found!";
        }
        string line;
        while(getline(file, line))
        {
            int posEqual = line.find("=");
            string key = line.substr(0, posEqual);
            string value = line.substr(posEqual + 1, line.length());
            if(!key.compare("port"))
            {
                this->PORT = atoi(value.c_str());
            }
            if(!key.compare("website"))
            {
                this->WEB_SITE = value;
            }
        }
    }
    catch(...)
    {
        this->PORT = 8888;

        this->WEB_SITE = "D:/SimpleWebserver/SimpleWebserver/bin/Debug";
    }
}

int Config::getPort()

{
    return this->PORT;
}

string Config::getWEB_SITE()
{
    return this->WEB_SITE;
}

Config::~Config()
{
    //dtor
}
