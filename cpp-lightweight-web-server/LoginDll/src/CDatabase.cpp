#include "../include/CDatabase.h"

// ============================================================
//  构造函数 —— 打开或创建 SQLite 数据库文件
// ============================================================
CDatabase::CDatabase(const string& dbPath)
{
    // sqlite3_open_v2: 打开数据库文件，若不存在则自动创建
    int rc = sqlite3_open_v2(
        dbPath.c_str(),           // 数据库文件路径
        &m_db,                    // 输出：数据库句柄
        SQLITE_OPEN_READWRITE |   // 读写模式
        SQLITE_OPEN_CREATE,       // 不存在则创建
        NULL                      // 默认 VFS
    );

    if (rc != SQLITE_OK)
    {
        cerr << "[CDatabase] 无法打开数据库: " << sqlite3_errmsg(m_db) << endl;
        m_db = NULL;
        return;
    }

    // 启用 WAL 模式（Write-Ahead Logging），提高并发读写性能
    executeSQL("PRAGMA journal_mode=WAL;");
    // 启用外键约束
    executeSQL("PRAGMA foreign_keys=ON;");

    cout << "[CDatabase] 数据库已打开: " << dbPath << endl;
}

// ============================================================
//  析构函数 —— 关闭数据库（RAII）
// ============================================================
CDatabase::~CDatabase()
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = NULL;
        cout << "[CDatabase] 数据库已关闭" << endl;
    }
}

// ============================================================
//  执行非查询 SQL（INSERT/UPDATE/DELETE/CREATE 等）
// ============================================================
bool CDatabase::executeSQL(const string& sql)
{
    if (!m_db)
    {
        cerr << "[CDatabase] 数据库未打开，无法执行 SQL" << endl;
        return false;
    }

    char* errMsg = NULL;
    int rc = sqlite3_exec(m_db, sql.c_str(), NULL, NULL, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "[CDatabase] SQL 执行失败: " << errMsg << endl;
        cerr << "[CDatabase] SQL: " << sql << endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

// ============================================================
//  执行 SELECT 查询，结果以 vector<map<string,string>> 返回
//  每行是一个 map<列名, 列值>
// ============================================================
bool CDatabase::query(const string& sql, vector<map<string, string> >& results)
{
    if (!m_db)
    {
        cerr << "[CDatabase] 数据库未打开，无法查询" << endl;
        return false;
    }

    char* errMsg = NULL;
    int rc = sqlite3_exec(m_db, sql.c_str(), queryCallback, &results, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "[CDatabase] 查询失败: " << errMsg << endl;
        cerr << "[CDatabase] SQL: " << sql << endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

// ============================================================
//  查询回调函数（静态）
//  argc=列数, argv=列值数组, colName=列名数组
//  将每行数据填充到 vector<map<string,string>>
// ============================================================
int CDatabase::queryCallback(void* data, int argc, char** argv, char** colName)
{
    // data 指向 vector<map<string,string> >*
    vector<map<string, string> >* rows = static_cast<vector<map<string, string> >*>(data);

    map<string, string> row;
    for (int i = 0; i < argc; i++)
    {
        // colName[i] 为列名，argv[i] 为列值（NULL 转为空串）
        row[colName[i]] = (argv[i] ? argv[i] : "");
    }
    rows->push_back(row);

    return 0; // 返回 0 让 sqlite3_exec 继续处理后续行
}
