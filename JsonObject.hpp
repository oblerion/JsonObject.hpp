#ifndef JSONOBJECT_HPP
#define JSONOBJECT_HPP
#include <cstdarg>
#include <cmath>
#include <vector>
#include <string>

class JsonObject
{
private:
    std::vector<std::string> _ds;
    std::vector<std::string>_ds_keys;
    std::vector<int> _di;
    std::vector<std::string> _di_keys;
    std::vector<float> _df;
    std::vector<std::string> _df_keys;
    std::vector<bool> _db;
    std::vector<std::string> _db_keys;
    std::vector<JsonObject> _dj;
    std::vector<std::string> _dj_keys;
    std::vector<std::vector<int>> _dai1;
    std::vector<std::string> _dai1_keys;
    std::vector<std::vector<std::string>> _das1;
    std::vector<std::string> _das1_keys;
    void _addArray(std::string name,std::string value);
    void _addObject(std::string name,std::string value);
    void _addString(std::string name,std::string value);
    void _addInt(std::string name,std::string value);
    void _addFloat(std::string name,std::string value);
    void _addBool(std::string name,std::string value);
    std::string _readFile(std::string sfile);
    std::string _filter(std::string s);
    std::string _extract(std::string s,char cbegin,char cend);
    std::string _extract(std::string s,char cbegin);

public:
    JsonObject();
    JsonObject(std::string sfile);
    ~JsonObject();
    void WriteFile(std::string sfile);
    std::string ToString();
    bool IsEmpty();
    void Print();
    std::vector<int> GetArray(std::string key);
    std::vector<std::string> GetArrayS(std::string key);
    bool GetBool(std::string key);
    std::string GetString(std::string key);
    int GetInt(std::string key);
    float GetFloat(std::string key);
    JsonObject GetObject(std::string key);
    JsonObject GetObject(int id);
    int GetNbObject();
    void SetArray(std::string key,std::vector<int> value);
    void SetArray(std::string key,std::vector<std::string> value);
    void SetObject(std::string key,JsonObject value);
    void SetBool(std::string key,bool value);
    void SetString(std::string key,std::string value);
    void SetInt(std::string key,int value);
    void SetFloat(std::string key,float value);
    void DelArray(std::string key);
    void DelObject(std::string key);
    void DelBool(std::string key);
    void DelString(std::string key);
    void DelInt(std::string key);
};
#endif
