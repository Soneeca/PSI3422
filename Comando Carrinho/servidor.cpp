#include "projeto.hpp"
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <cstdlib>


using namespace std;

void controlaMotor(uint32_t comando) {
    // Função para controlar o motor com base no comando recebido
    switch (comando) {
        case 1: 
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            delay(50);
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
        break;
        case 2: 
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break;
        case 3: 
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            delay(50);
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break;
        case 4: 
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break;
        case 5: 
            softPwmWrite(0, 100);
            softPwmWrite(1, 100);
            softPwmWrite(2, 100);
            softPwmWrite(3, 100);
            break;
        case 6:
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            break;
        case 7:
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 50);
            softPwmWrite(3, 0);
            delay(50);
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break;
        case 8:
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            break;
        case 9:
            softPwmWrite(0, 50);
            softPwmWrite(1, 0);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
            delay(50);
            softPwmWrite(0, 0);
            softPwmWrite(1, 50);
            softPwmWrite(2, 0);
            softPwmWrite(3, 50);
        case 0: printf("Motor: Parado (Sinal de Encerramento)"); break;
        default: printf("Comando desconhecido!"); break;
    }
}

int main() {

    wiringPiSetup();
    if (softPwmCreate(0, 0, 100)) exit(1);
    if (softPwmCreate(1, 0, 100)) exit(1);
    if (softPwmCreate(2, 0, 100)) exit(1);
    if (softPwmCreate(3, 0, 100)) exit(1);    

    SERVER server;
    server.waitConnection();
    uint32_t comando;

    while (true) {
        server.receiveUint(comando);  // Recebe o comando como uint32_t

        if (comando == 0) break;  // Sinal de parada
        controlaMotor(comando);   // Chama a função para mover o motor
    }

    cout << "Conexão encerrada pelo cliente." << endl;
    return 0;
}
