// server6c.cpp
#include "projeto.hpp"
#include <iostream>

int main() {
    SERVER server;
    server.waitConnection();

    server.sendUint(1234567890);

    uint32_t u;
    server.receiveUint(u);
    std::cout << u << std::endl;

    return 0;
}
