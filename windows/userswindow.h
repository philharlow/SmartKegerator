#ifndef USERSWINDOW_H
#define USERSWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <data/user.h>

using namespace std;

namespace Ui {
class UsersWindow;
}

class UsersWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit UsersWindow(QWidget *parent = 0);
    ~UsersWindow();

    static UsersWindow* Instance;

    static void ShowWindow();
    void HideWindow();

    // move these to private
    void updateUserImage();
    void updateCamImage();

signals:
    void updateCamImageSignal();

protected:
    void closeEvent(QCloseEvent *);
    
private slots:
    void setSelectedIndex(int index);
    void on_closeButton_clicked();
    void on_startCameraButton_clicked();
    void on_saveImageButton_clicked();
    void on_startSearchButton_clicked();

    void updateCamImageSlot();

    void onPourStarted();

    void on_prevImageButton_clicked();
    void on_nextImageButton_clicked();
    void on_deleteImageButton_clicked();

    void on_trainButton_clicked();

private:
    Ui::UsersWindow *ui;
    User* selectedUser;
    int selectedIndex;

    void updateUsersList();
    void updateLearningButtons();

};

#endif // USERSWINDOW_H
