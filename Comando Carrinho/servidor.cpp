#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

using namespace std;
using namespace cv;

// Funções de controle dos motores (setupMotores e controlarMotores) continuam aqui...

int main(int argc, char** argv) {
    SERVER server; // Alterado para usar o construtor padrão

    server.waitConnection();
    setupMotores();

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a câmera." << endl;
        return 1;
    }

    cap.set(CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CAP_PROP_FRAME_HEIGHT, 240);

    Mat_<COR> frame;
    char comando;

    while (true) {
        server.receiveBytes(1, reinterpret_cast<BYTE*>(&comando)); // Modificado para remover `<= 0`
        if (comando == 's') break;

        controlarMotores(comando);

        cap >> frame;
        if (frame.empty()) break;

        server.sendImgComp(frame);
    }

    cout << "Servidor finalizado." << endl;
    return 0;
}
