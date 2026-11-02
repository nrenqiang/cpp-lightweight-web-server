#include "../include/CNet.h"

using namespace std;


CNet::CNet()
{
    string curPath = "";
    try
    {
        Config config;
        this->PORT = config.getPort();
        this->WEB_SITE = config.getWEB_SITE();
    }
    catch(...)
    {
        this->PORT = 8888;
        this->WEB_SITE = curPath + "\\webapps";
    }

    // this->WEB_SITE = "D:/SimpleWebserver/SimpleWebserver/webapps";

    // 1.初始化windows socket
    WSAData wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    // 2.创建socke对象
    this->m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // 3.绑定端口号、IP地址等
    this->m_serverAddr.sin_family = AF_INET;
    this->m_serverAddr.sin_addr.s_addr = INADDR_ANY;
    this->m_serverAddr.sin_port   = htons(this->PORT);
    bind(this->m_serverSocket, (SOCKADDR*)&this->m_serverAddr, sizeof(this->m_serverAddr));

    // 4.监听
    listen(this->m_serverSocket, 5);
    cout << "服务器启动：127.0.0.1" << "端口：" << this->PORT << endl;
    cout << "服务器启动站点："<< this->WEB_SITE << endl;

}

void CNet::start()
{
    char buffer[1024] = {'\0'};
    while(true)
    {
        SOCKET clientSocket = accept(this->m_serverSocket, NULL, NULL);
        int time = 1000;
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&time, sizeof(time));
        // 获得客户端请求
        string request;

        // 查收请求
        while(true)
        {
            memset(buffer, 0, sizeof(buffer));
            int len = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if(len < 0) break;
            request += buffer;
            if(request.find("\r\n\r\n") != string::npos) break;
        }
        cout << "完整请求\n" << request << endl;

        CNetService cNetService;

        // 请求还没发完整，没有 \r\n\r\n 或 空的request
        if(request.find("\r\n\r\n") == string::npos)
        {
            closesocket(clientSocket);
            continue;
        }

        map<string, string> methodUrlMap = cNetService.getMethodUrl(request);
        if(!methodUrlMap.empty())
        {
            string method = methodUrlMap["method"];
            string url = methodUrlMap["url"];
            if(!method.compare("GET"))
            {
                // string url = methodUrlMap["url"];
                string response = this->getMethod(url);
                send(clientSocket, response.c_str(), response.length(), 0);
            }
            else
            {
                size_t startIndex = request.find("\r\n\r\n") + 4;
                // cout << "现在的请求为：\n" << request << endl;

                // 参数字符串
                string paramersStr = request.substr(startIndex);

                string response = this->postMethod(url, paramersStr);
                if(!response.empty())
                {
                    send(clientSocket, response.c_str(), response.length(), 0);
                }

            }
        }
        else
            {
                // 这个位置先做测试，以后再改
                const string response = "HTTP/1.1 200 ok\r\n"
                                "Content-Type:text/html;charset=gbk\r\n"
                                "Connection:close\r\n\r\n"
                                "<h1> C++ Web Server </h1>";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        closesocket((clientSocket));

    }

}

string CNet::postMethod(const string& url, const string& post_Para)
{
    string tUrl = url;
    string response;
    // 加载动态连接库的方法
    // 解析参数
    // 解析url中的参数

    // 存url参数 key-value
    map<string, string> parMap;

    if(tUrl.rfind("?") != std::string::npos)
    {
        vector<string> splitUrl = this->split(tUrl, '?');
        tUrl = splitUrl.at(0);
        parMap = this->getRequestData(splitUrl.at(1));
    }
    if(!post_Para.empty())
    {
        if(post_Para.rfind("&") != std::string::npos)
        {
            parMap = this->getRequestData(post_Para);
        }
    }
    else
    {
        cout << "post_Para为非空时：" << post_Para.length() << endl;;
    }

    map<string, string>::const_iterator it = parMap.find("account");

    response = this->postMethodWithParameters(tUrl, parMap);

    // 如果DLL返回了完整HTTP响应（如302重定向），直接透传
    if(response.find("HTTP/") == 0)
    {
        return response;
    }

    /*跳页面*/
    response = this->RESPONSE_HEADER_HTML + response;
    return response;

}

string CNet::postMethodWithParameters(const string& url, const map<string, string>& para_map)
{
    string response ="";
    string realPath = this->WEB_SITE + url;

    /** 加载动态链接库 **/
    try{
        string realPath = this->WEB_SITE + url;
        CLoadDll loadDll(realPath);
        response = loadDll.doPost(para_map);
        }
     catch(...)
     {
         /*html 500*/
        response = RESPOSE_HEADER_ERROR;
        response += "<h1>服务器内部错误</h1>";  /*读取一个网页*/
     }
     return response;

}

string CNet::getMethodWithouParameters(const string& url, const map<string, string>& id_key_map)
{


    CNetService cNetService;
    // 分析请求资源
    // 2种情况，1）请求默认资源；2）请求给定资源：2.1）请求给定资源的默认资源；2.2）请求给定资源的制定资源
    char lUrl = url[url.length() - 1];

    switch(lUrl)
    {
    case'/':{
        string filePath = this->WEB_SITE + url + "index.html";

        string fileContent = cNetService.getFileContent(filePath, "html");

        fileContent = RESPONSE_HEADER_HTML + fileContent;
        return fileContent;
        }
    default:{
        // 不是默认资源
        // 1)不是html、js、css结尾的
        // 2)不是jig、png
        // 3）不是dll结尾的
        // 通过拓展名区分
        int pos = url.find(".");
        string extend = url.substr(pos + 1, url.length());

        // 格式化文件拓展名
        extend = cNetService.getExtendName(extend);

        if(!extend.compare("html") || !extend.compare("js") || !extend.compare("css") || !extend.compare("ico") || !extend.compare("jpg")|| !extend.compare("jpeg")|| !extend.compare("gif"))
        {
            string request_header;

            string filePath = this->WEB_SITE + url;

            string fileContent = request_header + cNetService.getFileContent(filePath, extend);

            if(!extend.compare("html"))
            {
                request_header = this->RESPONSE_HEADER_HTML;
            }
            else if(!extend.compare("css"))
            {
                request_header = this->RESPONSE_HEADER_CSS;
            }
            else if(!extend.compare("js"))
            {
                request_header = this->RESPONSE_HEADER_JS;
            }
            else if(!extend.compare("ico"))
            {
                request_header = RESPONSE_HEADER_ICO + this->intToStr(fileContent.length());
                request_header += RESPONSE_HEADER_END;
            }
            else if(!extend.compare("jpg")|!extend.compare("jpeg"))
            {
                request_header = RESPONSE_HEADER_JPG + this->intToStr(fileContent.length());
                request_header += RESPONSE_HEADER_END;
            }
            else if(!extend.compare("gif"))
            {
                request_header = RESPONSE_HEADER_GIF + this->intToStr(fileContent.length());
                request_header += RESPONSE_HEADER_END;
            }

            fileContent = request_header + fileContent;
            return fileContent;
        }
        else if(!extend.compare("dll"))
        {
            cout << "!extend.compare(\"dll\"看我" << endl;// 动态请求
        }
    }
    }
}

string CNet::getMethod(const string & url)
{
    string response;

    // 存url参数 key-value
    map<string, string> parMap;

    string requetUrl;

    // CNetService cNetService;
    if(url.rfind("?") == std::string::npos)
    {
        requetUrl = url;
    }
    else
    {

        vector<string> splitUrl = this->split(url, '?');
        requetUrl = splitUrl.at(0);
        parMap = this->getRequestData(splitUrl.at(1));

    }
    response = this->getMethodWithouParameters(requetUrl, parMap);

    return response;
}


CNet::~CNet()
{
    // 把服务器关了
    closesocket(this->m_serverSocket);
    WSACleanup();
}
