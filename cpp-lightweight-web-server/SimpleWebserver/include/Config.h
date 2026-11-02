#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <iostream>
#include <windows.h>
#include <fstream>
using namespace std;

class Config
{
    public:
        Config();
        virtual ~Config();
        int getPort();
        string getWEB_SITE();

    protected:
        int PORT; // ¶Ë¿ÚºÅ
        string WEB_SITE; // ´æ´¢Õ¾µãÎ»ÖÃ
        string config = "\\conf\\config.ini";
    protected:


    private:
};

#endif // CONFIG_H
