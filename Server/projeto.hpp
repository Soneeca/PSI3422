#ifndef PROJETO_HPP
#define PROJETO_HPP

#include "raspberry.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Definindo o tipo BYTE como um sinônimo para uint8_t (inteiro de 8 bits sem sinal)
typedef uint8_t BYTE;

// Função que testa se todos os bytes da memória apontada por buf têm o valor b
bool testaBytes(BYTE* buf, BYTE b, int n) {
    for (int i = 0; i < n; i++) {
        if (buf[i] != b) {
            return false;
        }
    }
    return true;
}

// Classe base DEVICE para encapsular funções comuns de CLIENT e SERVER
class DEVICE {
public:
    // Função para obter o endereço IPv4 ou IPv6
    static void *get_in_addr(struct sockaddr *sa) {
        if (sa->sa_family == AF_INET) {  // IPv4
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);  // IPv6
    }

    // Métodos virtuais puros que serão implementados pelas classes derivadas
    virtual void sendBytes(int nBytesToSend, BYTE *buf) = 0;
    virtual void receiveBytes(int nBytesToReceive, BYTE *buf) = 0;
};

// Classe SERVER herda de DEVICE
class SERVER : public DEVICE {
private:
    const string PORT = "3490";  // Porta do servidor
    int sockfd, new_fd;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];

public:
    SERVER() {
        struct addrinfo hints, *servinfo, *p;
        int rv;
        int yes = 1;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        if ((rv = getaddrinfo(NULL, PORT.c_str(), &hints, &servinfo)) != 0) {
            perror("getaddrinfo");
            exit(1);
        }

        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("server: socket");
                continue;
            }
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
                perror("setsockopt");
                exit(1);
            }
            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("server: bind");
                continue;
            }
            break;
        }

        freeaddrinfo(servinfo);

        if (p == NULL) {
            fprintf(stderr, "server: failed to bind\n");
            exit(1);
        }

        if (listen(sockfd, 10) == -1) {
            perror("listen");
            exit(1);
        }
    }

    ~SERVER() {
        close(new_fd);
    }

    void waitConnection() {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            exit(1);
        }

        // Usar a função get_in_addr para obter o IP do cliente
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
    }

    // Implementação do método sendBytes da classe SERVER
    void sendBytes(int nBytesToSend, BYTE* buf) override {
        int totalSent = 0;
        int bytesLeft = nBytesToSend;
        int n;

        while (totalSent < nBytesToSend) {
            n = send(new_fd, buf + totalSent, bytesLeft, 0);
            if (n == -1) {
                perror("send");
                exit(1);
            }
            totalSent += n;
            bytesLeft -= n;
        }
    }

    // Implementação do método receiveBytes da classe SERVER
    void receiveBytes(int nBytesToReceive, BYTE* buf) override {
        int totalReceived = 0;
        int bytesLeft = nBytesToReceive;
        int n;

        while (totalReceived < nBytesToReceive) {
            n = recv(new_fd, buf + totalReceived, bytesLeft, 0);
            if (n == -1) {
                perror("recv");
                exit(1);
            }
            totalReceived += n;
            bytesLeft -= n;
        }
    }
};

// Classe CLIENT herda de DEVICE
class CLIENT : public DEVICE {
private:
    const string PORT = "3490";  // Porta do cliente
    int sockfd, numbytes;

public:
    CLIENT(string endereco) {
        struct addrinfo hints, *servinfo, *p;
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo(endereco.c_str(), PORT.c_str(), &hints, &servinfo)) != 0) {
            perror("getaddrinfo");
            exit(1);
        }

        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
            }
            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("client: connect");
                continue;
            }
            break;
        }

        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            exit(1);
        }

        freeaddrinfo(servinfo);
    }

    ~CLIENT() {
        close(sockfd);
    }

    // Implementação do método sendBytes da classe CLIENT
    void sendBytes(int nBytesToSend, BYTE* buf) override {
        int totalSent = 0;
        int bytesLeft = nBytesToSend;
        int n;

        while (totalSent < nBytesToSend) {
            n = send(sockfd, buf + totalSent, bytesLeft, 0);
            if (n == -1) {
                perror("send");
                exit(1);
            }
            totalSent += n;
            bytesLeft -= n;
        }
    }

    // Implementação do método receiveBytes da classe CLIENT
    void receiveBytes(int nBytesToReceive, BYTE* buf) override {
        int totalReceived = 0;
        int bytesLeft = nBytesToReceive;
        int n;

        while (totalReceived < nBytesToReceive) {
            n = recv(sockfd, buf + totalReceived, bytesLeft, 0);
            if (n == -1) {
                perror("recv");
                exit(1);
            }
            totalReceived += n;
            bytesLeft -= n;
        }
    }
};

#endif // PROJETO_HPP
