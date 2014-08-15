#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <data/settings.h>
#include <QApplication>

using namespace std;

map<string, string> Settings::Map;
string fileName;

bool Settings::Load(string fn)
{
    fileName = fn;
    qDebug("Loading settings");
    ifstream file(fileName.c_str());
    if (!file)
        return false;
    string str;
    while(getline(file, str))
    {
        int index = str.find('=');
        if (index > -1)
        {
            string val = str.substr(index+1);
            size_t end = val.find('\r');
            if (end != string::npos)
                val = str.substr(index+1, end);
            Settings::Map[str.substr(0, index)] = val;
        }
    }
    qDebug("%d settings loaded", Settings::Count());
    return true;
}

void Settings::Save()
{
    //qDebug("Saving settings");
    map<string, string>::iterator iter;
    string str;

    for (iter = Settings::Map.begin(); iter != Settings::Map.end(); ++iter)
    {
        str.append(iter->first);
        str.append("=");
        str.append(iter->second);
        str.append("\r\n");
    }

    ofstream file(fileName.c_str());
    file << str;
    file.close();
}

int Settings::Count() { return Settings::Map.size(); }

int Settings::GetInt(string key) { return QString(Settings::Map[key].c_str()).toInt(); }
void Settings::SetInt(string key, int val) { Settings::Map[key] = QString("%1").arg(val).toStdString(); }

double Settings::GetDouble(string key) { return QString(Settings::Map[key].c_str()).toDouble(); }
void Settings::SetDouble(string key, double val) { Settings::Map[key] = QString("%1").arg(val).toStdString(); }

bool Settings::GetBool(string key) { return Settings::Map[key] == "true"; }
void Settings::SetBool(string key, bool val) { Settings::Map[key] = QString("%1").arg(val ? "true" : "false").toStdString(); }

string Settings::GetString(string key) { return Settings::Map[key]; }
void Settings::SetString(string key, string val) { Settings::Map[key] = val; }

QString Settings::GetQString(string key) { return QString::fromStdString(Settings::Map[key]); }
void Settings::SetQString(string key, QString val) { Settings::Map[key] = val.toStdString(); }
