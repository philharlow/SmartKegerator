#ifndef KEG_H
#define KEG_H

#include <string>
#include <time.h>
#include <data/dataobject.h>

using namespace std;

class Keg : DataObject
{
public:
    static map<int, Keg*> KegsById;

    static Keg* LeftKeg;
    static Keg* RightKeg;

    static Keg* AddKeg(int beerId, string dateBought, float liters, float price);

    static void LoadKegs();
    static void SaveKegs();

    Keg();
    Keg(StringMap);

    void ParseStringMap(StringMap data);
    string ToString();

    double GetPrice(double liters);
    string GetEmptyDate();

    void ComputeValues();

    static int NextId;

    int Id;
    int BeerId;
    string DateBought;

    double LitersCapacity;
    double LitersPoured;
    double Price;
    double WarmestTemp;

    // Computed
    long int DateBoughtInt;
    float PricePerPint;

};

#endif // KEG_H
