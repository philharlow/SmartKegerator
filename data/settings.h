#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <string>
#include <map>

using namespace std;

class Settings
{

public:
    static bool Load(string fileName);
    static void Save();

    static int Count();

    static int GetInt(string key);
    static bool GetBool(string key);
    static double GetDouble(string key);
    static string GetString(string key);
    static QString GetQString(string key);

    static void SetInt(string key, int val);
    static void SetBool(string key, bool val);
    static void SetDouble(string key, double val);
    static void SetString(string key, string val);
    static void SetQString(string key, QString val);

private:
    static map<string, string> Map;

};

#endif // SETTINGS_H
