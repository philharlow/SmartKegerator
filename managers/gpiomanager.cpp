#include <managers/gpiomanager.h>
#include <data/settings.h>
#include <iostream>
#include <stdio.h>
#include <dirent.h>

using namespace std;

int cameraLedsPin;

void GPIOManager::Init()
{
    cameraLedsPin = Settings::GetInt("cameraLEDsPin");
    GPIOManager::SetMode(cameraLedsPin, "out");

    GPIOManager::EnableCameraLights(false);
}

void GPIOManager::EnableCameraLights(bool state)
{
    GPIOManager::Write(cameraLedsPin, state);
}

void GPIOManager::SetMode(int pin, string mode)
{
    qDebug("GPIO::SetMode %d %s", pin, mode.c_str());

    // load from settings
    string gpioModeStr = Settings::GetString("gpioModeCommand");

    //pinMode(pin, OUTPUT);
    system(QString(gpioModeStr.c_str()).arg(pin).arg(mode.c_str()).toUtf8());
}

void GPIOManager::Write(int pin, bool val)
{
    qDebug("GPIO::Write %d %d", pin, val);

    // load from settings
    string gpioWriteStr = Settings::GetString("gpioWriteCommand");

    //digitalWrite(pin, val ? 255 : 0);
    system(QString(gpioWriteStr.c_str()).arg(pin).arg(val ? 1 : 0).toUtf8());
}
