#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QProcess>
#include <queue>
#include <data/keg.h>
#include <data/beer.h>
#include <widgets/graph/graph.h>

using namespace std;

class BeerDisplay;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static MainWindow* Instance;

    void ShowWindow();
    void HideWindow();

private slots:
    void moveBackground();
    void updateUI();
    void onTempData();

    void on_closeButton_clicked();
    void on_settingsButton_clicked();
    void on_historyButton_clicked();
    void on_usersButton_clicked();

    void on_temperatureButton_clicked();

    void on_humidityButton_clicked();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    QTimer updateTimer;
    bool hidden;
    Graph* tempGraph;
    Graph* humGraph;

    BeerDisplay *leftDisplay;
    BeerDisplay *rightDisplay;

    void onQuit();
    void updateTime();
    void updateBeers();

};


// Beer display
class BeerDisplay
{
public:
    BeerDisplay(QWidget* keg, QLabel *logoLabel)
    {
        lastId = -1;
        kegFill = keg;
        kegHeight = keg->height();
        kegY = keg->y();
        logo = logoLabel;
        logo->setFrameShape(QFrame::NoFrame);
    }

    QLabel* nameField;
    QLabel* companyField;
    QLabel* typeField;
    QLabel* ibuField;
    QLabel* abvField;
    QLabel* priceField;
    QLabel* boughtField;
    QLabel* emptyField;
    QLabel* logo;

    int kegHeight, kegY;
    QWidget* kegFill;

    int lastId;

    void update(Keg* keg)
    {
        if (keg == NULL)
        {
            setKegFill(0);
            nameField->setText("");
            companyField->setText("");
            typeField->setText("");
            priceField->setText("-");
            abvField->setText("-");
            ibuField->setText("-");
            boughtField->setText("-");
            emptyField->setText("-");

            if (lastId != -1)
                logo->clear();
            lastId = -1;
        }
        else
        {
            Beer* beer = Beer::BeersById[keg->BeerId];
            setKegFill(1-(keg->LitersPoured / keg->LitersCapacity));
            nameField->setText(beer->Name.c_str());
            companyField->setText(beer->Company.c_str());
            typeField->setText(beer->Type.c_str());
            priceField->setText(QString("$%1").arg(QString::number(keg->PricePerPint, 'f', 2)));
            abvField->setText(QString("%1%").arg(QString::number(beer->ABV, 'f', 1)));
            ibuField->setText(QString("%1").arg(beer->IBU));
            boughtField->setText(QString("%1").arg(keg->DateBought.c_str()));
            emptyField->setText(QString("%1").arg(keg->GetEmptyDate().c_str()));

            if (lastId != keg->Id)
            {
                QPixmap qp(beer->LogoPath.c_str());
                logo->setPixmap(qp);
            }
            lastId = keg->Id;
        }
    }

    void setKegFill(double percent)
    {
        int newHeight = kegHeight * percent;
        kegFill->setGeometry(kegFill->x(), kegY + kegHeight - newHeight, kegFill->width(), newHeight);
    }

};



#endif // MAINWINDOW_H
