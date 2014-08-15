#include <windows/historywindow.h>
#include <ui_historywindow.h>
#include <app.h>
#include <shared.h>
#include <QStyledItemDelegate>
#include <QtConcurrentRun>
#include <QFont>
#include <QCloseEvent>
#include <managers/flowmetermanager.h>
#include <QMessageBox>

using namespace std;

HistoryWindow* HistoryWindow::Instance;

int picIndex;

HistoryWindow::HistoryWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HistoryWindow)
{
    ui->setupUi(this);

    connect(FlowMeterManager::Instance, SIGNAL(PourFinished()), this, SLOT(pourFinishedSlot()));
    connect(this->ui->scrollPane, SIGNAL(SelectionChanged(int)), this, SLOT(setSelectedIndex(int)));

    picIndex = 0;
    selectedIndex = -1;
    selectedPour = NULL;

    QAbstractItemView *view = this->ui->usersComboBox->view();
    view->setItemDelegate(new CustomComboBoxItem(this));
    connect(this->ui->usersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(userSelectedSlot(int)));

    vector<User*>::iterator iter;
    for (iter = User::UsersList.begin(); iter != User::UsersList.end(); ++iter)
    {
        User* user = (*iter);
        this->ui->usersComboBox->addItem(QString(user->Name.c_str()));
    }
    this->ui->usersComboBox->setCurrentIndex(-1);

    //QtConcurrent::run(this, &HistoryWindow::updateButtons);
}

void HistoryWindow::userSelectedSlot(int index)
{
    if (index == -1 || selectedIndex == -1)
        return;

    User* user = User::UsersList[index];
    if (selectedPour->UserId == user->Id)
        return;

    selectedPour->UserId = user->Id;
    Pour::SavePours();
    updateButtons();
}

void HistoryWindow::pourFinishedSlot()
{
    if (this->isHidden())
        return;

    updateButtons();
    this->ui->scrollPane->SetSelectedIndex(0);
}


void HistoryWindow::on_deleteButton_clicked()
{
    if (selectedIndex == -1)
        return;

    if (KeyboardDialog::VerifyPassword() == false)
        return;

    QMessageBox::StandardButton response;
    response = QMessageBox::question(this, "Confirm", "Are you sure you want to remove this pour?", QMessageBox::Yes | QMessageBox::No);

    if (response != QMessageBox::Yes)
        return;

    Pour::RemovePour(selectedPour);

    updateButtons();
    this->ui->scrollPane->SetSelectedIndex(-1);
}


void HistoryWindow::updateButtons()
{
    vector<string> labels;
    for(unsigned int i=0; i<Pour::AllPours.size(); i++)
    {
        Pour* pour = Pour::AllPours[Pour::AllPours.size() - i - 1];
        Keg* keg = Keg::KegsById[pour->KegId];
        Beer* beer = Beer::BeersById[keg->BeerId];
        User* user = User::UsersById[pour->UserId];

        QString str = QString("%1 - %2\n$%3 - %4oz - %5")
                .arg(user->Name.c_str())
                .arg(beer->Name.c_str())
                .arg(QString::number(pour->Price, 'f', 2))
                .arg(QString::number(pour->Ounces, 'f', 2))
                .arg(pour->TimeString.c_str());
        labels.push_back(str.toStdString());
    }

    this->ui->scrollPane->SetOptions(labels);
}

void HistoryWindow::setSelectedIndex(int index)
{
    if (selectedIndex != index)
        picIndex = 0;

    selectedIndex = index;

    if (selectedIndex == -1)
    {
        selectedPour = NULL;
        this->ui->infoLabel->setText("Select an item");
        this->ui->usersComboBox->setEnabled(false);
        this->ui->usersComboBox->setCurrentIndex(-1);
    }
    else
    {
        selectedPour = Pour::AllPours[Pour::AllPours.size() - selectedIndex - 1];
        Keg* keg = Keg::KegsById[selectedPour->KegId];
        Beer* beer = Beer::BeersById[keg->BeerId];
        User* user = User::UsersById[selectedPour->UserId];

        int userIndex = std::find(User::UsersList.begin(), User::UsersList.end(), user) - User::UsersList.begin();
        this->ui->usersComboBox->setEnabled(true);
        this->ui->usersComboBox->setCurrentIndex(userIndex);

        // Time - User
        // Beer - Price - Ounces
        QString str = QString("Time: %1\nBeer: %2\nPrice: $%3\nOunces: %4oz\nTicks: %5")
                .arg(selectedPour->TimeString.c_str())
                .arg(QString("%1 - %2").arg(beer->Company.c_str()).arg(beer->Name.c_str()))
                .arg(QString::number(selectedPour->Price, 'f', 2))
                .arg(QString::number(selectedPour->Ounces, 'f', 2))
                .arg(selectedPour->Ticks);
        this->ui->infoLabel->setText(str);
    }

    updateImage();
}

void HistoryWindow::updateImage()
{
    this->ui->prevButton->setEnabled(picIndex > 0);
    if (selectedPour != NULL && selectedPour->ImagePaths.size() > 0)
    {
        this->ui->snapIndexLabel->setText(QString("%1/%2").arg(picIndex + 1).arg(selectedPour->ImagePaths.size()));
        this->ui->nextButton->setEnabled(picIndex < (int)selectedPour->ImagePaths.size() - 1);
        string location = selectedPour->ImagePaths[picIndex];
        QPixmap qp(QString(location.c_str()));
        this->ui->previewLabel->setPixmap(qp.scaled(260, 193, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        this->ui->previewLabel->clear();
        this->ui->snapIndexLabel->setText("");
        this->ui->nextButton->setEnabled(false);
    }
}

void HistoryWindow::on_nextButton_clicked()
{
    picIndex++;
    updateImage();
}

void HistoryWindow::on_prevButton_clicked()
{
    picIndex--;
    updateImage();
}

void HistoryWindow::on_closeButton_clicked()
{
    this->HideWindow();
}

void HistoryWindow::closeEvent(QCloseEvent *event)
{
    HideWindow();
    event->ignore();
}

void HistoryWindow::ShowWindow()
{
    if (HistoryWindow::Instance == NULL)
        HistoryWindow::Instance = new HistoryWindow();

    HistoryWindow::Instance->updateButtons();
    HistoryWindow::Instance->ui->scrollPane->SetSelectedIndex(-1);

    if (HistoryWindow::Instance->isHidden() == false)
        return;

    if (App::Fullscreen)
        HistoryWindow::Instance->showFullScreen();
    else
    {
        HistoryWindow::Instance->move(App::WindowX, App::WindowY);
        HistoryWindow::Instance->show();
    }
}

void HistoryWindow::HideWindow()
{
    MainWindow::Instance->ShowWindow();
    if (this->isHidden())
        return;
    this->hide();
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}
