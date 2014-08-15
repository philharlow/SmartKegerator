#ifndef PAYMENT_H
#define PAYMENT_H

#include <time.h>
#include <data/dataobject.h>

using namespace std;

class Payment : DataObject
{
public:
    static map<int, Payment*> PaymentsById;
    static map<int, vector<Payment*> > PaymentsByUserId;

    static Payment* AddPayment(int userId, float amount);

    static void LoadPayments();
    static void SavePayments();

    Payment();
    Payment(StringMap);

    string ToString();


    static int NextId;

    int Id;
    int UserId;
    double Time;
    double Amount;
};

#endif // PAYMENT_H
