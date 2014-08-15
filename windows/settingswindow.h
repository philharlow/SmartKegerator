#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <windows/keyboarddialog.h>

using namespace std;

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

    static SettingsWindow* Instance;

    static void ShowWindow();
    void HideWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_closeButton_clicked();
    void on_option2Button_clicked();
    void on_option1Button_clicked();
    void on_option3Button_clicked();
    void on_option4Button_clicked();

    void on_addKegButton_clicked();
    void on_finishKegButton_clicked();

    void on_editBeerButton_clicked();

    void on_editKegButton_clicked();

    void on_setTimeButton_clicked();

    void on_addUserButton_clicked();

    void on_addPaymentButton_clicked();

private:
    Ui::SettingsWindow *ui;

    void updateUI();
};

#endif // SETTINGSWINDOW_H
