#include "CLoadDll.h"
#include <iostream>


CLoadDll::CLoadDll(const string& realPath)
{
    string response ="";

    this->loadDll = LoadLibraryA(realPath.c_str());
    if(!this->loadDll)
    {
        cout << "load dll error" << endl;
        throw "load dll error";
    }

    /**得到动态链接库的一个类对象**/
    CREATE_SERVER fp_cs = (CREATE_SERVER)GetProcAddress(loadDll, "createServer");

    this->fp_doGet = (DO_GET)GetProcAddress(loadDll, "doGet");
    this-> fp_doPost = (DO_POST)GetProcAddress(loadDll, "doPost");

    if(!fp_cs || !fp_doGet || !fp_doPost)
    {
       cout << "load dll error：load get() | post()" << endl;
       FreeLibrary(loadDll);
       throw "load dll error：load get() | post()";
    }
    this->pWebServer = fp_cs();
}

string CLoadDll::doGet (const map<string,string>& para)
{
    return this->fp_doGet(this->pWebServer, para);
}
string CLoadDll::doPost(const map<string,string>& para)
{
    return this->fp_doPost(this->pWebServer, para);
}

CLoadDll::~CLoadDll()
{
    if(this->pWebServer)
    {
        delete this->pWebServer;
    }
    if(this->loadDll)
    {
        FreeLibrary(this->loadDll);
    }
}
