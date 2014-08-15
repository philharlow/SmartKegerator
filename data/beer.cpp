#include <data/beer.h>
#include <app.h>
#include <data/settings.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <QApplication>

using namespace std;


// Statics

map<int, Beer*> Beer::BeersById;
int Beer::NextId;

void Beer::LoadBeers()
{
    string filename = Settings::GetString("dataBeersFile");
    vector<StringMap> maps = DataObject::GetStringMaps(filename);

    vector<StringMap>::size_type i;
    for (i=0; i != maps.size(); i++)
    {
        Beer* beer = new Beer(maps[i]);
        Beer::BeersById[beer->Id] = beer;
    }
}

void Beer::SaveBeers()
{
    string filename = Settings::GetString("dataBeersFile");
    string str;

    map<int, Beer*>::iterator iter;
    for (iter = Beer::BeersById.begin(); iter != Beer::BeersById.end(); ++iter)
    {
        str.append(iter->second->ToString());
    }

    ofstream file(filename.c_str());
    file << str;
    file.close();
}


Beer* Beer::AddBeer(string name, string company, string location, string type, float abv, int ibu)
{
    Beer* beer = new Beer();

    beer->Name = name;
    beer->Company = company;
    beer->Location = location;
    beer->Type = type;
    beer->ABV = abv;
    beer->IBU = ibu;

    Beer::BeersById[beer->Id] = beer;

    Beer::SaveBeers();

    return beer;
}


// Data Object

Beer::Beer() : DataObject()
{
    Id = NextId++;
    Name = "";
    Company = "";
    Location = "";
    Type = "";
    ABV = -1;
    IBU = -1;
    LogoPath = "";
}

Beer::Beer(StringMap data) : DataObject(data)
{
    Id = GetInt("Id");
    NextId = max(NextId, Id+1);

    Name = GetString("Name");
    Company = GetString("Company");
    Location = GetString("Location");
    Type = GetString("Type");
    ABV = GetDouble("ABV");
    IBU = GetInt("IBU");
    LogoPath = GetString("LogoPath");
}

string Beer::ToString()
{
    SetInt("Id", Id);
    SetString("Name", Name);
    SetString("Company", Company);
    SetString("Location", Location);
    SetString("Type", Type);
    SetDouble("ABV", ABV);
    SetInt("IBU", IBU);
    SetString("LogoPath", LogoPath);

    return DataObject::ToString();
}
