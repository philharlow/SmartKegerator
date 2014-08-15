#ifndef POURINGWINDOW_H
#define POURINGWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <data/keg.h>
#include <data/user.h>
#include <data/pour.h>
#include <managers/flowmetermanager.h>

using namespace std;

namespace Ui {
class PouringWindow;
}

class PouringWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit PouringWindow(QWidget *parent = 0);
    ~PouringWindow();

    static PouringWindow* Instance;

    void ShowWindow();
    void HideWindow();

    void updateCamImage();

public slots:
    void onPourStarted();
    void onFlowMeterTick();
    void onPourFinished();

signals:
    void pourStartedSignal();
    void pourFinishedSignal();
    void updateCamImageSignal();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void updateUI();
    void userSelectedSlot(int);

    void updateCamImageSlot();

    void on_finishPourButton_clicked();

private:
    Ui::PouringWindow *ui;
    QTimer updateTimer;
    double closeWindowAt;

    void updateFinishedUI();
    void updateScanningText();
    void startCamera();
    void stopCamera();

    QString prettyEnding(int val);
};

#endif // POURINGWINDOW_H
