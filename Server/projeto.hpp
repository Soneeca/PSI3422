#ifndef PROJETO_HPP
#define PROJETO_HPP

// projeto.hpp - incluir nos programas do seu projeto
#include "raspberry.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
typedef uint8_t BYTE;

class SERVER {
public:
    SERVER() {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) perror("Erro ao criar socket");

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(3490);

        if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Erro ao fazer bind");
        }
    }

    ~SERVER() {
        close(new_fd);
        close(sockfd);
    }

    void waitConnection() {
        listen(sockfd, 1);
        socklen_t sin_size = sizeof(client_addr);
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd < 0) perror("Erro ao aceitar conexão");
    }

    void sendBytes(int nBytesToSend, BYTE *buf) {
        int sentBytes = send(new_fd, buf, nBytesToSend, 0);
        if (sentBytes == -1) perror("Erro ao enviar bytes");
    }

    void receiveBytes(int nBytesToReceive, BYTE *buf) {
        int receivedBytes = recv(new_fd, buf, nBytesToReceive, 0);
        if (receivedBytes == -1) perror("Erro ao receber bytes");
    }

    void sendUint(uint32_t m) {
        m = htonl(m); // Converte para big endian
        sendBytes(sizeof(m), reinterpret_cast<BYTE*>(&m));
    }

    void receiveUint(uint32_t& m) {
        receiveBytes(sizeof(m), reinterpret_cast<BYTE*>(&m));
        m = ntohl(m); // Converte para little endian
    }

private:
    int sockfd, new_fd;
    struct sockaddr_in server_addr, client_addr;
};

class CLIENT {
public:
    CLIENT(const string& endereco) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) perror("Erro ao criar socket");

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(3490);

        if (inet_pton(AF_INET, endereco.c_str(), &server_addr.sin_addr) <= 0) {
            perror("Endereço inválido");
        }

        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Erro ao conectar");
        }
    }

    ~CLIENT() {
        close(sockfd);
    }

    void sendBytes(int nBytesToSend, BYTE *buf) {
        int sentBytes = send(sockfd, buf, nBytesToSend, 0);
        if (sentBytes == -1) perror("Erro ao enviar bytes");
    }

    void receiveBytes(int nBytesToReceive, BYTE *buf) {
        int receivedBytes = recv(sockfd, buf, nBytesToReceive, 0);
        if (receivedBytes == -1) perror("Erro ao receber bytes");
    }

    void sendUint(uint32_t m) {
        m = htonl(m); // Converte para big endian
        sendBytes(sizeof(m), reinterpret_cast<BYTE*>(&m));
    }

    void receiveUint(uint32_t& m) {
        receiveBytes(sizeof(m), reinterpret_cast<BYTE*>(&m));
        m = ntohl(m); // Converte para little endian
    }

private:
    int sockfd;
    struct sockaddr_in server_addr;
};

#endif // PROJETO_HPP
