#ifndef POUR_H
#define POUR_H

#include <data/dataobject.h>
#include <data/beer.h>
#include <data/user.h>
#include <data/keg.h>
#include <string>
#include <map>

using namespace std;

class Pour : DataObject
{
    typedef vector<Pour*> PourVector;

public:
    static void LoadPours();
    static void SavePours();

    static PourVector AllPours;
    static map<int, PourVector> PoursByKegId;

    static void AddPour(Pour*, bool save = true);
    static void RemovePour(Pour*);

    static vector<Pour*> GetPoursInLast(int time);
    static vector<Pour*> GetPoursInLast(int time, User *user);
    static vector<Pour*> GetPoursInLast(int time, Beer *beer);
    static vector<Pour*> GetPoursByUserId(int userId);

    static vector<int> GetPoursByDow(Keg* keg = NULL);
    static map<User*, int> GetPoursByUser(Keg* keg = NULL);

    Pour();
    Pour(StringMap);

    string ToString();

    static int NextId;

    int Id;
    double Time;
    int KegId;
    int UserId;
    double Ticks;
    double Ounces;
    double PriceModifier;
    vector<string> ImagePaths;

    // Computed
    string TimeString;
    double Price;

};

#endif // POUR_H
