#ifndef CUSERLOGIN_H
#define CUSERLOGIN_H

#include "CDatabase.h"
#include "IWebServer.h"
#include <string>

using namespace std;

class CUserLogin : public IWebServer
{
    public:
        CUserLogin();

        string doPost(const map<string, string>&);
        string doGet (const map<string, string>&);

        virtual ~CUserLogin();

    protected:

    private:
        CDatabase* m_db;        // 数据库操作封装对象
        string     m_dbPath;    // 数据库文件路径

        bool initDatabase();    // 初始化表结构 + 默认账户
        bool checkLogin(const string& uName, const string& uPwd);
        bool registerUser(const string& uName, const string& uPwd);
        bool resetPassword(const string& uName, const string& newPwd);

};

#endif // CUSERLOGIN_H
