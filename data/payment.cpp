#include <data/payment.h>
#include <app.h>
#include <data/settings.h>
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
map<int, Payment*> Payment::PaymentsById;
map<int, vector<Payment*> > Payment::PaymentsByUserId;
int Payment::NextId;

void Payment::LoadPayments()
{
    string filename = Settings::GetString("dataPaymentsFile");
    vector<StringMap> maps = DataObject::GetStringMaps(filename);

    vector<StringMap>::size_type i;
    for (i=0; i != maps.size(); i++)
    {
        Payment* payment = new Payment(maps[i]);
        Payment::PaymentsById[payment->Id] = payment;
        Payment::PaymentsByUserId[payment->UserId].push_back(payment);
    }
}

void Payment::SavePayments()
{
    string filename = Settings::GetString("dataPaymentsFile");
    string str;

    map<int, Payment*>::iterator iter;
    for (iter = Payment::PaymentsById.begin(); iter != Payment::PaymentsById.end(); ++iter)
    {
        str.append(iter->second->ToString());
    }

    ofstream file(filename.c_str());
    file << str;
    file.close();
}


Payment* Payment::AddPayment(int userId, float amount)
{
    Payment* payment = new Payment();
    payment->UserId = userId;
    payment->Amount = amount;

    Payment::PaymentsById[payment->Id] = payment;
    Payment::PaymentsByUserId[payment->UserId].push_back(payment);

    Payment::SavePayments();

    return payment;
}

// Data Object
Payment::Payment() : DataObject()
{
    Id = NextId++;
    Time = time(NULL);
    UserId = -1;
    Amount = -1;
}

Payment::Payment(StringMap data) : DataObject(data)
{
    Id = GetInt("Id");
    NextId = max(NextId, Id+1);
    UserId = GetInt("UserId");
    Time = GetDouble("Time");
    Amount = GetDouble("Amount");
}

string Payment::ToString()
{
    SetInt("Id", Id);
    SetInt("UserId", UserId);
    SetDouble("Time", Time);
    SetDouble("Amount", Amount);

    return DataObject::ToString();
}
