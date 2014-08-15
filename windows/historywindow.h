#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <data/pour.h>

using namespace std;

namespace Ui {
class HistoryWindow;
}

class HistoryWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit HistoryWindow(QWidget *parent = 0);
    ~HistoryWindow();

    static HistoryWindow* Instance;

    static void ShowWindow();
    void HideWindow();


protected:
    void closeEvent(QCloseEvent *);

private slots:
    void setSelectedIndex(int pourId);
    void pourFinishedSlot();
    void userSelectedSlot(int);
    void on_closeButton_clicked();
    void on_nextButton_clicked();
    void on_prevButton_clicked();

    void on_deleteButton_clicked();

private:
    Ui::HistoryWindow *ui;
    int selectedIndex;
    Pour* selectedPour;

    void updateButtons();
    void updateImage();

};

#endif // HISTORYWINDOW_H
