#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

using namespace std;
using namespace cv;

void setupMotores() {
    wiringPiSetup();
    pinMode(0, OUTPUT); // Ajuste conforme as portas conectadas aos motores
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT); // Ajuste conforme as portas conectadas aos motores
    pinMode(3, OUTPUT);
    digitalWrite (0, LOW) ;
    digitalWrite (1, LOW) ;
    digitalWrite (2, LOW) ;
    digitalWrite (3, LOW) ;
    if (softPwmCreate(0, 0, 100)) exit(1);
    if (softPwmCreate(1, 0, 100)) exit(1);
    if (softPwmCreate(2, 0, 100)) exit(1);
    if (softPwmCreate(3, 0, 100)) exit(1);
}

void controlarMotores(char comando) {
    switch (comando) {
        case '1': // Diagonal Cima Esquerda
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            delay(100);
            softPwmWrite(0, 50); // Motor 0 para frente
            softPwmWrite(1, 0);  // Motor 1 para trás
            softPwmWrite(2, 50); // Motor 2 para frente
            softPwmWrite(3, 0);  // Motor 3 para trás
            break;
        case '2': // Frente
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break; // cert
        case '3': // Diagonal Cima Direita
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            delay(100);
            softPwmWrite(0, 50);  // Motor 0 para trás
            softPwmWrite(1, 0); // Motor 1 para frente
            softPwmWrite(2, 50);  // Motor 2 para trás
            softPwmWrite(3, 0); // Motor 3 para frente
            break;
        case '4': // Direita
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break; // cert
        case '5': // Aceleração Total
            softPwmWrite(0, 100);
            softPwmWrite(1, 100);
            softPwmWrite(2, 100);
            softPwmWrite(3, 100);
            break; // cert
        case '6': // Rotação
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break; // cert
        case '7': // Diagonal Baixo Esquerda
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            delay(100);
            softPwmWrite(0, 0); // Motor 0 para frente
            softPwmWrite(1, 50);  // Motor 1 para trás
            softPwmWrite(2, 0); // Motor 2 para frente
            softPwmWrite(3, 50);  // Motor 3 para trás
            break;
        case '8': // Trás
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break; // cert
        case '9': // Diagonal Baixo Direita
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            delay(100);
            softPwmWrite(0, 0);  // Motor 0 para trás
            softPwmWrite(1, 50);  // Motor 1 para trás
            softPwmWrite(2, 0); // Motor 2 para frente
            softPwmWrite(3, 50); // Motor 3 para frente
            break;
        default: 
            softPwmStop(0); 
            softPwmStop(1); 
            softPwmStop(2); 
            softPwmStop(3); 
            break; // cert
    }
}

int main() {
    SERVER server;
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
        server.receiveBytes(1, reinterpret_cast<BYTE*>(&comando));
        if (comando == 's') break;

        controlarMotores(comando);

        cap >> frame;
        if (frame.empty()) break;

        server.sendImgComp(frame);
    }

    cout << "Servidor finalizado." << endl;
    return 0;
}
