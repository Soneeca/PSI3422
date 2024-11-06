#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

using namespace std;
using namespace cv;

void setupMotores() {
    wiringPiSetup();
    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    digitalWrite(0, LOW);
    digitalWrite(1, LOW);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    if (softPwmCreate(0, 0, 100)) exit(1);
    if (softPwmCreate(1, 0, 100)) exit(1);
    if (softPwmCreate(2, 0, 100)) exit(1);
    if (softPwmCreate(3, 0, 100)) exit(1);
}

void controlarMotores(uint32_t comando) {
    switch (comando) {
        case 1: cout << "Diagonal Cima Esquerda\n"; break;
        case 2: cout << "Frente\n"; break;
        case 3: cout << "Diagonal Cima Direita\n"; break;
        case 4: cout << "Rotação Esquerda\n"; break;
        case 5: cout << "Freio\n"; break;
        case 6: cout << "Rotação Direita\n"; break;
        case 7: cout << "Diagonal Baixo Esquerda\n"; break;
        case 8: cout << "Trás\n"; break;
        case 9: cout << "Diagonal Baixo Direita\n"; break;
        default: cout << "Parado\n"; break;
    }
}

int main() {
    SERVER server;
    server.waitConnection();
    setupMotores();

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a câmera.\n";
        return 1;
    }

    cap.set(CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CAP_PROP_FRAME_HEIGHT, 240);

    Mat_<COR> frame;
    uint32_t comando = 5, ultimo_comando = 0;

    while (true) {
        server.receiveUint(comando);
        if (comando != ultimo_comando) {
            cout << "Comando recebido: " << comando << endl;
            controlarMotores(comando);
            ultimo_comando = comando;
        }

        cap >> frame;
        if (frame.empty()) break;

        server.sendImgComp(frame);
        server.receiveUint(comando);
        if (comando == 27) break;
    }

    cout << "Servidor finalizado.\n";
    return 0;
}
