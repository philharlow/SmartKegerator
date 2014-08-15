#include <windows/mainwindow.h>
#include <ui_mainwindow.h>
#include <data/settings.h>
#include <data/constants.h>
#include <managers/tempsensormanager.h>
#include <QtConcurrentRun>
#include <QCloseEvent>
#include <QDebug>
#include <time.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <app.h>
#include <cmath>

MainWindow* MainWindow::Instance;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tempGraph = new Graph(QString("Temperature"), 760/5.0, QColor(240, 100, 100, 150));
    humGraph = new Graph(QString("Humidity"), 760/5.0, QColor(100, 200, 100, 130));
    this->ui->graphFrame->AddGraph(tempGraph);
    this->ui->graphFrame->AddGraph(humGraph);

    // Set up displays
    leftDisplay = new BeerDisplay(this->ui->leftKegFill, this->ui->leftLogo);
    leftDisplay->nameField = this->ui->leftBeerNameField;
    leftDisplay->companyField = this->ui->leftBeerCompanyField;
    leftDisplay->typeField = this->ui->leftCityField;
    leftDisplay->priceField = this->ui->leftPriceField;
    leftDisplay->abvField = this->ui->leftAbvField;
    leftDisplay->ibuField = this->ui->leftIbuField;
    leftDisplay->boughtField = this->ui->leftBoughtField;
    leftDisplay->emptyField = this->ui->leftEmptyField;

    rightDisplay = new BeerDisplay(this->ui->rightKegFill, this->ui->rightLogo);
    rightDisplay->nameField = this->ui->rightBeerNameField;
    rightDisplay->companyField = this->ui->rightBeerCompanyField;
    rightDisplay->typeField = this->ui->rightCityField;
    rightDisplay->priceField = this->ui->rightPriceField;
    rightDisplay->abvField = this->ui->rightAbvField;
    rightDisplay->ibuField = this->ui->rightIbuField;
    rightDisplay->boughtField = this->ui->rightBoughtField;
    rightDisplay->emptyField = this->ui->rightEmptyField;


    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateUI()));
    updateTimer.start(1000);

    connect(TempSensorManager::Instance, SIGNAL(TempUpdated()), this, SLOT(onTempData()));
}

void MainWindow::onTempData()
{
    ui->tempField->setText(QString::number(ceil(TempSensorManager::Temperature-0.5), 'f', 0));
    ui->humidityField->setText(QString::number(ceil(TempSensorManager::Humidity-0.5), 'f', 0));

    tempGraph->AddReading(TempSensorManager::Temperature);
    humGraph->AddReading(TempSensorManager::Humidity);
}

void MainWindow::on_historyButton_clicked()
{
    HistoryWindow::Instance->ShowWindow();
    this->HideWindow();
}

void MainWindow::on_usersButton_clicked()
{
    UsersWindow::Instance->ShowWindow();
    this->HideWindow();
}

void MainWindow::on_settingsButton_clicked()
{
    if (KeyboardDialog::VerifyPassword() == false)
        return;

    SettingsWindow::ShowWindow();
    this->HideWindow();
}

double nextUpdate = 0;// * 60;
void MainWindow::updateUI()
{
    if (this->hidden)
        return;

    updateTime();
    moveBackground();

    double now = time(NULL);
    if (now > nextUpdate)
    {
        updateBeers();
        nextUpdate = now + 60;
    }
}

int backgroundMoved = 0;
void MainWindow::moveBackground()
{
    bool move = false;
    if (move)
    {
        double now = backgroundMoved++ / 10.0;
        double drift = 20.0;
        this->ui->background->move((sin(now) + 1) * -drift, (cos(now) + 1) * -drift);
    }
    else
        this->ui->graphFrame->update();
}

QString getUptimeString(double uptimed)
{
    int weeks = uptimed / (double)Constants::Week;
    int uptime = fmod(uptimed, Constants::Week);
    int days = (uptime / Constants::Day) % 7;
    int hours = (uptime / Constants::Hour) % 24;
    int mins = (uptime / Constants::Minute) % 60;
    int secs = uptime % Constants::Minute;
    if (uptimed >= Constants::Week)
        return QString("%1w%2d%3h%4m%5s").arg(weeks).arg(days).arg(hours).arg(mins).arg(secs);
    if (uptime >= Constants::Day)
        return QString("%1d%2h%3m%4s").arg(days).arg(hours).arg(mins).arg(secs);
    if (uptime >= Constants::Hour)
        return QString("%1h%2m%3s").arg(hours).arg(mins).arg(secs);
    if (uptime >= Constants::Minute)
        return QString("%1m%2s").arg(mins).arg(secs);
    return QString("%1s").arg(secs);
}

void MainWindow::updateTime()
{
    time_t timeObj;
    time(&timeObj);
    tm *pTime = localtime(&timeObj);

    QString mins = QString(pTime->tm_min < 10 ? "0%1" : "%1").arg(pTime->tm_min);

    double now = time(NULL);
    double uptime = now - App::StartTime;

    ui->uptimeField->setText(QString("%1/%2/%3 %4:%5 - Uptime: %6")
                             .arg(pTime->tm_mon + 1).arg(pTime->tm_mday).arg(pTime->tm_year - 100)
                             .arg((pTime->tm_hour%12) == 0 ? 12 : (pTime->tm_hour%12)).arg(mins)
                             .arg(getUptimeString(uptime)));
}


void MainWindow::updateBeers()
{
    // Update displays
    leftDisplay->update(Keg::LeftKeg);
    rightDisplay->update(Keg::RightKeg);

    ui->leftBeerStatsFrame->SetKeg(Keg::LeftKeg);
    ui->rightBeerStatsFrame->SetKeg(Keg::RightKeg);
}

void MainWindow::on_temperatureButton_clicked()
{
    this->ui->graphFrame->ShowGraph(tempGraph);
}

void MainWindow::on_humidityButton_clicked()
{
    this->ui->graphFrame->ShowGraph(humGraph);
}

void MainWindow::ShowWindow()
{
    updateBeers();
    updateUI();
    this->hidden = false;

    if (App::Fullscreen)
        this->showFullScreen();
    else
    {
        this->move(App::WindowX, App::WindowY);
        if (this->isFullScreen())
            this->showNormal();
        else
            this->show();
    }
}

void MainWindow::HideWindow()
{
    this->hidden = true;
    //if (this->isHidden())
    //    return;
    //this->hide();
    //updateTimer.stop();
}

void MainWindow::onQuit()
{
    qDebug("Quitting!");
    updateTimer.stop();
    App::Instance->Shutdown();
}

void MainWindow::on_closeButton_clicked()
{
    onQuit();
    QCoreApplication::quit();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    onQuit();
    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
}
