#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <string>
#include <vector>
#include <map>
#include <QString>

using namespace std;

typedef map<string, string> StringMap;

class DataObject
{
public:
    static vector<StringMap> GetStringMaps(string fileName);

    DataObject();
    DataObject(StringMap data);
    virtual string ToString();

protected:
    StringMap dataMap;
    StringMap::iterator dataMapIter;

    int GetInt(string key);
    bool GetBool(string key);
    double GetDouble(string key);
    string GetString(string key);
    QString GetQString(string key);
    vector<string> GetStringVector(string key);

    void SetInt(string key, int val);
    void SetBool(string key, bool val);
    void SetDouble(string key, double val);
    void SetString(string key, string val);
    void SetQString(string key, QString val);
    void SetStringVector(string key, vector<string> val);
};

#endif // DATAOBJECT_H
