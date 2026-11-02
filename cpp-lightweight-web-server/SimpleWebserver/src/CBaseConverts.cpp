#include "CBaseConverts.h"

CBaseConverts::CBaseConverts()
{
    //ctor
}

CBaseConverts::~CBaseConverts()
{
    //dtor
}

string CBaseConverts::intToStr(int num)
{
    ostringstream oss;
    oss << num;
    return oss.str();
}

vector<string> CBaseConverts::split(const string &str, const char &sp)
{
    vector<string> reVector;
    vector<int> index;
    bool flag = false;
    for(size_t i = 0; i < str.length(); i++)
    {
        if(str[i] == sp)
        {
            index.push_back(i);
            flag = true;
        }
    }
    if(flag)
    {
        index.push_back(str.length());
    }
    if(flag)
    {
        for(size_t i = 0, pos = 0; i < index.size(); i++)
        {
            reVector.push_back(str.substr(pos, index.at(i) - pos));
            pos = index.at(i) + 1;
        }
        return reVector;
    }
    else
    {
        reVector.push_back(str);
        return reVector;
    }
}

map<string, string> CBaseConverts::getRequestData(const string &str)
{
    vector<string> temp_map, id_data_vector = this->split(str, '&');
    map<string, string> id_data_map;
    for(size_t i = 0; i < id_data_vector.size(); i++)
    {
        temp_map = this->split(id_data_vector.at(i), '=');
        id_data_map[temp_map.at(0)] = temp_map.at(1);
    }

    return id_data_map;
}
