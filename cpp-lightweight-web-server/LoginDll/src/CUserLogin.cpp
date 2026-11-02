#define BUILDING_DLL
#include "../include/CUserLogin.h"
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <cstdio>
#include <iostream>

using namespace std;

CUserLogin::CUserLogin()
{
    // ---- 计算数据库文件路径：与 DLL 同目录的 data/users.db ----
    char dllPath[MAX_PATH] = {0};
    HMODULE hModule = NULL;

    static int dummy = 0;
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&dummy,
        &hModule
    );

    if (hModule)
    {
        GetModuleFileNameA(hModule, dllPath, MAX_PATH);
    }

    string dllDir(dllPath);
    size_t pos = dllDir.rfind('\\');
    if (pos != string::npos)
    {
        dllDir = dllDir.substr(0, pos);
    }

    string dataDir = dllDir + "\\data";
    CreateDirectoryA(dataDir.c_str(), NULL);

    m_dbPath = dataDir + "\\users.db";
    m_db = new CDatabase(m_dbPath);
    initDatabase();
}

CUserLogin::~CUserLogin()
{
    if (m_db) { delete m_db; m_db = NULL; }
}

// ============================================================
//  初始化数据库
// ============================================================
bool CUserLogin::initDatabase()
{
    if (!m_db || !m_db->getHandle()) return false;

    const string createSQL =
        "CREATE TABLE IF NOT EXISTS users ("
        "  id       INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  account  TEXT    NOT NULL UNIQUE,"
        "  password TEXT    NOT NULL,"
        "  role     TEXT    DEFAULT 'user',"
        "  created  TEXT    DEFAULT (datetime('now','localtime'))"
        ");";

    if (!m_db->executeSQL(createSQL)) return false;

    vector<map<string, string> > results;
    if (!m_db->query("SELECT COUNT(*) AS cnt FROM users;", results)) return false;

    if (!results.empty() && atoi(results[0]["cnt"].c_str()) == 0)
    {
        m_db->executeSQL(
            "INSERT INTO users (account, password, role) "
            "VALUES ('zhang', '1234', 'admin');");
        cout << "[CUserLogin] default admin created (zhang/1234)" << endl;
    }
    return true;
}

// ============================================================
//  登录验证
// ============================================================
bool CUserLogin::checkLogin(const string& uName, const string& uPwd)
{
    if (!m_db || !m_db->getHandle()) return false;

    char sql[512];
    snprintf(sql, sizeof(sql),
        "SELECT * FROM users WHERE account='%s' AND password='%s';",
        uName.c_str(), uPwd.c_str());

    vector<map<string, string> > results;
    return m_db->query(sql, results) && !results.empty();
}

// ============================================================
//  用户注册
// ============================================================
bool CUserLogin::registerUser(const string& uName, const string& uPwd)
{
    if (!m_db || !m_db->getHandle()) return false;

    char checkSql[256];
    snprintf(checkSql, sizeof(checkSql),
        "SELECT * FROM users WHERE account='%s';", uName.c_str());

    vector<map<string, string> > results;
    if (m_db->query(checkSql, results) && !results.empty())
    {
        cout << "[CUserLogin] user exists: " << uName << endl;
        return false;
    }

    char insertSql[512];
    snprintf(insertSql, sizeof(insertSql),
        "INSERT INTO users (account, password, role) VALUES ('%s', '%s', 'user');",
        uName.c_str(), uPwd.c_str());

    return m_db->executeSQL(insertSql);
}

// ============================================================
//  密码重置 —— 用户存在则更新密码
// ============================================================
bool CUserLogin::resetPassword(const string& uName, const string& newPwd)
{
    if (!m_db || !m_db->getHandle()) return false;

    // 先查用户是否存在
    char checkSql[256];
    snprintf(checkSql, sizeof(checkSql),
        "SELECT * FROM users WHERE account='%s';", uName.c_str());

    vector<map<string, string> > results;
    if (!m_db->query(checkSql, results) || results.empty())
    {
        cout << "[CUserLogin] user not found: " << uName << endl;
        return false;
    }

    // 存在 → 更新密码
    char updateSql[512];
    snprintf(updateSql, sizeof(updateSql),
        "UPDATE users SET password='%s' WHERE account='%s';",
        newPwd.c_str(), uName.c_str());

    return m_db->executeSQL(updateSql);
}

// ============================================================
//  GET 请求（登录）
// ============================================================
string CUserLogin::doGet(const map<string, string>& para)
{
    string uName = para.at("account");
    string uPwd  = para.at("pwd");

    if (checkLogin(uName, uPwd))
    {
        return "HTTP/1.1 302 Found\r\n"
               "Location: /webapps/admin.html\r\n"
               "Connection: close\r\n\r\n";
    }
    else
    {
        return "HTTP/1.1 302 Found\r\n"
               "Location: /webapps/login.html?error=1\r\n"
               "Connection: close\r\n\r\n";
    }
}

// ============================================================
//  POST 请求 —— 按 action 参数分发
//    action=register → 注册新用户
//    action=reset    → 重置密码
//    无 action       → 登录（默认）
// ============================================================
string CUserLogin::doPost(const map<string, string>& para)
{
    string action;
    map<string, string>::const_iterator it = para.find("action");
    if (it != para.end()) { action = it->second; }

    if (action == "register")
    {
        // -------- 注册流程 --------
        string uName = para.at("account");
        string uPwd  = para.at("pwd");

        if (registerUser(uName, uPwd))
        {
            return "HTTP/1.1 302 Found\r\n"
                   "Location: /webapps/login.html?registered=1\r\n"
                   "Connection: close\r\n\r\n";
        }
        else
        {
            return "HTTP/1.1 302 Found\r\n"
                   "Location: /webapps/register.html?error=1\r\n"
                   "Connection: close\r\n\r\n";
        }
    }
    else if (action == "reset")
    {
        // -------- 重置密码流程 --------
        string uName = para.at("account");
        string uPwd  = para.at("pwd");

        if (resetPassword(uName, uPwd))
        {
            return "HTTP/1.1 302 Found\r\n"
                   "Location: /webapps/login.html?resetok=1\r\n"
                   "Connection: close\r\n\r\n";
        }
        else
        {
            return "HTTP/1.1 302 Found\r\n"
                   "Location: /webapps/forgetpsw.html?error=1\r\n"
                   "Connection: close\r\n\r\n";
        }
    }
    else
    {
        // -------- 默认：登录流程 --------
        return this->doGet(para);
    }
}

// ============================================================
//  DLL 导出函数
// ============================================================
extern "C" DLL_API IWebServer* createServer()
{
    return new CUserLogin();
}

extern "C" DLL_API string doGet(IWebServer* pWebServer, const map<string, string>& para)
{
    return pWebServer->doGet(para);
}

extern "C" DLL_API string doPost(IWebServer* pWebServer, const map<string, string>& para)
{
    return pWebServer->doPost(para);
}
