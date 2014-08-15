#include <data/pour.h>
#include <app.h>
#include <data/settings.h>
#include <data/constants.h>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <map>
#include <time.h>
#include <QApplication>

using namespace std;

QString amQstring = QString("am");
QString pmQstring = QString("pm");

// Statics
int Pour::NextId;
Pour::PourVector Pour::AllPours;
map<int, Pour::PourVector> Pour::PoursByKegId;

void Pour::LoadPours()
{
    string filename = Settings::GetString("dataPoursFile");
    vector<StringMap> maps = DataObject::GetStringMaps(filename);

    vector<StringMap>::size_type i;
    for (i=0; i != maps.size(); i++)
    {
        Pour* pour = new Pour(maps[i]);
        AddPour(pour, false);
    }

    qDebug("%d pours loaded!", Pour::AllPours.size());
}

void Pour::SavePours()
{
    string filename = Settings::GetString("dataPoursFile");
    string str;

    vector<Pour*>::iterator iter;
    for(iter = Pour::AllPours.begin(); iter != Pour::AllPours.end(); iter++)
    {
        str.append((*iter)->ToString());
    }

    ofstream file(filename.c_str());
    file << str;
    file.close();
}

void Pour::AddPour(Pour* pour, bool save)
{
    Pour::AllPours.push_back(pour);
    Pour::PoursByKegId[pour->KegId].push_back(pour);


    Keg* keg = Keg::KegsById[pour->KegId];
    double liters = pour->Ounces / Constants::OuncesPerLiter;
    keg->LitersPoured += liters;

    // Generate price
    pour->Price = keg->GetPrice(liters) * pour->PriceModifier;

    // Generate time
    time_t timeObj = pour->Time;
    tm *pTime = localtime(&timeObj);

    QString mins = QString(pTime->tm_min < 10 ? "0%1" : "%1").arg(pTime->tm_min);

    pour->TimeString = QString("%1/%2/%3 %4:%5%6")
            .arg(pTime->tm_mon + 1)
            .arg(pTime->tm_mday)
            .arg(pTime->tm_year +  1900)
            .arg((pTime->tm_hour%12) == 0 ? 12 : (pTime->tm_hour%12))
            .arg(mins)
            .arg(pTime->tm_hour < 12 ? amQstring : pmQstring)
            .toStdString();

    if (save)
        Pour::SavePours();
}


vector<Pour*> Pour::GetPoursByUserId(int userId)
{
    vector<Pour*> pours;
    vector<Pour*>::iterator iter;
    for(iter = Pour::AllPours.begin(); iter != Pour::AllPours.end(); iter++)
    {
        Pour* pour = (*iter);
        if (pour->UserId == userId)
            pours.push_back(pour);
    }
    return pours;
}

vector<Pour*> Pour::GetPoursInLast(int threshold)
{
    vector<Pour*> pours;
    vector<Pour*>::reverse_iterator iter;
    double now = time(NULL);
    for(iter = Pour::AllPours.rbegin(); iter != Pour::AllPours.rend(); iter++)
    {
        Pour* pour = (*iter);
        if (now - pour->Time < threshold)
            pours.push_back(pour);
        else
            break;
    }
    return pours;
}

vector<Pour*> Pour::GetPoursInLast(int threshold, User* user)
{
    vector<Pour*> pours;
    vector<Pour*>::reverse_iterator iter;
    double now = time(NULL);
    for(iter = Pour::AllPours.rbegin(); iter != Pour::AllPours.rend(); iter++)
    {
        Pour* pour = (*iter);
        if (now - pour->Time < threshold)
        {
            if (pour->UserId == user->Id)
                pours.push_back(pour);
        }
        else
            break;
    }
    return pours;
}

vector<Pour*> Pour::GetPoursInLast(int threshold, Beer* beer)
{
    vector<Pour*> pours;
    vector<Pour*>::reverse_iterator iter;
    double now = time(NULL);
    for(iter = Pour::AllPours.rbegin(); iter != Pour::AllPours.rend(); iter++)
    {
        Pour* pour = (*iter);
        if (now - pour->Time < threshold)
        {
            Keg* keg = Keg::KegsById[pour->KegId];
            if (keg->BeerId == beer->Id)
                pours.push_back(pour);
        }
        else
            break;
    }
    return pours;
}

vector<int> Pour::GetPoursByDow(Keg* keg)
{
    vector<int> days;
    for (int i=0; i<7; i++)
        days.push_back(0);

    vector<Pour*>::reverse_iterator iter;
    for(iter = Pour::AllPours.rbegin(); iter != Pour::AllPours.rend(); iter++)
    {
        Pour* pour = (*iter);
        if (keg != NULL && pour->KegId != keg->Id)
            continue;

        time_t timeObj = pour->Time;
        tm *pourTime = localtime(&timeObj);
        days[pourTime->tm_wday]++;
    }
    return days;
}


map<User*, int> Pour::GetPoursByUser(Keg* keg)
{
    map<User*, int> pourCounts;

    vector<Pour*>::reverse_iterator iter;
    for(iter = Pour::AllPours.rbegin(); iter != Pour::AllPours.rend(); iter++)
    {
        Pour* pour = (*iter);
        if (keg != NULL && pour->KegId != keg->Id)
            continue;

        User* user = User::UsersById[pour->UserId];
        pourCounts[user]++;
    }
    return pourCounts;
}


void Pour::RemovePour(Pour* pour)
{
    Pour::AllPours.erase(std::remove(Pour::AllPours.begin(), Pour::AllPours.end(), pour), Pour::AllPours.end());
    Pour::PoursByKegId[pour->KegId].erase(std::remove(Pour::PoursByKegId[pour->KegId].begin(), Pour::PoursByKegId[pour->KegId].end(), pour), Pour::PoursByKegId[pour->KegId].end());
    Pour::SavePours();
}



// Data Object
Pour::Pour() : DataObject()
{
    Id = NextId++;
    Time = time(NULL);

    KegId = -1;
    UserId = -1;
    Ticks = -1;
    Ounces = -1;
    PriceModifier = 1;
}

Pour::Pour(StringMap data) : DataObject(data)
{
    Id = GetInt("Id");
    NextId = max(NextId, Id+1);

    Time = GetDouble("Time");
    KegId = GetInt("KegId");
    UserId = GetInt("UserId");
    Ticks = GetInt("Ticks");
    Ounces = GetDouble("Ounces");
    ImagePaths = GetStringVector("ImagePaths");
    PriceModifier = GetDouble("PriceModifier");
}

string Pour::ToString()
{
    SetInt("Id", Id);
    SetDouble("Time", Time);
    SetInt("KegId", KegId);
    SetInt("UserId", UserId);
    SetInt("Ticks", Ticks);
    SetDouble("Ounces", Ounces);
    SetStringVector("ImagePaths", ImagePaths);
    SetDouble("PriceModifier", PriceModifier);

    return DataObject::ToString();
}

