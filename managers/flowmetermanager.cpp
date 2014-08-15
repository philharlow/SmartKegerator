#include <managers/flowmetermanager.h>
#include <data/settings.h>


int tickThreshold = 3;
int endPourSeconds = 5;

// Statics
FlowMeterManager* FlowMeterManager::Instance;

Keg* FlowMeterManager::CurrentKeg;
BeerTapSide FlowMeterManager::CurrentTapSide;
double FlowMeterManager::LastTickAt;
int FlowMeterManager::Ticks;
bool FlowMeterManager::IsPouring;
bool FlowMeterManager::ConflictingTicks;

void FlowMeterManager::Init()
{
    FlowMeterManager::Instance = new FlowMeterManager();
}

void FlowMeterManager::Shutdown()
{
    FlowMeterManager::Instance->gpioProc->kill();
}


// Instance
FlowMeterManager::FlowMeterManager()
{
    gpioWFICommand = Settings::GetString("flowMeterWFICommand");
    leftTickString = QString("Left");
    rightTickString = QString("Right");

    gpioProc = new QProcess();
    connect(gpioProc, SIGNAL(readyReadStandardOutput()), this, SLOT(onFlowMeterData()));
    gpioProc->start(QString(gpioWFICommand.c_str()));

    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTick()));

    qDebug("Flow meter thread created");
}

void FlowMeterManager::onUpdateTick()
{
    double now = time(NULL);
    if (now - LastTickAt >= endPourSeconds)
        FinishPour();
}

void FlowMeterManager::onFlowMeterData()
{
    QString out = QString(gpioProc->readAllStandardOutput());

    double now = time(NULL);
    // If it's been a couple seconds since the last tick, and we're not pouring, reset ticks
    if (IsPouring == false && now > LastTickAt)
        Ticks = 0;

    // Count the tick
    Ticks++;
    LastTickAt = now;

    // If we're still under the threshold, bail
    if (Ticks < tickThreshold)
        return;

    BeerTapSide tapSide = RIGHT_TAP;
    if (out.startsWith(leftTickString))
        tapSide = LEFT_TAP;

    // If we just crossed the threshold, lets start the pour
    if (Ticks == tickThreshold)
    {
        beginPour(tapSide);
        return;
    }

    // Check for the other beer
    if (tapSide != CurrentTapSide)
        ConflictingTicks = true;

    emit FlowMeterTicked();
}

void FlowMeterManager::beginPour(BeerTapSide tapSide)
{
    CurrentTapSide = tapSide;
    CurrentKeg = CurrentTapSide == RIGHT_TAP ? Keg::RightKeg : Keg::LeftKeg;

    if (CurrentKeg == NULL)
    {
        Ticks = 0;
        return;
    }

    IsPouring = true;
    ConflictingTicks = false;

    updateTimer.start(1000);

    emit PourStarted();
}

void FlowMeterManager::FinishPour()
{
    IsPouring = false;
    updateTimer.stop();

    emit PourFinished();
}
