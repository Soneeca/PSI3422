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
        case '3': // Diagonal
