#ifndef CBASECONVERTS_H
#define CBASECONVERTS_H
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <map>
using namespace std;

class CBaseConverts
{
    public:
        CBaseConverts();
        virtual ~CBaseConverts();

    protected:
        string intToStr(int);
        vector<string> split(const string &, const char &);
        map<string, string> getRequestData(const string &);
    private:
};



#endif // CBASECONVERTS_H
