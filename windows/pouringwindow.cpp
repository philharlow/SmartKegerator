#include <windows/pouringwindow.h>
#include <ui_pouringwindow.h>
#include <data/pour.h>
#include <data/constants.h>
#include <QtConcurrentRun>
#include <raspicvcam.h>
#include <QCloseEvent>
#include <QTimer>
#include <data/settings.h>
#include <managers/facialrecognitionmanager.h>
#include <time.h>
#include <app.h>
#include <shared.h>
#include <stdio.h>
#include <sys/stat.h>
#include <managers/flowmetermanager.h>
#include <managers/gpiomanager.h>
#include <QCloseEvent>

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/objdetect/objdetect.hpp"


PouringWindow* PouringWindow::Instance;

double ticksPerLiter;

double ounces, liters, price;

int closeWindowAfterSeconds = 5;

bool pouring = false;
bool foundNewUser = false;
bool logPours = true;
int photoFrequency = 5;

int frameWidth = 253, frameHeight = 190;

Pour* currentPour;
User *currentUser;


PouringWindow::PouringWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PouringWindow)
{
    ui->setupUi(this);

    QObject::connect(FlowMeterManager::Instance, SIGNAL(PourStarted()), this, SLOT(onPourStarted()));
    QObject::connect(FlowMeterManager::Instance, SIGNAL(PourFinished()), this, SLOT(onPourFinished()));
    QObject::connect(FlowMeterManager::Instance, SIGNAL(FlowMeterTicked()), this, SLOT(onFlowMeterTick()));

    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateUI()));
    connect(this, SIGNAL(updateCamImageSignal()), this, SLOT(updateCamImageSlot()));

    ticksPerLiter = Settings::GetDouble("ticksPerLiter");
    logPours = Settings::GetBool("logPours");
    photoFrequency = Settings::GetInt("pourPhotoFrequency");

    currentUser = User::UnknownUser;
    currentPour == NULL;

    this->ui->usersComboBox->view()->setItemDelegate(new CustomComboBoxItem(this));
    vector<User*>::iterator iter;
    for (iter = User::UsersList.begin(); iter != User::UsersList.end(); ++iter)
    {
        User* user = (*iter);
        this->ui->usersComboBox->addItem(QString(user->Name.c_str()));
    }
    connect(this->ui->usersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(userSelectedSlot(int)));
}

void PouringWindow::onPourStarted()
{
    ShowWindow();

    closeWindowAt = -1;
    foundNewUser = false;
    pouring = true;
    currentUser = User::UnknownUser;

    currentPour = new Pour();
    currentPour->KegId = FlowMeterManager::CurrentKeg->Id;
    currentPour->UserId = 0;
    currentPour->PriceModifier = 1;

    liters = 0;
    ounces = 0;
    price = 0;

    Beer* beer = Beer::BeersById[FlowMeterManager::CurrentKeg->BeerId];

    this->ui->beerNameField->setText(beer->Name.c_str());
    this->ui->beerCompanyField->setText(beer->Company.c_str());
    int beerCount = Pour::GetPoursInLast(Constants::Day).size() + 1;
    this->ui->beerCountField->setText(QString("%1 beer in last 24h").arg(prettyEnding(beerCount)));
    this->ui->usersComboBox->setCurrentIndex(0);

    this->ui->finishPourButton->setText("Finish Pour");

    updateScanningText();

    startCamera();

    updateTimer.start(1000);
}

void PouringWindow::onPourFinished()
{
    if (logPours)
    {
        qDebug("Finished pour! Id: %d", Pour::NextId);

        currentPour->KegId = FlowMeterManager::CurrentKeg->Id;
        currentPour->Ticks = FlowMeterManager::Ticks;
        currentPour->PriceModifier = 1;
        currentPour->Ounces = ounces;
        Pour::AddPour(currentPour);
    }
    else
    {
        qDebug("Finished pour! Logging disabled");
        this->ui->beerCountField->setText(QString("Logging disabled!"));
    }

    pouring = false;

    this->ui->finishPourButton->setText("Close");
    this->ui->beerCountField->setText(QString("Enjoy your beer!"));


    closeWindowAt = (double)time(NULL) + closeWindowAfterSeconds;
}

void PouringWindow::userSelectedSlot(int index)
{
    if (index == -1)
        return;

    currentUser = User::UsersList[index];

    currentPour->UserId = currentUser->Id;
    if (pouring == false)
        User::SaveUsers();

    int beerCount = Pour::GetPoursInLast(Constants::Day, currentUser).size() + 1;
    this->ui->beerCountField->setText(QString("Your %1 beer today").arg(prettyEnding(beerCount)));
}

void PouringWindow::onFlowMeterTick()
{
    updateUI();

    if (FlowMeterManager::ConflictingTicks)
        this->ui->beerCompanyField->setText(QString("Uh oh! Both beers detected!"));
}


void PouringWindow::updateScanningText()
{
    if (currentUser == User::UnknownUser)
    {
        int faces = FacialRecognitionManager::FaceCount;
        if (faces == 0)
            this->ui->scanningField->setText(QString("Scanning..."));
        else
            this->ui->scanningField->setText(QString("1 face detected!"));
    }
    else
        this->ui->scanningField->setText(QString("Hello %1!").arg(currentUser->Name.c_str()));
}

void PouringWindow::updateUI()
{
    if (pouring == false)
    {
        double now = time(NULL);
        if (closeWindowAt < now)
            HideWindow();
        return;
    }

    updateScanningText();

    if (foundNewUser)
    {
        int userIndex = std::find(User::UsersList.begin(), User::UsersList.end(), currentUser) - User::UsersList.begin();
        this->ui->usersComboBox->setCurrentIndex(userIndex);
        foundNewUser = false;
    }

    liters = FlowMeterManager::Ticks / (double)ticksPerLiter;
    ounces = liters * Constants::OuncesPerLiter;
    price = FlowMeterManager::CurrentKeg->GetPrice(liters);

    this->ui->volumePouredField->setText(QString("%1oz poured : %2").arg(QString::number(ounces, 'f', 1)).arg(FlowMeterManager::Ticks));
    this->ui->priceField->setText(QString("$%1").arg(QString::number(price, 'f', 2)));
}

QString PouringWindow::prettyEnding(int val)
{
    if (val == 11 || val == 12 || val == 13)
        return QString("%1th").arg(val);
    if ((val % 10) == 1)
        return QString("%1st").arg(val);
    if ((val % 10) == 2)
        return QString("%1nd").arg(val);
    if ((val % 10) == 3)
        return QString("%1rd").arg(val);
    return QString("%1th").arg(val);
}

void PouringWindow::on_finishPourButton_clicked()
{
    if (pouring)
        FlowMeterManager::Instance->FinishPour();
    else
        HideWindow();
}

void PouringWindow::closeEvent(QCloseEvent *event)
{
    HideWindow();
    event->ignore();
}

void PouringWindow::ShowWindow()
{
    if (this->isHidden() == false)
        return;

    MainWindow::Instance->HideWindow();

    if (App::Fullscreen)
        this->showFullScreen();
    else
    {
        this->move(App::WindowX, App::WindowY);
        this->show();
    }
}

void PouringWindow::HideWindow()
{
    stopCamera();
    currentPour = NULL;

    this->updateTimer.stop();
    MainWindow::Instance->ShowWindow();
    if (this->isHidden())
        return;
    this->hide();
}

PouringWindow::~PouringWindow()
{
    delete ui;
}




int nextPhotoAt = 0;
Mat jpgMat;
QImage qImage;

void takeSnapshot()
{
    double now = time(NULL);

    if (nextPhotoAt <= now)
    {
        nextPhotoAt = now + photoFrequency;

        int index = currentPour->ImagePaths.size() + 1;

        string location = QString(Settings::GetString("pourPhotosLocation").c_str()).arg(Pour::NextId).arg(index).toStdString();

        if (index == 1)
        {
            string folder = location.substr(0, location.find_last_of('/'));
            mkdir(folder.c_str(), 0777);
        }

        cvtColor(RaspiCvCam::ImageMat,jpgMat,RaspiCvCam::UseColor ? CV_RGB2BGR : CV_GRAY2BGR);	// convert in RGB color space (slow)

        if (FacialRecognitionManager::FaceCount > 0)
        {
            Rect face_i = FacialRecognitionManager::Faces[0];
            rectangle(jpgMat, face_i, CV_RGB(255, 255 ,255), 1);

            /*
            //User user = detectUser();
            string userName = "Unknown";//User == NULL ? "Unknown" : :user.Name;
            int pos_x = std::max(face_i.tl().x, 0);
            int pos_y = std::max(face_i.tl().y - 3, 0);
            putText(jpgMat, userName, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,255,255), 1.0);
            */
        }

        imwrite(location, jpgMat);
        qDebug("saving pour snap to: %s", location.c_str());
        currentPour->ImagePaths.push_back(location);
        if (pouring == false)
            Pour::SavePours();
    }
}


void handleFrame()
{
    int userId = FacialRecognitionManager::RunRecognition();
    if (userId > -1)
    {
        currentUser = User::UsersById[userId];
        foundNewUser = true;
    }

    takeSnapshot();

    // Scale for UI
    cv::resize(RaspiCvCam::ImageMat, RaspiCvCam::ImageMat, cv::Size(frameWidth, frameHeight));

    double scale = (double)frameWidth/(double)RaspiCvCam::SourceWidth;

    if (FacialRecognitionManager::FaceCount > 0)
    {
        Rect face_i = FacialRecognitionManager::Faces[0];

        // Scale rect to new size
        face_i.x = face_i.x * scale;
        face_i.y = face_i.y * scale;
        face_i.width = face_i.width * scale;
        face_i.height = face_i.height * scale;


        // draw frame
        rectangle(RaspiCvCam::ImageMat, face_i, CV_RGB(255, 255 ,255), 1);

        // draw name
        //User user = detectUser();
        string userName = currentUser->Name;//User == NULL ? "Unknown" : :user.Name;
        int pos_x = std::max(face_i.tl().x, 0);
        int pos_y = std::max(face_i.tl().y - 3, 0);
        putText(RaspiCvCam::ImageMat, userName, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,255,255), 1.0);
    }

    double fps = (double)RaspiCvCam::FrameCount / difftime(time(NULL), RaspiCvCam::CameraStartedAt);
    string fpsText = QString("FPS: %1").arg(QString::number(fps, 'f', 1)).toStdString();
    putText(RaspiCvCam::ImageMat, fpsText, Point(3, frameHeight - 3), FONT_HERSHEY_PLAIN, 1, CV_RGB(255,255,255), 1.0);

    if (RaspiCvCam::UseColor)
    {
        qImage = QImage((uchar*)RaspiCvCam::ImageMat.data, RaspiCvCam::ImageMat.cols, RaspiCvCam::ImageMat.rows, RaspiCvCam::ImageMat.step, QImage::Format_RGB888);
    }
    else
    {
        qImage = QImage((uchar*)RaspiCvCam::ImageMat.data, RaspiCvCam::ImageMat.cols, RaspiCvCam::ImageMat.rows, RaspiCvCam::ImageMat.step, QImage::Format_Indexed8);
        qImage.setColorTable(RaspiCvCam::GrayscaleColorTable);
    }

    PouringWindow::Instance->updateCamImage();
}


void PouringWindow::startCamera()
{
    nextPhotoAt = (double)time(NULL) + 2; //Let the camera wake a little

    GPIOManager::EnableCameraLights(true);

    FacialRecognitionManager::Faces.clear();
    FacialRecognitionManager::FaceCount = 0;
    FacialRecognitionManager::Enabled = Settings::GetBool("facialRecEnabled");

    RaspiCvCam::UseColor = Settings::GetBool("cameraUseColor");
    RaspiCvCam::FrameUpdatedCallback = &handleFrame;
    RaspiCvCam::StartCamera();
}

void PouringWindow::stopCamera()
{
    ui->cameraDisplay->clear();
    GPIOManager::EnableCameraLights(false);
    RaspiCvCam::StopCamera();
    RaspiCvCam::FrameUpdatedCallback = NULL;
}

void PouringWindow::updateCamImageSlot()
{
    QPixmap qp = QPixmap::fromImage(qImage);
    this->ui->cameraDisplay->setPixmap(qp);//.scaled(frameWidth, frameHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void PouringWindow::updateCamImage()
{
    emit updateCamImageSignal();
}
