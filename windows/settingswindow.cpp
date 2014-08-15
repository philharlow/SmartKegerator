#include "windows/settingswindow.h"
#include "ui_settingswindow.h"
#include "data/settings.h"
#include "data/payment.h"
#include "data/constants.h"
#include "managers/gpiomanager.h"
#include "app.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QThread>
#include <windows/keyboarddialog.h>
#include <windows/optionselectdialog.h>

SettingsWindow* SettingsWindow::Instance;

bool ledState;
int ledPin = 18;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    updateUI();

    GPIOManager::SetMode(ledPin, "out");
}

void SettingsWindow::updateUI()
{
    this->ui->option1Button->setText(QString("Turn LED %1").arg(ledState ? "Off" : "On"));
    this->ui->option2Button->setText(QString(App::Fullscreen ? "Go Windowed" : "Go Fullscreen"));
    this->ui->option3Button->setText(QString(Settings::GetBool("cameraUseColor") ? "Use BW Video" : "Use Color Video"));
    this->ui->option4Button->setText(QString(Settings::GetBool("facialRecEnabled") ? "Recognition: On" : "Recognition: Off"));
}

void SettingsWindow::on_option1Button_clicked()
{
    ledState = !ledState;
    updateUI();
    GPIOManager::Write(ledPin, ledState);
}

void SettingsWindow::on_option2Button_clicked()
{
    App::Fullscreen = !App::Fullscreen;
    updateUI();
    Settings::SetBool("windowFullscreen", App::Fullscreen);
    Settings::Save();

    if (App::Fullscreen)
        this->showFullScreen();
    else
        this->showNormal();
}

void SettingsWindow::on_option3Button_clicked()
{
    bool color = Settings::GetBool("cameraUseColor");
    Settings::SetBool("cameraUseColor", color == false);
    updateUI();
    Settings::Save();
}

void SettingsWindow::on_option4Button_clicked()
{
    bool recognitionEnabled = Settings::GetBool("facialRecEnabled");
    Settings::SetBool("facialRecEnabled", recognitionEnabled == false);
    updateUI();
    Settings::Save();
}

int getSide()
{
    vector<string> choices;
    if (Keg::LeftKeg != NULL)
    {
        Beer* beer = Beer::BeersById[Keg::LeftKeg->BeerId];
        choices.push_back(QString("Left tap\n%1 - %2").arg(beer->Company.c_str()).arg(beer->Name.c_str()).toStdString());
    }
    else
        choices.push_back(QString("Left tap\nEmpty").toStdString());

    if (Keg::RightKeg != NULL)
    {
        Beer* beer = Beer::BeersById[Keg::RightKeg->BeerId];
        choices.push_back(QString("Right tap\n%1 - %2").arg(beer->Company.c_str()).arg(beer->Name.c_str()).toStdString());
    }
    else
        choices.push_back(QString("Right tap\nEmpty").toStdString());

    int choice = OptionSelectDialog::GetChoice("Select side", choices);

    return choice;
}

void SettingsWindow::on_addUserButton_clicked()
{
    string userName = "";
    if (KeyboardDialog::TryGetString(userName, "Enter User Name") == false) return;

    User* user = User::AddUser(userName);

    QMessageBox::information(this, QString("User Added"), QString("User '%1' added!").arg(userName.c_str()), QMessageBox::Ok);
}

void SettingsWindow::on_addKegButton_clicked()
{
    vector<Beer*> beers;
    vector<string> beerStrings;
    map<int, Beer*>::iterator beerIter;
    for (beerIter = Beer::BeersById.begin(); beerIter != Beer::BeersById.end(); ++beerIter)
    {
        Beer* beer = beerIter->second;
        beers.push_back(beer);
        beerStrings.push_back(QString("%1 - %2").arg(beer->Company.c_str()).arg(beer->Name.c_str()).toStdString());
    }
    beerStrings.push_back("Add New Beer");

    Beer* beer;
    int beerChoice;
    if (OptionSelectDialog::TryGetChoice(beerChoice, "Select beer", beerStrings) == false) return;

    if (beerChoice == beerStrings.size()-1)
    {
        string name, company, location, type;
        double abv;
        int ibu;

        if (KeyboardDialog::TryGetString(name, "Enter Beer Name") == false) return;
        if (KeyboardDialog::TryGetString(company, "Enter Brewery Name") == false) return;
        if (KeyboardDialog::TryGetString(location, "Enter Brewery Location") == false) return;
        if (KeyboardDialog::TryGetString(type, "Enter Beer Type") == false) return;
        if (KeyboardDialog::TryGetDouble(abv, "Enter Enter ABV % (proof/2)") == false) return;
        if (KeyboardDialog::TryGetInt(ibu, "Enter Beer IBU") == false) return;

        beer = Beer::AddBeer(name, company, location, type, abv, ibu);

        QMessageBox::information(this, QString("Beer Added"), QString("Beer '%1' added!").arg(name.c_str()), QMessageBox::Ok);
    }
    else
        beer = beers[beerChoice];

    time_t timeObj;
    time(&timeObj);
    tm *pTime = localtime(&timeObj);

    string date = QString("%1/%2/%3").arg(pTime->tm_mon + 1).arg(pTime->tm_mday).arg(pTime->tm_year - 100).toStdString();
    double gallons, price;

    if (KeyboardDialog::TryGetString(date, "Enter Date Bought", date) == false) return;
    if (KeyboardDialog::TryGetDouble(gallons, "Enter Keg Volume (in gallons)", 5) == false) return;

    double previousPrice = 90;
    map<int, Keg*>::reverse_iterator kegIter;
    for (kegIter = Keg::KegsById.rbegin(); kegIter != Keg::KegsById.rend(); ++kegIter)
    {
        Keg* keg = kegIter->second;
        if (keg->BeerId == beer->Id)
        {
            previousPrice = keg->Price;
            break;
        }
    }

    if (KeyboardDialog::TryGetDouble(price, "Enter Keg Price ($)", previousPrice) == false) return;

    Keg* keg = Keg::AddKeg(beer->Id, date, gallons * Constants::LitersPerGallon, price);

    QMessageBox::information(this, QString("Keg Added"), QString("Keg successfully' added!"), QMessageBox::Ok);

    int side = getSide();
    if (side == LEFT_TAP)
        Keg::LeftKeg = keg;
    else if (side == RIGHT_TAP)
        Keg::RightKeg = keg;

    Keg::SaveKegs();
}

void SettingsWindow::on_finishKegButton_clicked()
{
    int choice = getSide();
    bool confirmed = false;

    if (choice == LEFT_TAP && Keg::LeftKeg != NULL)
        confirmed = QMessageBox::question(this, "Finish Keg", "Are you sure you want to finish the left keg?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
    else if (choice == RIGHT_TAP && Keg::RightKeg != NULL)
        confirmed = QMessageBox::question(this, "Finish Keg", "Are you sure you want to finish the right keg?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;

    if (confirmed)
    {
        if (choice == LEFT_TAP)
            Keg::LeftKeg = NULL;
        else
            Keg::RightKeg = NULL;

        Keg::SaveKegs();

        QMessageBox::information(this, QString("Keg Finished"), QString("%1 keg marked finished!").arg(choice == LEFT_TAP ? "Left" : "Right"), QMessageBox::Ok);
    }
}


void SettingsWindow::on_editBeerButton_clicked()
{
    vector<Beer*> beers;
    vector<string> beerStrings;
    map<int, Beer*>::iterator beerIter;
    for (beerIter = Beer::BeersById.begin(); beerIter != Beer::BeersById.end(); ++beerIter)
    {
        Beer* beer = beerIter->second;
        beers.push_back(beer);
        beerStrings.push_back(QString("%1 - %2").arg(beer->Company.c_str()).arg(beer->Name.c_str()).toStdString());
    }

    int beerChoice;
    if (OptionSelectDialog::TryGetChoice(beerChoice, "Select beer", beerStrings) == false) return;

    Beer* beer = beers[beerChoice];

    string name, company, location, type;
    double abv;
    int ibu;

    if (KeyboardDialog::TryGetString(name, "Enter Beer Name", beer->Name) == false) return;
    if (KeyboardDialog::TryGetString(company, "Enter Brewery Name", beer->Company) == false) return;
    if (KeyboardDialog::TryGetString(location, "Enter Brewery Location", beer->Location) == false) return;
    if (KeyboardDialog::TryGetString(type, "Enter Beer Type", beer->Type) == false) return;
    if (KeyboardDialog::TryGetDouble(abv, "Enter ABV % (proof/2)", beer->ABV) == false) return;
    if (KeyboardDialog::TryGetInt(ibu, "Enter IBU", beer->IBU) == false) return;

    beer->Name = name;
    beer->Company = company;
    beer->Location = location;
    beer->Type = type;
    beer->ABV = abv;
    beer->IBU = ibu;

    Beer::SaveBeers();

    QMessageBox::information(this, QString("Beer editted"), QString("%1 saved!").arg(name.c_str()), QMessageBox::Ok);
}

void SettingsWindow::on_editKegButton_clicked()
{
    vector<Keg*> kegs;
    vector<string> kegStrings;
    map<int, Keg*>::iterator kegIter;
    for (kegIter = Keg::KegsById.begin(); kegIter != Keg::KegsById.end(); ++kegIter)
    {
        Keg* keg = kegIter->second;
        Beer *beer = Beer::BeersById[keg->BeerId];
        kegs.push_back(keg);
        string price = QString::number(keg->Price, 'f', 2).toStdString();
        kegStrings.push_back(QString("Bought: %1 - $%2\n%3 - %4").arg(keg->DateBought.c_str()).arg(price.c_str()).arg(beer->Company.c_str()).arg(beer->Name.c_str()).toStdString());
    }

    int kegChoice;
    if (OptionSelectDialog::TryGetChoice(kegChoice, "Select keg", kegStrings) == false) return;

    Keg* keg = kegs[kegChoice];

    string bought;
    double gallons, price;

    if (KeyboardDialog::TryGetString(bought, "Enter Date Bought", keg->DateBought) == false) return;
    if (KeyboardDialog::TryGetDouble(gallons, "Enter gallons", keg->LitersCapacity * 1.f/Constants::LitersPerGallon) == false) return;
    if (KeyboardDialog::TryGetDouble(price, "Enter price", keg->Price) == false) return;

    keg->DateBought = bought;
    keg->LitersCapacity = gallons * Constants::LitersPerGallon;
    keg->Price = price;

    keg->ComputeValues();

    int currentBeerIndex = -1;
    int i = 0;
    vector<Beer*> beers;
    vector<string> beerStrings;
    map<int, Beer*>::iterator beerIter;
    for (beerIter = Beer::BeersById.begin(); beerIter != Beer::BeersById.end(); ++beerIter)
    {
        Beer* beer = beerIter->second;
        if (beer->Id == keg->BeerId)
            currentBeerIndex = i;
        beers.push_back(beer);
        beerStrings.push_back(QString("%1 - %2").arg(beer->Company.c_str()).arg(beer->Name.c_str()).toStdString());
        i++;
    }

    int beerChoice;
    if (OptionSelectDialog::TryGetChoice(beerChoice, "Select beer", beerStrings, currentBeerIndex) == false) return;

    keg->BeerId = beers[beerChoice]->Id;

    int side = getSide();
    if (side == LEFT_TAP)
        Keg::LeftKeg = keg;
    else if (side == RIGHT_TAP)
        Keg::RightKeg = keg;

    Keg::SaveKegs();

    QMessageBox::information(this, QString("Keg Edited"), QString("Keg saved!"), QMessageBox::Ok);
}

void SettingsWindow::on_setTimeButton_clicked()
{
    time_t timeObj;
    time(&timeObj);
    tm *pTime = localtime(&timeObj);

    QString mins = QString(pTime->tm_min < 10 ? "0%1" : "%1").arg(pTime->tm_min);

    string dateTime = QString("%1/%2/%3 %4:%5").arg(pTime->tm_mon + 1).arg(pTime->tm_mday).arg(pTime->tm_year - 100).arg(pTime->tm_hour).arg(mins).toStdString();

    if (KeyboardDialog::TryGetString(dateTime, "Set Date/Time", dateTime, NUMBER_DATETIME_INPUT, true) == false) return;

    qDebug("setting time: %s", dateTime.c_str());

    string command = Settings::GetString("setDateTimeCommand");

    system(QString(command.c_str()).arg(dateTime.c_str()).toUtf8());

    QMessageBox::information(this, QString("Time Updated"), QString("Time Updated!"), QMessageBox::Ok);
}

void SettingsWindow::on_addPaymentButton_clicked()
{
    vector<User*> users;
    vector<string> userStrings;
    map<int, User*>::iterator iter = User::UsersById.begin();
    for (iter++; iter != User::UsersById.end(); ++iter)
    {
        User* user = iter->second;
        users.push_back(user);
        userStrings.push_back(user->Name);
    }

    int userChoice;
    if (OptionSelectDialog::TryGetChoice(userChoice, "Select user", userStrings) == false) return;

    User* user = users[userChoice];
    double amount;

    if (KeyboardDialog::TryGetDouble(amount, "Enter amount") == false) return;

    Payment::AddPayment(user->Id, amount);

    QMessageBox::information(this, QString("Payment Added"), QString("Payment Added!"), QMessageBox::Ok);
}

void SettingsWindow::on_closeButton_clicked()
{
    this->HideWindow();
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    HideWindow();
    event->ignore();
}

void SettingsWindow::ShowWindow()
{
    if (SettingsWindow::Instance == NULL)
        SettingsWindow::Instance = new SettingsWindow();

    if (SettingsWindow::Instance->isHidden() == false)
        return;

    if (App::Fullscreen)
        SettingsWindow::Instance->showFullScreen();
    else
    {
        SettingsWindow::Instance->move(App::WindowX, App::WindowY);
        SettingsWindow::Instance->show();
    }
}

void SettingsWindow::HideWindow()
{
    MainWindow::Instance->ShowWindow();
    if (this->isHidden())
        return;
    this->hide();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
