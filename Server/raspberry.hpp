#ifndef RASPBERRY_HPP
#define RASPBERRY_HPP

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <string>

using namespace std;

// Função que imprime uma mensagem de erro e encerra o programa
void erro(string mensagem) {
    cerr << "Erro: " << mensagem << endl;
    exit(1);
}

// Macro para depuração que imprime o nome do arquivo e a linha de código
#define xdebug { cout << "Debug: " << __FILE__ << " linha " << __LINE__ << endl; }

// Macro que imprime o nome de uma variável e seu valor
#define xprint(var) { cout << #var << " = " << var << endl; }

// Função que retorna o tempo decorrido desde a época (1 de janeiro de 1970, para sistemas UNIX)
double timeSinceEpoch() {
    using namespace std::chrono;
    return duration_cast<duration<double>>(system_clock::now().time_since_epoch()).count();
}

// Definindo o tipo BYTE como um sinônimo para uint8_t (inteiro de 8 bits sem sinal)
typedef uint8_t BYTE;

#endif // RASPBERRY_HPP
