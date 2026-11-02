#ifndef CNETSERVICE_H
#define CNETSERVICE_H
#include <iostream>
#include <string>
#include <map>
using namespace std;

class CNetService
{
    public:
        CNetService();
        virtual ~CNetService();
    public:
        map<string, string> getMethodUrl(const string &);
        string getFileContent(const string &, const string &);

        // 规范拓展名格式
        string getExtendName(const string &);
    protected:
        string getTxtFile(const string &);
        string getBinaryFile(const string &);

    private:
};

#endif // CNETSERVICE_H
