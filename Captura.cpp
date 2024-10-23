#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main() {
 VideoCapture w(0);
 if (!w.isOpened()) {
 fprintf(stderr,"Erro: Abertura de webcam 0.");
 exit(1);
 }
 w.set(CAP_PROP_FRAME_WIDTH,640);
 w.set(CAP_PROP_FRAME_HEIGHT,480);
 VideoWriter vo("nomevideo.avi", CV_FOURCC('X','V','I','D'), 30, Size(640,480));
 Mat_<Vec3b> a;
 namedWindow("janela");
 while (true) {
 w >> a; // get a new frame from camera
 imshow("janela",a);
 int ch=(signed char)(waitKey(30)); // E necessario (signed char)
 if (ch>=0) break;
 vo << a;
 }
}
