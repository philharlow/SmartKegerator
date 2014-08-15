#include <managers/facialrecognitionmanager.h>
#include <managers/gpiomanager.h>
#include <QApplication>
#include <data/user.h>
#include <data/settings.h>
#include <raspicvcam.h>


CascadeClassifier FacialRecognitionManager::QuickFaceCascade;
CascadeClassifier FacialRecognitionManager::FaceCascade;
CascadeClassifier FacialRecognitionManager::EyesCascade;
CascadeClassifier FacialRecognitionManager::GlassesCascade;
Eigenfaces FacialRecognitionManager::FaceRecModel;

vector< Rect_<int> > FacialRecognitionManager::Faces;
int FacialRecognitionManager::FaceCount = 0;
Mat FacialRecognitionManager::FaceMat;
Mat FacialRecognitionManager::FaceResizedMat;
bool FacialRecognitionManager::Trained = false;
bool FacialRecognitionManager::Enabled = false;

void FacialRecognitionManager::Init()
{
    Enabled = Settings::GetBool("facialRecEnabled");

    // Face rec
    string face_cascade_loc = Settings::GetString("facialRecFaceCascadeXML");
    string quick_face_cascade_loc = Settings::GetString("facialRecQuickFaceCascadeXML");
    string eyes_cascade_loc = Settings::GetString("facialRecEyesCascadeXML");
    string glasses_cascade_loc = Settings::GetString("facialRecGlassesCascadeXML");

    if( !FaceCascade.load( face_cascade_loc ) ){ qDebug("ERROR: Face cascade model not loaded : %s",face_cascade_loc.c_str()); };
    if( !QuickFaceCascade.load( quick_face_cascade_loc ) ){ qDebug("ERROR: Quick Face cascade model not loaded : %s",quick_face_cascade_loc.c_str()); };
    if( !EyesCascade.load( eyes_cascade_loc ) ){ qDebug("ERROR: Eyes cascade model not loaded : %s",eyes_cascade_loc.c_str());  };
    if( !GlassesCascade.load( glasses_cascade_loc ) ){ qDebug("ERROR: Glasses cascade model not loaded : %s",glasses_cascade_loc.c_str());  };

    qDebug("FacialRecognitionManager cascades loaded");

    if (Enabled)
        Train();

    qDebug("FacialRecognitionManager inited");
}

void FacialRecognitionManager::Train()
{
    qDebug("Acquirring facerec images");
    vector<Mat> images;
    vector<int> ids;

    vector<User*>::iterator iter;
    for(iter = User::UsersList.begin(); iter != User::UsersList.end(); iter++)
    {
        User* user = (*iter);
        if (user->ImagePaths.size() == 0)
            continue;

        vector<string>::iterator strIter;
        for(strIter = user->ImagePaths.begin(); strIter != user->ImagePaths.end(); strIter++)
        {
            ids.push_back(user->Id + 1);
            string path = (*strIter);
            Mat img = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
            //qDebug("Loaded face image %dx%d: %s", img.size().width, img.size().height, path.c_str());
            images.push_back(img);
        }
    }

    // train the model with your nice collection of pictures
    //qDebug("Beginning facerec training %d profiles", images.size());
    FaceRecModel.train(images, ids);
    qDebug("Facerec training completed");

    Trained = true;
}

void FacialRecognitionManager::SearchForFaces()
{
    // Skip while camera is starting up
    if (RaspiCvCam::FrameCount < 10)
        return;

    // detect faces
    QuickFaceCascade.detectMultiScale(RaspiCvCam::ImageMat, Faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, Size(70,70), Size(150,150));

    FaceCount = Faces.size();
}


int FacialRecognitionManager::RunRecognition()
{
    SearchForFaces();

    if (Enabled &&
        Trained &&
        FaceCount > 0)
    {
        FaceMat = RaspiCvCam::ImageMat(Faces[0]);

        cv::resize(FaceMat, FaceResizedMat, Size(100, 100), 1.0, 1.0, CV_INTER_NN); //INTER_CUBIC);

        if (RaspiCvCam::UseColor)
            cv::cvtColor(FaceResizedMat, FaceResizedMat, CV_RGB2GRAY);

        equalizeHist(FaceResizedMat, FaceResizedMat);

        double confidence = 0.0;
        int prediction = -1;
        FaceRecModel.predict(FaceResizedMat, prediction, confidence);

        int confidenceThreshold = 4500;
        if (prediction > - 1 && confidence > confidenceThreshold)
        {
            qDebug("found userId: %d, with %f confidence", prediction - 1, confidence);
            return prediction - 1;
        }
        qDebug("did not find user! userId: %d, with %f confidence", prediction - 1, confidence);
    }
    // Unknown user
    return -1;
}
