#include <data/keg.h>
#include <app.h>
#include <data/settings.h>
#include <data/constants.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <time.h>
#include <map>
#include <QApplication>

using namespace std;


// Statics
map<int, Keg*> Keg::KegsById;
Keg* Keg::LeftKeg;
Keg* Keg::RightKeg;
int Keg::NextId;

void Keg::LoadKegs()
{
    string filename = Settings::GetString("dataKegsFile");
    vector<StringMap> maps = DataObject::GetStringMaps(filename);

    vector<StringMap>::size_type i;
    for (i=1; i != maps.size(); i++)
    {
        Keg* keg = new Keg(maps[i]);
        Keg::KegsById[keg->Id] = keg;
    }

    if (maps.size() > 0)
    {
        int leftId = -1;
        int rightId = -1;
        if (maps[0].find("LeftKegId") != maps[0].end())
            leftId = QString(maps[0]["LeftKegId"].c_str()).toInt();
        if (maps[0].find("RightKegId") != maps[0].end())
            rightId = QString(maps[0]["RightKegId"].c_str()).toInt();
        Keg::LeftKeg = leftId == -1 ? NULL : Keg::KegsById[leftId];
        Keg::RightKeg = rightId == -1 ? NULL : Keg::KegsById[rightId];
    }
}

void Keg::SaveKegs()
{
    string filename = Settings::GetString("dataKegsFile");
    string str;

    str.append("{\r\n");
    str.append(QString("    LeftKegId:%1\r\n").arg(Keg::LeftKeg == NULL ? -1 : Keg::LeftKeg->Id).toStdString());
    str.append(QString("    RightKegId:%1\r\n").arg(Keg::RightKeg == NULL ? -1 : Keg::RightKeg->Id).toStdString());
    str.append("}\r\n");

    map<int, Keg*>::iterator iter;
    for (iter = Keg::KegsById.begin(); iter != Keg::KegsById.end(); ++iter)
    {
        str.append(iter->second->ToString());
    }

    ofstream file(filename.c_str());
    file << str;
    file.close();
}


Keg* Keg::AddKeg(int beerId, string dateBought, float liters, float price)
{
    Keg* keg = new Keg();
    keg->BeerId = beerId;
    keg->DateBought = dateBought;
    keg->LitersCapacity = liters;
    keg->Price = price;
    keg->WarmestTemp = 0;
    keg->LitersPoured = 0;

    keg->ComputeValues();

    Keg::KegsById[keg->Id] = keg;

    Keg::SaveKegs();

    return keg;
}

// Data Object
Keg::Keg() : DataObject()
{
    Id = NextId++;
    BeerId = -1;
    DateBought = "";
    DateBoughtInt = -1;
    LitersCapacity = -1;
    Price = -1;
    WarmestTemp = -1;
    LitersPoured = -1;
    PricePerPint = -1;
}

Keg::Keg(StringMap data) : DataObject(data)
{
    Id = GetInt("Id");
    NextId = max(NextId, Id+1);

    BeerId = GetInt("BeerId");
    DateBought = GetString("DateBought");
    LitersCapacity = GetDouble("LitersCapacity");
    Price = GetDouble("Price");
    WarmestTemp = GetDouble("WarmestTemp");
    LitersPoured = 0;

    ComputeValues();
}

string Keg::ToString()
{
    SetInt("Id", Id);
    SetInt("BeerId", BeerId);
    SetString("DateBought", DateBought);
    SetDouble("LitersCapacity", LitersCapacity);
    SetDouble("Price", Price);
    SetDouble("WarmestTemp", WarmestTemp);

    return DataObject::ToString();
}

string Keg::GetEmptyDate()
{
    if (LitersPoured == 0)
        return "--";

    long int now = time(NULL);
    int days = (now - DateBoughtInt) / Constants::Day;
    if (days < 1)
        return "--";

    double litersPerDay = LitersPoured / (double)days;
    double daysUntilEmpty = (LitersCapacity - LitersPoured) / litersPerDay;

    time_t emptyDateObj = DateBoughtInt + daysUntilEmpty * Constants::Day;
    tm* emptyDate = localtime(&emptyDateObj);

    return QString("%1/%2/%3").arg(emptyDate->tm_mon+1).arg(emptyDate->tm_mday).arg(emptyDate->tm_year - 100).toStdString();
}

double Keg::GetPrice(double liters)
{
    return liters * (Price / LitersCapacity);
}

void Keg::ComputeValues()
{
    tm timem;
    strptime(DateBought.c_str(), "%m/%d/%y", &timem);
    timem.tm_hour = timem.tm_min = timem.tm_sec = 0;
    DateBoughtInt = mktime(&timem);
    PricePerPint = Price / LitersCapacity / Constants::PintsPerLiter;
}
