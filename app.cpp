#include <app.h>
#include <QtConcurrentRun>

App* App::Instance;

bool App::Fullscreen;
int App::WindowX, App::WindowY;
double App::StartTime;


App::App() : QObject()
{
    qDebug("Starting app");

    // Set globals
    Instance = this;
    StartTime = time(NULL);
    Fullscreen = Settings::GetBool("windowFullscreen");
    WindowX = Settings::GetInt("windowX");
    WindowY = Settings::GetInt("windowY");

    // Load data
    Beer::LoadBeers();
    Keg::LoadKegs();
    User::LoadUsers();
    Pour::LoadPours();
    Payment::LoadPayments();

    // Start managers
    QtConcurrent::run(RaspiCvCam::Init);
    QtConcurrent::run(FacialRecognitionManager::Init);
    GPIOManager::Init();
    TempSensorManager::Init();
    FlowMeterManager::Init();

    // Create windows
    MainWindow::Instance = new MainWindow();
    PouringWindow::Instance = new PouringWindow();
    qDebug("Windows created");

    // Show main window
    MainWindow::Instance->ShowWindow();
}

void App::Shutdown()
{
    TempSensorManager::Shutdown();
    FlowMeterManager::Shutdown();

    RaspiCvCam::Destroy();
    qDebug("App shut down!");
}
