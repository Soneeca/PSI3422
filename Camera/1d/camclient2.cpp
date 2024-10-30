#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

using namespace std;
using namespace cv;

double timeSinceEpoch() {
    return chrono::duration_cast<chrono::milliseconds>(
               chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: camclient2 <IP do servidor>" << endl;
        return 1;
    }

    CLIENT client(argv[1]);

    Mat frame;
    namedWindow("Recebendo Quadro", WINDOW_AUTOSIZE);

    double t1 = timeSinceEpoch();
    int ch = -1;
    int i = 0; // Contador de quadros recebidos
    char confirm = '0';

    while (ch < 0) {
        client.receiveImgComp(frame);
        if (frame.empty()) break;

        imshow("Recebendo Quadro", frame);
        ch = waitKey(1);
        i++;  // Incrementa o contador de quadros
        client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
    }

    double t2 = timeSinceEpoch();
    double t = t2 - t1;
    printf("Quadros=%d tempo=%8.2fs fps=%8.2f\n", i, t, i / t);

    destroyAllWindows();
    return 0;
}
