#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

using namespace std;
using namespace cv;

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a câmera." << endl;
        return 1;
    }

    cap.set(CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CAP_PROP_FRAME_HEIGHT, 240);

    wiringPiSetup();
    SERVER server;
    server.waitConnection();

    Mat_<Vec3b> frame;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Envia a imagem comprimida para o cliente
        server.sendImgComp(frame);
    }

    return 0;
}
