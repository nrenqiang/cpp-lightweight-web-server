#include "../include/CNetService.h"
// #include <string.h>
#include <map>
#include <fstream>

CNetService::CNetService()
{
    //ctor
}

map<string, string> CNetService::getMethodUrl(const string &req)
{
    map<string, string> remap;
    string HTTP = "HTTP/1.1";
    // 获取第一次匹配HTTP的位置
    size_t pos = req.find(HTTP);

    string headHttpStr = req.substr(0, pos - 1);

    remap["method"] = headHttpStr.substr(0,headHttpStr.find_first_of(" "));
    remap["url"] = headHttpStr.substr(headHttpStr.find_first_of(" ") + 1);

    return remap;
}

string CNetService::getFileContent(const string &filePath, const string &extend)
{
    string content;

    if(filePath.empty() || extend.empty())
    {
        cout << "getFileContent() Error!" << endl;
        return content;
    }
    if(!extend.compare("html") || !extend.compare("js") || !extend.compare("css"))
    {
        content = getTxtFile(filePath);
    }
    else
    {
        content = getBinaryFile(filePath);
    }

    return content;

}

string CNetService::getBinaryFile(const string & filePath)
{
    string content;
    ifstream file;
    file.open(filePath.c_str(), ios::binary | ios::ate);
    if(!file.is_open())
    {
        cout<< "open file error 1" << endl;
        return content;
    }

    // 获得文件大小
    streamsize fileSize = file.tellg();

    // 文件定位
    file.seekg(0, ios::beg);

    content.resize(fileSize);
    file.read(&content[0], fileSize);
    return content;
}

string CNetService::getTxtFile(const string & filePath)
{
    string content, tConten;
    fstream file;
    file.open(filePath.c_str(), ios::in);
    if(!file)
    {
        cout<<"open file error 2"<<endl;
        return content;
    }
    while(getline(file, tConten))
    {
        content += tConten;
    }
    file.close();
    return content;
}

string CNetService::getExtendName(const string & extend)
{
    string reurl;
    for(size_t i = 0; i < extend.length(); i++)
    {
        char ch = extend.at(i);
        if(ch >= 'a' && ch <= 'z')
        {
            reurl += ch;
        }
        else
        {
            reurl += ch + 32;
        }
    }
    return reurl;
}


CNetService::~CNetService()
{
    //dtor
}
