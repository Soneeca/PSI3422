#include "projeto.hpp"
#include <iostream>

using namespace std;

void controlaMotor(uint32_t comando) {
    // Função para controlar o motor com base no comando recebido
    switch (comando) {
        case 1: cout << "Motor: Esquerda Cima" << endl; break;
        case 2: cout << "Motor: Cima" << endl; break;
        case 3: cout << "Motor: Direita Cima" << endl; break;
        case 4: cout << "Motor: Esquerda" << endl; break;
        case 5: cout << "Motor: Parado" << endl; break;
        case 6: cout << "Motor: Direita" << endl; break;
        case 7: cout << "Motor: Esquerda Baixo" << endl; break;
        case 8: cout << "Motor: Baixo" << endl; break;
        case 9: cout << "Motor: Direita Baixo" << endl; break;
        case 0: cout << "Motor: Parado (Sinal de Encerramento)" << endl; break;
        default: cout << "Comando desconhecido!" << endl; break;
    }
}

int main() {
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
