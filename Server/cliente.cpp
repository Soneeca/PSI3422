// client6c.cpp
#include "projeto.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: client6c <IP do servidor>" << std::endl;
        return 1;
    }

    CLIENT client(argv[1]);

    uint32_t u;
    client.receiveUint(u);
    std::cout << u << std::endl;

    client.sendUint(3333333333);

    return 0;
}
