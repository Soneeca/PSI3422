// camclient1.cpp
#include "projeto.hpp"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: camclient1 <IP do servidor>" << endl;
        return 1;
    }

    CLIENT client(argv[1]);

    Mat_<Vec3b> frame;
    namedWindow("Recebendo Quadro", WINDOW_AUTOSIZE);
    char confirm = '0';
    while (true) {
        client.receiveImg(frame);
        if (frame.empty()) break;

        imshow("Recebendo Quadro", frame);
        int ch = waitKey(30);
        if (ch == 27) {  // Encerra ao pressionar ESC
            confirm = 's';
            client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
            break;
        }

        client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
    }
    destroyAllWindows();
    return 0;
}
