#include "projeto.hpp"
#include "raspberry.hpp"
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

void controlarMotores(char comando) {
    switch (comando) {
        case '1': // Diagonal Cima Esquerda
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            delay(100);
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break;
        case '2': // Frente
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break;
        case '3': // Diagonal Cima Direita
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            delay(100);
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break;
        case '4': // Rotação Esquerda
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break;
        case '5': // Freio
            softPwmWrite(0, 100);
            softPwmWrite(1, 100);
            softPwmWrite(2, 100);
            softPwmWrite(3, 100);
            break;
        case '6': // Rotação Direita
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break;
        case '7': // Diagonal Baixo Esquerda
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            delay(100);
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break;
        case '8': // Trás
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break;
        case '9': // Diagonal Baixo Direita
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            delay(100);
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break;
        default: 
            softPwmStop(0);
            softPwmStop(1);
            softPwmStop(2);
            softPwmStop(3);
            break;
    }
}

int main(int argc, char** argv) {
    SERVER server; // Usa o construtor padrão

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
    char comando = '5';  // Comando inicial para o freio
    char ultimo_comando = '0';  // Armazena o último comando para verificar mudanças

    while (true) {
        server.receiveBytes(1, reinterpret_cast<BYTE*>(&comando));
        if (comando == 's') break;

        // Se o comando mudou, imprime e atualiza o último comando
        if (comando != ultimo_comando) {
            cout << "Novo comando recebido: " << comando << endl;
            ultimo_comando = comando;
        }

        controlarMotores(comando);

        cap >> frame;
        if (frame.empty()) break;

        server.sendImgComp(frame);
    }

    cout << "Servidor finalizado." << endl;
    return 0;
}
