#include <QApplication>
#include <QDesktopWidget>
#include <app.h>
#include <data/settings.h>

int main(int argc, char *argv[])
{
    string configPath;
    if (argc == 2)
        configPath = argv[1];
    else
    {
       configPath = "/home/pi/qt/SmartKegerator/config.txt";
       qDebug("usage: SmartKegerator config.txt. Using %s", configPath.c_str());
    }

    QApplication a(argc, argv);

    if (Settings::Load(configPath) == false)
    {
        qDebug("Could not load file: %s", configPath.c_str());
        return 1;
    }

    QDesktopWidget screen;
    int w = screen.availableGeometry(screen.primaryScreen()).width();
    int h = screen.availableGeometry(screen.primaryScreen()).height();
    bool isTouchscreen = false;

    // TODO: check for touchscreen
    if (w == 800 && h == 480 - 26)
    {
        isTouchscreen = true;
        a.setOverrideCursor(QCursor(Qt::BlankCursor));
        qDebug("Touchscreen detected. Hiding cursor and setting fullscreen");
    }

    Settings::SetBool("windowFullscreen", isTouchscreen);

    new App();
    
    return a.exec();
}
