#include "windows/userswindow.h"
#include "ui_userswindow.h"
#include "data/settings.h"
#include "data/payment.h"
#include "app.h"
#include "stdio.h"
#include "raspicvcam.h"
#include "sys/stat.h"
#include "sys/types.h"
#include <QGridLayout>
#include <QFont>
#include <QCloseEvent>
#include <QMessageBox>
#include <QtConcurrentRun>
#include <managers/facialrecognitionmanager.h>
#include <managers/gpiomanager.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

UsersWindow* UsersWindow::Instance;


unsigned int userPhotoIndex = 0;

UsersWindow::UsersWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UsersWindow)
{
    ui->setupUi(this);

    connect(FlowMeterManager::Instance, SIGNAL(PourStarted()), this, SLOT(onPourStarted()));
    connect(this->ui->scrollPane, SIGNAL(SelectionChanged(int)), this, SLOT(setSelectedIndex(int)));
    connect(this, SIGNAL(updateCamImageSignal()), this, SLOT(updateCamImageSlot()));

    updateUsersList();
    updateLearningButtons();
    this->ui->scrollPane->SetSelectedIndex(-1);

    this->ui->saveImageButton->setEnabled(false);
    this->ui->startSearchButton->setEnabled(false);
}



void UsersWindow::updateUsersList()
{
    vector<string> users;
    // Update text
    for(unsigned int i=0; i<User::UsersList.size(); i++)
    {
        User* user = User::UsersList[i];
        QString str = QString("%2").arg(user->Name.c_str());
        users.push_back(str.toStdString());
    }

    this->ui->scrollPane->SetOptions(users);
}
void UsersWindow::setSelectedIndex(int index)
{
    if (selectedIndex != index)
        userPhotoIndex = 0;

    selectedIndex = index;

    if (selectedIndex == -1)
    {
        selectedUser = NULL;
        this->ui->infoLabel->setText("Select a user");
        this->ui->learningGroupBox->setVisible(false);
    }
    else
    {
        selectedUser = User::UsersList[selectedIndex];

        vector<Pour*> pours = Pour::GetPoursByUserId(selectedUser->Id);

        double owed = 0;

        map<int, int> beersPouredById;
        int highestBeerCount = 0;
        Beer* favoriteBeer = NULL;

        for(unsigned int i=0; i<pours.size(); i++)
        {
            Pour* pour = pours[i];
            Keg* keg = Keg::KegsById[pour->KegId];
            owed += pour->Price;
            beersPouredById[keg->BeerId]++;
            if (beersPouredById[keg->BeerId] > highestBeerCount)
            {
                highestBeerCount = beersPouredById[keg->BeerId];
                favoriteBeer = Beer::BeersById[keg->BeerId];
            }
        }

        double paid = 0;

        for(unsigned int i=0; i<Payment::PaymentsByUserId[selectedUser->Id].size(); i++)
        {
            Payment* payment = Payment::PaymentsByUserId[selectedUser->Id][i];
            paid += payment->Amount;
        }

        owed -= paid;

        qDebug("Owed: %f Paid: %f", owed, paid);

        QString favoriteBeerStr = favoriteBeer == NULL ? QString("") : QString("%1-%2").arg(favoriteBeer->Company.c_str()).arg(favoriteBeer->Name.c_str());
        QString pourStr = QString::number(pours.size());

        QString str = QString("Id: %1\nName: %2\nOutstanding: $%3\nFavorite beer: %4\nPours: %5")
                .arg(selectedUser->Id)
                .arg(selectedUser->Name.c_str())
                .arg(QString::number(owed, 'f', 2))
                .arg(favoriteBeerStr)
                .arg(pourStr);
        this->ui->infoLabel->setText(str);

        this->ui->learningGroupBox->setVisible(true);
    }

    updateUserImage();
}

void UsersWindow::updateUserImage()
{
    this->ui->prevImageButton->setEnabled(userPhotoIndex > 0);
    if (selectedUser != NULL && selectedUser->ImagePaths.size() > 0)
    {
        this->ui->deleteImageButton->setEnabled(true);
        this->ui->imageCountLabel->setText(QString("%1/%2").arg(userPhotoIndex + 1).arg(selectedUser->ImagePaths.size()));
        this->ui->nextImageButton->setEnabled(userPhotoIndex < selectedUser->ImagePaths.size() - 1);
        string location = selectedUser->ImagePaths[userPhotoIndex];
        this->ui->userImageLabel->setPixmap(QPixmap(QString(location.c_str())));
    }
    else
    {
        this->ui->userImageLabel->clear();
        this->ui->imageCountLabel->setText("");
        this->ui->nextImageButton->setEnabled(false);
        this->ui->deleteImageButton->setEnabled(false);
    }
}


void UsersWindow::on_prevImageButton_clicked()
{
    userPhotoIndex--;
    updateUserImage();
}

void UsersWindow::on_nextImageButton_clicked()
{
    userPhotoIndex++;
    updateUserImage();
}

void UsersWindow::on_deleteImageButton_clicked()
{
    QMessageBox::StandardButton confirm = QMessageBox::question(this, "Confirm", "Really delete photo?", QMessageBox::Yes | QMessageBox::No);
    if (confirm != QMessageBox::Yes)
        return;

    string location = selectedUser->ImagePaths[userPhotoIndex];
    qDebug("Removing photo: %s", location.c_str());

    remove(location.c_str());
    selectedUser->ImagePaths.erase(selectedUser->ImagePaths.begin() + userPhotoIndex);

    User::SaveUsers();

    userPhotoIndex = 0;
    updateUserImage();
}


void UsersWindow::ShowWindow()
{
    if (UsersWindow::Instance == NULL)
        UsersWindow::Instance = new UsersWindow();

    UsersWindow::Instance->updateUsersList();
    UsersWindow::Instance->updateLearningButtons();
    UsersWindow::Instance->ui->scrollPane->SetSelectedIndex(-1);

    if (UsersWindow::Instance->isHidden() == false)
        return;

    if (App::Fullscreen)
        UsersWindow::Instance->showFullScreen();
    else
    {
        UsersWindow::Instance->move(App::WindowX, App::WindowY);
        UsersWindow::Instance->show();
    }
}


void UsersWindow::closeEvent(QCloseEvent *event)
{
    HideWindow();
    event->ignore();
}

void UsersWindow::on_closeButton_clicked()
{
    this->HideWindow();
}

void UsersWindow::HideWindow()
{
    MainWindow::Instance->ShowWindow();
    RaspiCvCam::StopCamera();
    if (this->isHidden())
        return;
    this->hide();
}

UsersWindow::~UsersWindow()
{
    delete ui;
}





///////////////////////////////////////////////////////////////////////////////////////////////////
// MOVING TO FACETRAINER
///////////////////////////////////////////////////////////////////////////////////////////////////


CvPoint Myeye_left;
CvPoint Myeye_right;
int bEqHisto = 1;

CvPoint Myoffset_pct;
CvPoint Mydest_sz;
std::vector<int> qualityType;

Mat faceMat, croppedMat, faceResizedMat;
QImage faceImage, croppedImage;
QString statusText;
bool searching;
bool foundCroppedImage;

int detectAndDisplay();

//////////////////////////////////////////////
// compute distance btw 2 points
//////////////////////////////////////////////
float Distance(CvPoint p1, CvPoint p2)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    return sqrt(dx*dx+dy*dy);
}

//////////////////////////////////////////////
// rotate picture (to align eyes-y)
//////////////////////////////////////////////
Mat rotate(Mat& image, double angle, CvPoint centre)
{
    Point2f src_center(centre.x, centre.y);
    // conversion en degre
    angle = angle*180.0/3.14157;
    //DEBUG printf("(D) rotate : rotating : %fÂ° %d %d\n",angle, centre.x, centre.y);
    Mat rot_matrix = getRotationMatrix2D(src_center, angle, 1.0);

    Mat rotated_img(Size(image.size().height, image.size().width), image.type());

    warpAffine(image, rotated_img, rot_matrix, image.size());
    return (rotated_img);
}



//////////////////////////////////////////////
// crop picture
//////////////////////////////////////////////
int CropFace(Mat &MyImage,
    CvPoint eye_left,
    CvPoint eye_right,
    CvPoint offset_pct,
    CvPoint dest_sz)
{

    // calculate offsets in original image
    int offset_h = (offset_pct.x*dest_sz.x/100);
    int offset_v = (offset_pct.y*dest_sz.y/100);
    //DEBUG printf("(D) CropFace : offeth=%d, offsetv=%d\n",offset_h,offset_v);

    // get the direction
    CvPoint eye_direction;
    eye_direction.x = eye_right.x - eye_left.x;
    eye_direction.y = eye_right.y - eye_left.y;

    // calc rotation angle in radians
    float rotation = atan2((float)(eye_direction.y),(float)(eye_direction.x));

    // distance between them
    float dist = Distance(eye_left, eye_right);
    //DEBUG printf("(D) CropFace : dist=%f\n",dist);

    // calculate the reference eye-width
    int reference = dest_sz.x - 2*offset_h;

    // scale factor
    float scale = dist/(float)reference;
    //DEBUG printf("(D) CropFace : scale=%f\n",scale);

    // rotate original around the left eye
    MyImage = rotate(MyImage, (double)rotation, eye_left);

    // crop the rotated image
    CvPoint crop_xy;
    crop_xy.x = eye_left.x - scale*offset_h;
    crop_xy.y = eye_left.y - scale*offset_v;

    CvPoint crop_size;
    crop_size.x = dest_sz.x*scale;
    crop_size.y = dest_sz.y*scale;

    // Crop the full image to that image contained by the rectangle myROI
    //qDebug("(D) CropFace : crop_xy.x=%d, crop_xy.y=%d, crop_size.x=%d, crop_size.y=%d",crop_xy.x, crop_xy.y, crop_size.x, crop_size.y);

    cv::Rect myROI(crop_xy.x, crop_xy.y, crop_size.x, crop_size.y);
    if ((crop_xy.x >= 0) &&
        (crop_xy.y >= 0) &&
        (crop_xy.x + crop_size.x < MyImage.size().width) &&
        (crop_xy.y + crop_size.y < MyImage.size().height))
    {
        MyImage = MyImage(myROI);
    }
    else
    {
        qDebug("-- error cropping");
        return 0;
    }

    //resize it
    //qDebug("-- resize image");
    cv::resize(MyImage, MyImage, Size(dest_sz));

    return 1;
}


//////////////////////////////////////////////
// Main program
//////////////////////////////////////////////
void handleFrame2()
{
    // Apply the classifier to the frame
    if( !RaspiCvCam::ImageMat.empty() )
    {
        int result = detectAndDisplay();
        if (result>0)
        {
            if (CropFace(RaspiCvCam::ImageMat, Myeye_left, Myeye_right, Myoffset_pct, Mydest_sz)==1)
            {
                croppedMat = Mat(RaspiCvCam::ImageMat);
                equalizeHist(croppedMat, croppedMat);
                croppedImage = QImage((uchar*)croppedMat.data, croppedMat.cols, croppedMat.rows, croppedMat.step, QImage::Format_Indexed8);
                croppedImage.setColorTable(RaspiCvCam::GrayscaleColorTable);
                foundCroppedImage = true;
                searching = false;
            }
            else
            {
                qDebug("- crop face failed");
            }
        }

        UsersWindow::Instance->updateCamImage();
    }

}
//////////////////////////////////////////////
// detectAndDisplay
//////////////////////////////////////////////
int detectAndDisplay()
{
    // detect faces
    FacialRecognitionManager::SearchForFaces();

    //RaspiCvCam::QuickFaceCascade.detectMultiScale(imageMat, RaspiCvCam::Faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, Size(80,80));
    //RaspiCvCam::QuickFaceCascade.detectMultiScale( *frame, RaspiCvCam::Faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );

    //.detectMultiScale(imageMat, RaspiCvCam::Faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, Size(80,80));

    // simplify : we only take picture with one face !
    //DEBUG printf("(D) detectAndDisplay : nb face=%d\n",faces.size());
    if (FacialRecognitionManager::FaceCount==0)
    {
        statusText = "No faces found!";
        return 0;
    }
    else
    {
        //qDebug("- face found. looking for eyes");
        statusText = "Face found!";
        Rect rect = FacialRecognitionManager::Faces[0];

        faceMat = RaspiCvCam::ImageMat( rect );
        cv::resize(faceMat, faceResizedMat, Size(Mydest_sz), 1.0, 1.0, CV_INTER_NN);
        faceImage = QImage((uchar*)faceResizedMat.data, faceResizedMat.cols, faceResizedMat.rows, faceResizedMat.step, QImage::Format_Indexed8);
        faceImage.setColorTable(RaspiCvCam::GrayscaleColorTable);

        if (searching == false)
            return 0;

        std::vector<Rect> eyes;

        //-- In each face, detect eyes
        FacialRecognitionManager::EyesCascade.detectMultiScale( faceMat, eyes, 1.05, 3, 0 |CV_HAAR_SCALE_IMAGE, Size(20, 20) );

        // if no glasses
        if (eyes.size()==2)
        {
            statusText = "Found face without glasses!";
            //qDebug("-- face without glasses");
            // detect eyes
            for( size_t j=0; j<2; j++ )
            {
                Point eye_center( rect.x + eyes[1-j].x + eyes[1-j].width/2, rect.y + eyes[1-j].y + eyes[1-j].height/2 );

                if (j==0) // left eye
                {
                    Myeye_left.x = eye_center.x;
                    Myeye_left.y = eye_center.y;
                }
                if (j==1) // right eye
                {
                    Myeye_right.x = eye_center.x;
                    Myeye_right.y = eye_center.y;
                }
            }
        }
        else
        {
            //qDebug("-- checking for glasses");
            // tests with glasses
            FacialRecognitionManager::GlassesCascade.detectMultiScale( faceMat, eyes, 1.05, 3, 0 |CV_HAAR_SCALE_IMAGE, Size(20, 20) );
            if (eyes.size() != 2)
                return 0;
            else
            {

                //qDebug("-- face with glasses");
                statusText = "Found face with glasses!";

                for( size_t j=0; j<2; j++ )
                {
                    Point eye_center( rect.x + eyes[1-j].x + eyes[1-j].width/2, rect.y + eyes[1-j].y + eyes[1-j].height/2 );
                    if (j==0) // left eye
                    {
                        Myeye_left.x = eye_center.x;
                        Myeye_left.y = eye_center.y;
                    }
                    if (j==1) // right eye
                    {
                        Myeye_right.x = eye_center.x;
                        Myeye_right.y = eye_center.y;
                    }
                }
            }
        }
    }

    // sometimes eyes are inversed ! we switch them
    if (Myeye_right.x < Myeye_left.x)
    {
        int tmpX = Myeye_right.x;
        int tmpY = Myeye_right.y;
        Myeye_right.x = Myeye_left.x;
        Myeye_right.y = Myeye_left.y;
        Myeye_left.x = tmpX;
        Myeye_left.y = tmpY;
        qDebug("-- eyes are switched, flipping");
    }

    return 1;
}

bool userInited = false;
void UsersWindow::on_startCameraButton_clicked()
{
    if (RaspiCvCam::CameraOn)
    {
        RaspiCvCam::StopCamera();
        GPIOManager::EnableCameraLights(false);
        updateLearningButtons();
        return;
    }

    if (userInited == false)
    {
        Myoffset_pct.x = 0.3 * 100.0;
        Myoffset_pct.y = Myoffset_pct.x;

        Mydest_sz.x = 100;
        Mydest_sz.y = Mydest_sz.x;

        qualityType.push_back(CV_IMWRITE_JPEG_QUALITY);
        qualityType.push_back(90);
        userInited = true;
    }

    foundCroppedImage = false;
    searching = false;

    FacialRecognitionManager::Faces.clear();
    FacialRecognitionManager::FaceCount = 0;

    RaspiCvCam::UseColor = false;
    RaspiCvCam::FrameUpdatedCallback = &handleFrame2;
    RaspiCvCam::StartCamera();

    GPIOManager::EnableCameraLights(true);

    updateLearningButtons();
}


void UsersWindow::updateCamImageSlot()
{
    this->ui->learningImageLabel1->setPixmap(QPixmap::fromImage(faceImage));
    this->ui->learningImageLabel2->setPixmap(QPixmap::fromImage(croppedImage));
    this->ui->learningStatusLabel->setText(statusText);
    updateLearningButtons();
}

void UsersWindow::updateCamImage()
{
    emit updateCamImageSignal();
}

void UsersWindow::on_saveImageButton_clicked()
{
    string location = QString(Settings::GetString("userPhotosLocation").c_str()).arg(selectedUser->Id).arg(selectedUser->NextImageId++).toStdString();

    if (selectedUser->NextImageId == 1)
    {
        string folder = location.substr(0, location.find_last_of('/'));
        mkdir(folder.c_str(), 0777);
    }

    imwrite(location, croppedMat);
    qDebug("saving user photo to: %s", location.c_str());
    selectedUser->ImagePaths.push_back(location);
    User::SaveUsers();

    updateUserImage();

    foundCroppedImage = false;
    updateLearningButtons();
}

void UsersWindow::on_startSearchButton_clicked()
{
    searching = true;
    foundCroppedImage = false;
    updateLearningButtons();
}

void UsersWindow::updateLearningButtons()
{
    this->ui->startSearchButton->setText(QString(searching ? "Searching..." : "Start Search"));
    this->ui->saveImageButton->setEnabled(selectedUser != NULL && foundCroppedImage && searching == false);
    this->ui->startSearchButton->setEnabled(RaspiCvCam::CameraOn);

    this->ui->startCameraButton->setText(QString(RaspiCvCam::CameraOn ? "Stop Camera" : "Start Camera"));

    if (RaspiCvCam::CameraOn == false)
    {
        this->ui->learningStatusLabel->setText(QString("Stopped"));
        this->ui->learningImageLabel1->clear();
        this->ui->learningImageLabel2->clear();
    }
}

void UsersWindow::onPourStarted()
{
    //RaspiCvCam::StopCamera();
    updateLearningButtons();
}

void UsersWindow::on_trainButton_clicked()
{
    QtConcurrent::run(FacialRecognitionManager::Train);
}
