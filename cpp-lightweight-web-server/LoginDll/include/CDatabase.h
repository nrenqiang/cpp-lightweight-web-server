/**
 * CDatabase - SQLite3 数据库操作封装模块
 *
 * ===== SQLite 嵌入式数据库介绍 =====
 * SQLite 是一个零配置、无服务器的嵌入式关系型数据库引擎。
 * 特点：
 *   - 无需安装或管理数据库服务进程，整个数据库就是一个 .db 文件
 *   - 支持标准 SQL 语法（CREATE、INSERT、SELECT、UPDATE、DELETE 等）
 *   - 支持事务（ACID），断电/崩溃不会损坏数据
 *   - 单文件部署：引入 sqlite3.c + sqlite3.h 两个文件即可使用
 *   - 轻量级：编译后约 1MB，适合嵌入桌面应用或小型 Web 服务
 *
 * 本模块对 sqlite3 C API 做了轻量 C++ 封装，提供：
 *   - 构造时打开/创建数据库（RAII 模式，析构自动关闭）
 *   - executeSQL()：执行 INSERT / UPDATE / DELETE / CREATE 等非查询语句
 *   - query()：执行 SELECT 查询，结果以 vector<map<string,string>> 返回
 */

#ifndef CDATABASE_H
#define CDATABASE_H

#include "sqlite3.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class CDatabase
{
public:
    /**
     * 构造函数 —— 打开或创建 SQLite 数据库文件
     * @param dbPath  数据库文件路径（如 "users.db" 或绝对路径）
     *                若文件不存在则自动创建
     */
    CDatabase(const string& dbPath);

    /**
     * 析构函数 —— 自动关闭数据库连接（RAII）
     */
    ~CDatabase();

    /**
     * 执行非查询 SQL（INSERT / UPDATE / DELETE / CREATE 等）
     * @param sql  要执行的 SQL 语句
     * @return     成功返回 true，失败打印错误并返回 false
     */
    bool executeSQL(const string& sql);

    /**
     * 执行 SELECT 查询，返回结果集
     * @param sql       SELECT 查询语句
     * @param results   输出参数：每行是一个 map<列名, 列值>
     * @return          成功返回 true
     */
    bool query(const string& sql, vector<map<string, string> >& results);

    /**
     * 获取原生 sqlite3* 句柄（高级操作用）
     */
    sqlite3* getHandle() { return m_db; }

private:
    sqlite3* m_db;  // SQLite 数据库连接句柄

    // 查询回调：将每行结果写入 vector<map>
    static int queryCallback(void* data, int argc, char** argv, char** colName);
};

#endif // CDATABASE_H
