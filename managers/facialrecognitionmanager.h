#ifndef FACIALRECOGNITIONMANAGER_H
#define FACIALRECOGNITIONMANAGER_H

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "/home/pi/raspberrypi/libfacerec-0.04/include/facerec.hpp"	//<-- modify to your path!

using namespace std;
using namespace cv;

class FacialRecognitionManager
{
public:
    static void Init();
    static void Train();
    static void SearchForFaces();
    static int RunRecognition();

    static bool Enabled;
    static bool Trained;
    static Eigenfaces FaceRecModel;
    static CascadeClassifier FaceCascade;
    static CascadeClassifier QuickFaceCascade;
    static CascadeClassifier EyesCascade;
    static CascadeClassifier GlassesCascade;
    static int FaceCount;
    static vector< Rect_<int> > Faces;
    static Mat FaceMat, FaceResizedMat;
};

#endif // FACIALRECOGNITIONMANAGER_H
