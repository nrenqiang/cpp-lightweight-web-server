#ifndef CLOADDLL_H
#define CLOADDLL_H
#include "IWebServer.h"
#include <windows.h>
#include <map>
#include <string>

using namespace std;
typedef IWebServer* (*CREATE_SERVER)();
typedef string (*DO_GET)(IWebServer*, map<string, string>);
typedef string (*DO_POST)(IWebServer*, map<string, string>);

class CLoadDll
{
    public:

        string doGet (const map<string,string> &);
        string doPost(const map<string,string> &);

        CLoadDll(const string &);
        virtual ~CLoadDll();

    protected:

    private:
        HMODULE loadDll;
        IWebServer* pWebServer;
        DO_GET fp_doGet;
        DO_POST fp_doPost;
};

#endif // CLOADDLL_H
