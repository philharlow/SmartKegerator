#ifndef TEMPSENSORMANAGER_H
#define TEMPSENSORMANAGER_H

#include <QApplication>
#include <QProcess>
#include <QTimer>

using namespace std;

class TempSensorManager : public QObject
{
    Q_OBJECT
public:
    static TempSensorManager* Instance;
    static double Temperature;
    static double Humidity;
    static void Init();
    static void Shutdown();

    TempSensorManager();

signals:
    void TempUpdated();

private slots:
    void onTempData();
    void onResetTick();

private:
    void resetSensor();
    int tempSensorPin;
    double lastTempUpdateAt;
    int resetState;
    int resetThreshold;
    QTimer resetTimer;
    QProcess *tempProcess;

    string tempSensorCommand, tempCleanupCommand;
};

#endif // TEMPSENSORMANAGER_H
