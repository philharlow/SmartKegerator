#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <data/dataobject.h>
#include <QApplication>

using namespace std;

DataObject::DataObject() {}

DataObject::DataObject(StringMap data)
{
    this->dataMap = data;
}

vector<StringMap> DataObject::GetStringMaps(string fileName)
{
    vector<StringMap> dataVector;
    //qDebug("Loading StringMaps from %s", fileName.c_str());
    ifstream file(fileName.c_str());
    if (!file)
        return dataVector;

    string str;
    map<string, string> currentMap;
    bool inMap;
    while(getline(file, str))
    {
        if (inMap)
        {
            if (str.find('}') != string::npos)
            {
                dataVector.push_back(currentMap);
                inMap = false;
            }
            else
            {
                size_t index = str.find(':');
                if (index != string::npos)
                {
                    string val = str.substr(index+1);
                    size_t end = val.find('\r');
                    if (end != string::npos)
                        val = str.substr(index+1, end);

                    string key = str.substr(0, index);
                    size_t keyStart = key.find_last_of(' ');
                    key = key.substr(keyStart+1);

                    currentMap[key] = val;
                }
            }
        }
        else
        {
            if (str.find('{') != string::npos)
            {
                currentMap.clear();
                inMap = true;
            }
        }
    }

    //qDebug("%d StringMaps loaded from %s", dataVector.size(), fileName.c_str());

    return dataVector;
}

string DataObject::ToString()
{
    StringMap::iterator iter;
    string str;

    str.append("{\r\n");
    for (iter = dataMap.begin(); iter != dataMap.end(); ++iter)
    {
        str.append("    ");
        str.append(iter->first);
        str.append(":");
        str.append(iter->second);
        str.append("\r\n");
    }
    str.append("}\r\n");

    return str;
}



int DataObject::GetInt(string key) { return QString(dataMap[key].c_str()).toInt(); }
void DataObject::SetInt(string key, int val) { dataMap[key] = QString("%1").arg(val).toStdString(); }

double DataObject::GetDouble(string key) { return QString(dataMap[key].c_str()).toDouble(); }
void DataObject::SetDouble(string key, double val) { dataMap[key] = QString::number(val, 'f', 5).toStdString(); }

bool DataObject::GetBool(string key) { return dataMap[key] == "true"; }
void DataObject::SetBool(string key, bool val) { dataMap[key] = QString("%1").arg(val ? "true" : "false").toStdString(); }

string DataObject::GetString(string key) { return dataMap[key]; }
void DataObject::SetString(string key, string val) { dataMap[key] = val; }

QString DataObject::GetQString(string key) { return QString::fromStdString(dataMap[key]); }
void DataObject::SetQString(string key, QString val) { dataMap[key] = val.toStdString(); }

vector<string> DataObject::GetStringVector(string key)
{
    vector<string> vec;
    string str = dataMap[key];
    int index = 0;
    while (str.find(',', index) != string::npos)
    {
        string var = str.substr(index, str.find(',', index) - index);
        vec.push_back(var);

        index = str.find(',', index) + 1;
    }
    if (index < str.length())
    {
        string end = str.substr(index, str.length() - index);
        vec.push_back(end);
    }
    return vec;
}
void DataObject::SetStringVector(string key, vector<string> vals)
{
    string str;
    int i=0;
    for(vector<string>::iterator val = vals.begin(); val != vals.end(); val++)
    {
        if (i++ > 0)
            str.append(",");
        str.append(*val);
    }
    //str.append(",");

    dataMap[key] = str;
}
