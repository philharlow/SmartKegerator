#include <managers/tempsensormanager.h>
#include <managers/gpiomanager.h>
#include <data/settings.h>
#include <app.h>
#include <time.h>


// Statics
double TempSensorManager::Temperature = -1;
double TempSensorManager::Humidity = -1;
TempSensorManager* TempSensorManager::Instance;

void TempSensorManager::Init()
{
    TempSensorManager::Instance = new TempSensorManager();
}

void TempSensorManager::Shutdown()
{
    TempSensorManager::Instance->tempProcess->kill();
}


// Instance
TempSensorManager::TempSensorManager()
{
    resetState = -1;
    resetThreshold = 30;
    connect(&resetTimer, SIGNAL(timeout()), this, SLOT(onResetTick()));

    // Init the temp sensor
    tempSensorPin = Settings::GetInt("tempSensorPowerPin");
    GPIOManager::SetMode(tempSensorPin, "out");
    GPIOManager::Write(tempSensorPin, true);

    tempCleanupCommand = Settings::GetString("tempSensorCleanupCommand");
    tempSensorCommand = Settings::GetString("tempSensorWFICommand");
    tempProcess = new QProcess();
    connect(tempProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onTempData()));
    tempProcess->start(QString(tempSensorCommand.c_str()));
    lastTempUpdateAt = time(NULL);

    Temperature = -1;
    Humidity = -1;

    qDebug("Temp thread created");
}

void TempSensorManager::onTempData()
{
    string output = QString(tempProcess->readAllStandardOutput()).toStdString();

    // No data
    if (output.find("Temp =") == string::npos)
    {
        double now = time(NULL);
        double timeSince = now - lastTempUpdateAt;

        if (timeSince > resetThreshold)
            resetSensor();

        return;
    }

    int index = output.find("Temp =") + 6;
    QString qstr(output.substr(index, output.find(" *C") - index).c_str());
    double temp = qstr.toDouble();
    temp = ((9.0/5.0) * temp) + 32;
    // filter bad data
    if (Temperature == -1 || abs(int(temp - Temperature)) < 10)
        Temperature = temp;

    index = output.find("Hum =") + 5;
    qstr = QString(output.substr(index, output.find(" %") - index).c_str());
    double hum = qstr.toDouble();
    if (Humidity == -1 || abs(int(hum - Humidity)) < 10)
        Humidity = hum;

    lastTempUpdateAt = time(NULL);

    emit TempUpdated();
}

void TempSensorManager::resetSensor()
{
    resetState = 0;

    // Kill the process
    system(tempCleanupCommand.c_str());
    tempProcess->close();

    // Turn off the dht
    GPIOManager::Write(tempSensorPin, false);
    qDebug("Turned off the dht!");

    resetTimer.start(1000);
}


void TempSensorManager::onResetTick()
{
    if (resetState == 0)
    {
        // Turn on the dht
        GPIOManager::Write(tempSensorPin, true);
        qDebug("Turned on the dht!");
        resetState++;
    }
    else
    {
        // Start the python script
        lastTempUpdateAt = time(NULL);
        tempProcess->start(QString(tempSensorCommand.c_str()));
        qDebug("Restarted temp process!");
        resetState = -1;
        resetTimer.stop();
    }
}
