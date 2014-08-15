#include <QApplication>
#include <raspicvcam.h>
#include <data/settings.h>
#include <data/keg.h>
#include <data/beer.h>
#include <data/user.h>
#include <data/pour.h>
#include <data/payment.h>
#include <managers/facialrecognitionmanager.h>
#include <managers/tempsensormanager.h>
#include <managers/flowmetermanager.h>
#include <managers/gpiomanager.h>
#include <windows/mainwindow.h>
#include <windows/pouringwindow.h>
#include <windows/keyboarddialog.h>
#include <windows/historywindow.h>
#include <windows/optionselectdialog.h>
#include <windows/settingswindow.h>
#include <windows/userswindow.h>

using namespace std;

class App : public QObject
{
    Q_OBJECT
public:
    App();
    static App* Instance;

    static bool Fullscreen;
    static int WindowX, WindowY;
    static double StartTime;

    void Shutdown();
};
