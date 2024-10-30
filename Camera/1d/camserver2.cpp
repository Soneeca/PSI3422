// camserver2.cpp
#include "projeto.hpp"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main() {
    SERVER server;
    server.waitConnection();

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a câmera." << endl;
        return 1;
    }

    cap.set(CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);

    Mat frame;
    char confirm;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        server.sendImgComp(frame);

        server.receiveBytes(1, reinterpret_cast<BYTE*>(&confirm));
        if (confirm == 's') break;  // Encerra o servidor ao receber 's' do cliente
    }
    return 0;
}
