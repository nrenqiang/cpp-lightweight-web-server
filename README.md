# C++ 轻量级 Web 服务器

这是一个运行在 Windows 平台上的轻量级 Web 服务器示例。项目使用 Winsock2 实现 HTTP 服务，支持静态资源访问，并通过动态加载 DLL 提供登录、注册和密码重置接口。示例业务数据由项目内置的 SQLite 3 保存。

> 本项目主要用于 C++ 网络编程、DLL 热插拔接口和 SQLite 集成练习，目前不应直接用于生产环境。

## 功能特性

- 使用 Winsock2 创建 TCP 服务，监听配置文件中指定的端口。
- 支持返回 `html`、`css`、`js`、`ico`、`jpg`、`jpeg` 和 `gif` 静态资源。
- 对于普通 `GET` 请求，根据 URL 读取 `WEB_SITE` 目录下的文件。
- 对于 `POST` 请求，将 URL 查询参数和请求体参数解析为 `map<string, string>`。
- 根据请求 URL 动态加载对应的 DLL，并调用 `createServer`、`doGet` 和 `doPost` 导出函数。
- 提供登录、注册和密码重置演示页面。
- 使用 SQLite 数据库保存用户信息，数据库文件随 DLL 一起部署。
- 使用 302 重定向在登录、注册和重置流程之间跳转。

## 技术栈

- C++（Windows API）
- Winsock2
- SQLite 3（源码已包含在项目中）
- HTML、CSS、JavaScript
- Code::Blocks（项目文件为 `.cbp`）
- MinGW/GCC 工具链

## 项目结构

```text
cpp-lightweight-web-server/
├─ README.md
├─ SimpleWebserver/
│  ├─ SimpleWebserver.cbp          # Web 服务器 Code::Blocks 工程
│  ├─ include/                     # Web 服务器头文件
│  ├─ src/
│  │  ├─ testNetMain.cpp           # 程序入口
│  │  ├─ CNet.cpp                  # Socket 监听与 HTTP 请求处理
│  │  ├─ CNetService.cpp           # 请求行解析与静态文件读取
│  │  ├─ Config.cpp                # 配置文件解析
│  │  ├─ CLoadDll.cpp              # DLL 加载与接口调用
│  │  └─ CBaseConverts.cpp         # 字符串和请求参数处理
│  └─ bin/Debug/
│     ├─ SimpleWebserver.exe       # 当前已有构建产物
│     ├─ conf/config.ini           # 服务配置
│     └─ webapps/
│        ├─ login.html
│        ├─ register.html
│        ├─ forgetpsw.html
│        ├─ admin.html
│        └─ Dll/
│           ├─ LoginDll.dll        # 登录业务动态库
│           └─ data/users.db       # SQLite 用户数据库
└─ LoginDll/
   ├─ LoginDll.cbp                 # 登录业务 Code::Blocks 工程
   ├─ include/
   │  ├─ IWebServer.h              # DLL 统一接口
   │  ├─ CUserLogin.h              # 登录、注册、重置密码业务
   │  ├─ CDatabase.h               # SQLite 封装
   │  └─ sqlite3.h
   └─ src/
      ├─ CUserLogin.cpp
      ├─ CDatabase.cpp
      └─ sqlite3.c
```

`obj/` 和 `bin/` 中的大部分内容是构建产物，修改源码后应由编译工具重新生成。

## 工作原理

### 请求处理流程

1. `main()` 创建 `CNet` 对象，读取 `conf/config.ini`，初始化 Winsock 并监听端口。
2. 服务器接受客户端连接，持续读取请求，直到发现请求头结束标记 `\r\n\r\n`。
3. `GET` 请求由 `CNet::getMethod()` 处理，文件路径由 `WEB_SITE` 与 URL 拼接得到。
4. `POST` 请求由 `CNet::postMethod()` 处理，URL 查询参数和请求体参数会被解析为键值对。
5. 动态请求的 URL 被当作 DLL 路径，`CLoadDll` 使用 `LoadLibraryA` 加载该文件。
6. DLL 必须导出 `createServer`、`doGet` 和 `doPost` 三个 C 接口。
7. 业务处理结束后，DLL 返回完整 HTTP 响应，主程序通过 Socket 发回客户端。

### DLL 接口

`IWebServer.h` 定义了主程序与业务 DLL 之间的统一契约：

```cpp
class IWebServer
{
public:
    virtual string doGet(const map<string, string>&) = 0;
    virtual string doPost(const map<string, string>&) = 0;
};

extern "C" IWebServer* createServer();
extern "C" string doGet(IWebServer*, const map<string, string>&);
extern "C" string doPost(IWebServer*, const map<string, string>&);
```

编写新的业务 DLL 时，需要实现相同接口并导出这三个函数。请求 URL 应与 DLL 在 `WEB_SITE` 下的相对路径一致。

## 环境要求

- Windows 系统。
- Code::Blocks，并配置可用的 MinGW/GCC 编译器；或自行准备等价的 MinGW-w64 命令行工具链。
- 无需单独安装 SQLite，项目已经包含 `sqlite3.c` 和 `sqlite3.h`。
- 默认使用项目提供的 Code::Blocks 工程，不需要 CMake。

## 构建项目

建议先构建 `LoginDll`，再构建 `SimpleWebserver`，这样便于直接验证动态库加载流程。

### 方式一：使用 Code::Blocks

1. 使用 Code::Blocks 打开 `LoginDll/LoginDll.cbp`。
2. 选择 `Debug` 目标并执行构建，生成 `LoginDll/bin/Debug/LoginDll.dll`。
3. 将生成的 DLL 复制到 `SimpleWebserver/bin/Debug/webapps/Dll/`。
4. 使用 Code::Blocks 打开 `SimpleWebserver/SimpleWebserver.cbp`。
5. 选择 `Debug` 目标并执行构建，生成 `SimpleWebserver/bin/Debug/SimpleWebserver.exe`。
6. 检查并修改 `SimpleWebserver/bin/Debug/conf/config.ini` 中的 `website`。

### 方式二：使用 MinGW 命令行

以下命令需要从项目根目录执行，并且 `gcc`、`g++` 已加入 `PATH`。

构建登录 DLL：

```bat
cd LoginDll
mkdir obj\Debug 2>nul
mkdir bin\Debug 2>nul

gcc -g -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION ^
    -c src\sqlite3.c -o obj\Debug\sqlite3.o

g++ -std=c++11 -Wall -g -DBUILD_DLL ^
    -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION ^
    -Iinclude -shared ^
    src\CDatabase.cpp src\CUserLogin.cpp obj\Debug\sqlite3.o ^
    -luser32 -o bin\Debug\LoginDll.dll

cd ..
```

构建 Web 服务器：

```bat
mkdir SimpleWebserver\bin\Debug 2>nul

g++ -std=c++11 -Wall -g -ISimpleWebserver\include ^
    SimpleWebserver\src\CBaseConverts.cpp ^
    SimpleWebserver\src\CLoadDll.cpp ^
    SimpleWebserver\src\CNet.cpp ^
    SimpleWebserver\src\CNetService.cpp ^
    SimpleWebserver\src\Config.cpp ^
    SimpleWebserver\src\testNetMain.cpp ^
    -lws2_32 -o SimpleWebserver\bin\Debug\SimpleWebserver.exe
```

构建完成后，将 `LoginDll/bin/Debug/LoginDll.dll` 复制到：

```text
SimpleWebserver/bin/Debug/webapps/Dll/LoginDll.dll
```

## 配置

配置文件位于可执行文件同目录下的 `conf/config.ini`：

```ini
port=9999
website=E:/path/to/cpp-lightweight-web-server/SimpleWebserver/bin/Debug
```

配置说明：

- `port`：HTTP 服务监听端口。
- `website`：静态资源和动态 DLL 的根目录，必须使用当前机器上实际存在的绝对路径。
- 程序根据可执行文件位置查找配置，不依赖启动时的当前工作目录。
- 当前文件中的 `D:/SimpleWebserver/...` 是旧机器路径；在本地运行前必须改成当前项目路径。
- 路径末尾不要添加额外的反斜杠或正斜杠。

如果配置文件无法打开，程序会回退到端口 `8888` 和 `D:/SimpleWebserver/SimpleWebserver/bin/Debug`。该回退路径通常不适用于当前仓库，因此建议始终正确配置 `config.ini`。

## 运行

直接运行：

```bat
SimpleWebserver\bin\Debug\SimpleWebserver.exe
```

启动后可通过以下地址访问页面：

```text
http://127.0.0.1:9999/webapps/login.html
http://127.0.0.1:9999/webapps/register.html
http://127.0.0.1:9999/webapps/forgetpsw.html
```

说明：

- 访问根路径 `/` 时，服务器查找的是 `website/index.html`，而当前示例首页位于 `website/webapps/index.html`。
- 页面、DLL 和数据库都位于 `SimpleWebserver/bin/Debug/webapps/` 下。
- 使用 `Ctrl+C` 停止前台运行的服务。

## 登录接口

页面提交地址为：

```text
POST /Dll/LoginDll.dll
```

登录请求示例：

```bat
curl.exe -i -X POST "http://127.0.0.1:9999/Dll/LoginDll.dll" ^
  --data "account=zhang&pwd=1234"
```

注册请求示例：

```bat
curl.exe -i -X POST "http://127.0.0.1:9999/Dll/LoginDll.dll" ^
  --data "action=register&account=test&pwd=123456"
```

密码重置请求示例：

```bat
curl.exe -i -X POST "http://127.0.0.1:9999/Dll/LoginDll.dll" ^
  --data "action=reset&account=test&pwd=654321"
```

`LoginDll` 根据 `action` 参数分发请求：

| `action` | 功能 | 必需参数 | 成功跳转 |
| --- | --- | --- | --- |
| 不传 | 登录 | `account`、`pwd` | `/webapps/admin.html` |
| `register` | 注册新用户 | `account`、`pwd` | `/webapps/login.html?registered=1` |
| `reset` | 重置密码 | `account`、`pwd` | `/webapps/login.html?resetok=1` |

## 默认账户与数据库

当 `users` 表为空时，登录 DLL 会创建默认管理员：

```text
账户：zhang
密码：1234
```

数据库默认位置：

```text
SimpleWebserver/bin/Debug/webapps/Dll/data/users.db
```

数据表结构：

```sql
CREATE TABLE users (
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    account  TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL,
    role     TEXT DEFAULT 'user',
    created  TEXT DEFAULT (datetime('now', 'localtime'))
);
```

密码当前以明文保存。首次启动时只会在用户表为空的情况下创建默认管理员。

## 已知限制与安全提示

该项目适合学习和功能演示，但存在以下限制：

- 仅支持 Windows，依赖 Winsock2、`LoadLibraryA` 等 Windows API。
- 主循环一次只处理一个连接，属于单线程阻塞模型，没有连接池或并发处理。
- 请求缓冲区固定为 1024 字节，未完整处理 `Content-Length`、分块传输和超大请求。
- 静态文件路径直接由 URL 拼接，没有规范化路径或目录访问控制。
- POST 参数解析没有进行 URL 解码，也没有处理复杂的表单编码。
- SQL 通过字符串拼接生成，存在 SQL 注入风险。
- 用户密码未加密、未加盐，并直接保存在 SQLite 中。
- 页面没有服务端会话和权限校验，`admin.html` 并不构成真实的访问控制。
- 部分 HTML 使用 GBK，部分文件使用 UTF-8，而服务器响应头统一声明为 GBK，可能出现中文乱码。
- 文件不存在时仍可能返回 `200` 状态码和空内容，错误状态码处理不完整。

在实际项目中使用前，应至少增加线程池或异步 I/O、路径校验、参数解码、参数化 SQL、密码哈希、会话鉴权、请求体长度限制，以及统一的 UTF-8 编码和完整的 HTTP 状态码处理。

## 许可证

当前项目目录未包含 `LICENSE` 文件。在复制、分发或用于其他项目之前，请先确认代码和第三方组件（尤其是 SQLite）的授权方式，并补充相应的许可证文件。
