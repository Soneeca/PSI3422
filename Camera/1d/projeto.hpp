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

    // Método para enviar imagem comprimida com JPEG
    void sendImgComp(const Mat& img) {
        vector<BYTE> vb;
        vector<int> params = {IMWRITE_JPEG_QUALITY, 80};  // Configura a qualidade para 80%
        imencode(".jpg", img, vb, params);               // Comprime a imagem

        sendUint(vb.size());                             // Envia o tamanho do vetor comprimido
        sendBytes(vb.size(), vb.data());                 // Envia o vetor de bytes comprimido
    }

    // Método para receber imagem comprimida com JPEG
    void receiveImgComp(Mat& img) {
        uint32_t buf_size;
        receiveUint(buf_size);                           // Recebe o tamanho do vetor comprimido

        vector<BYTE> vb(buf_size);
        receiveBytes(buf_size, vb.data());               // Recebe o vetor de bytes comprimido

        img = imdecode(vb, IMREAD_COLOR);                // Decodifica o vetor para uma imagem
        if (img.empty()) {
            cerr << "Erro ao descomprimir imagem" << endl;
            exit(1);
        }
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
