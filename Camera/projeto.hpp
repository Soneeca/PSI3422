#ifndef PROJETO_HPP
#define PROJETO_HPP

#include "raspberry.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
typedef uint8_t BYTE;

class DEVICE {
public:
    virtual ~DEVICE() = default;

    virtual void sendBytes(int nBytesToSend, BYTE *buf) = 0;
    virtual void receiveBytes(int nBytesToReceive, BYTE *buf) = 0;

    static void *get_in_addr(struct sockaddr *sa) {
        return sa->sa_family == AF_INET ? 
               (void*)&(((struct sockaddr_in*)sa)->sin_addr) : 
               (void*)&(((struct sockaddr_in6*)sa)->sin6_addr);
    }

    void sendUint(uint32_t m) {
        m = htonl(m);
        sendBytes(sizeof(m), reinterpret_cast<BYTE*>(&m));
    }

    void receiveUint(uint32_t& m) {
        receiveBytes(sizeof(m), reinterpret_cast<BYTE*>(&m));
        m = ntohl(m);
    }

    void sendImg(const Mat_<Vec3b>& img) {
        if (!img.isContinuous()) {
            cerr << "Erro: imagem não está em armazenamento contínuo!" << endl;
            exit(1);
        }
        sendUint(img.rows);
        sendUint(img.cols);
        sendBytes(img.total() * img.elemSize(), img.data);
    }

    void receiveImg(Mat_<Vec3b>& img) {
        uint32_t rows, cols;
        receiveUint(rows);
        receiveUint(cols);
        img.create(rows, cols);
        receiveBytes(img.total() * img.elemSize(), img.data);
    }
};

class SERVER : public DEVICE {
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

    ~SERVER() override {
        std::cout << "server: fechei conexao." << std::endl;
        close(new_fd);
        close(sockfd);
    }

    void waitConnection() {
        std::cout << "server: Esperando conexao..." << std::endl;
        listen(sockfd, 1);
        socklen_t sin_size = sizeof(client_addr);
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd < 0) perror("Erro ao aceitar conexão");

        char client_ip[INET6_ADDRSTRLEN];
        inet_ntop(client_addr.sin_family, get_in_addr((struct sockaddr *)&client_addr), client_ip, sizeof(client_ip));
        std::cout << "server: recebi conexao de " << client_ip << std::endl;
    }

    void sendBytes(int nBytesToSend, BYTE *buf) override {
        int totalSent = 0;
        while (totalSent < nBytesToSend) {
            int sentBytes = send(new_fd, buf + totalSent, nBytesToSend - totalSent, 0);
            if (sentBytes == -1) {
                perror("Erro ao enviar bytes");
                break;
            }
            totalSent += sentBytes;
        }
    }

    void receiveBytes(int nBytesToReceive, BYTE *buf) override {
        int totalReceived = 0;
        while (totalReceived < nBytesToReceive) {
            int receivedBytes = recv(new_fd, buf + totalReceived, nBytesToReceive - totalReceived, 0);
            if (receivedBytes == -1) {
                perror("Erro ao receber bytes");
                break;
            }
            totalReceived += receivedBytes;
        }
    }

private:
    int sockfd, new_fd;
    struct sockaddr_in server_addr, client_addr;
};

class CLIENT : public DEVICE {
public:
    CLIENT(const string& endereco) {
        std::cout << "client: conectando a " << endereco << std::endl;

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

    ~CLIENT() override {
        close(sockfd);
    }

    void sendBytes(int nBytesToSend, BYTE *buf) override {
        int totalSent = 0;
        while (totalSent < nBytesToSend) {
            int sentBytes = send(sockfd, buf + totalSent, nBytesToSend - totalSent, 0);
            if (sentBytes == -1) {
                perror("Erro ao enviar bytes");
                break;
            }
            totalSent += sentBytes;
        }
    }

    void receiveBytes(int nBytesToReceive, BYTE *buf) override {
        int totalReceived = 0;
        while (totalReceived < nBytesToReceive) {
            int receivedBytes = recv(sockfd, buf + totalReceived, nBytesToReceive - totalReceived, 0);
            if (receivedBytes == -1) {
                perror("Erro ao receber bytes");
                break;
            }
            totalReceived += receivedBytes;
        }
    }

private:
    int sockfd;
    struct sockaddr_in server_addr;
};

#endif // PROJETO_HPP
