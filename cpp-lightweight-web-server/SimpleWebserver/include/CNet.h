#ifndef CNET_H
#define CNET_H
#include <iostream>
#include <winsock2.h>
#include <string>
#include "CNetService.h"
#include <map>
#include "CBaseConverts.h"
#include "Config.h"
#include "CLoadDll.h"

class CNet : public  CBaseConverts
{
    public:
        CNet();
        virtual ~CNet();

    public:
        void start();

    protected:
        SOCKET       m_serverSocket;
        sockaddr_in  m_serverAddr;
        int PORT; // ¶Ë¿ÚºÅ
        // string WEB_SITE;

    private:
        string       WEB_SITE;// 站点启动位置

        string getMethod(const string &);
        string getMethodWithouParameters(const string &, const map<string, string> &);

        string postMethod(const string&, const string&);
        string postMethodWithParameters(const string &, const map<string, string> &);

        const string RESPONSE_HEADER_HTML = "HTTP/1.1 200 ok\r\n"
                                "Content-Type:text/html;charset=gbk\r\n"
                                "Connection:close\r\n\r\n";
        const string RESPONSE_HEADER_CSS = "HTTP/1.1 200 ok\r\n"
                                "Content-Type:text/css;charset=gbk\r\n"
                                "Connection:close\r\n\r\n";
        const string RESPONSE_HEADER_JS = "HTTP/1.1 200 ok\r\n"
                                "Content-Type:text/javascript;charset=gbk\r\n"
                                "Connection:close\r\n\r\n";
        const string RESPONSE_HEADER_ICO = "HTTP/1.1 200 ok\r\n"
                                "Content-Type:image/x-icon\r\n"
                                "Content-Length:";
        const string RESPONSE_HEADER_JPG = "HTTP/1.1 200 ok\r\n"
                                "Content-Type:image/jpeg\r\n"
                                "Content-Length:";
        const string RESPONSE_HEADER_GIF = "HTTP/1.1 200 ok\r\n"
                            "Content-Type:image/gif\r\n"
                            "Content-Length:";
        const string RESPONSE_HEADER_END = "\r\nConnection:close\r\n\r\n";

        const string RESPOSE_HEADER_ERROR = "HTTP/1.1 500 ok\r\n"
                                "Content-Type:text/html;charset=gbk\r\n"
                                "Connection:close\r\n\r\n";

};

#endif // CNET_H
