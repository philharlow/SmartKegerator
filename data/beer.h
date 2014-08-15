#ifndef BEER_H
#define BEER_H

#include <string>
#include <data/dataobject.h>

using namespace std;

class Beer : DataObject
{
public:
    static map<int, Beer*> BeersById;

    static Beer* AddBeer(string name, string company, string location, string type, float abv, int ibu);

    static void LoadBeers();
    static void SaveBeers();

    Beer();
    Beer(StringMap);

    string ToString();

    static int NextId;

    int Id;
    string Name;
    string Company;
    string Type;
    string Location;
    string LogoPath;
    double ABV;
    int IBU;
};

#endif // BEER_H
