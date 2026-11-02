#pragma once
#ifdef BUILDING_DLL
    #define DLL_API __declspec(dllexport)
#else
    #define DLL_API __declspec(dllimport)
#endif

#include <map>
#include <string>

using namespace std;

class  DLL_API IWebServer
{
    public:
        virtual string doGet(const map<string, string> &) = 0;
        virtual string doPost(const map<string, string> &) = 0;
};

extern "C" DLL_API IWebServer* createServer();
extern "C" DLL_API string doGet (IWebServer*, const map<string, string> &);
extern "C" DLL_API string doPost(IWebServer*, const map<string, string> &);
