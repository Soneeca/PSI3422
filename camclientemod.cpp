// camclient1.cpp
#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <chrono> // Inclui a biblioteca para medição de tempo

using namespace std;
using namespace cv;

// Função para obter o tempo em segundos desde a época
double timeSinceEpoch() {
    return chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
    ).count() / 1000.0; // Converte para segundos
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: camclient1 <IP do servidor>" << endl;
        return 1;
    }

    CLIENT client(argv[1]);

    Mat_<Vec3b> frame;
    namedWindow("Recebendo Quadro", WINDOW_AUTOSIZE);
    char confirm = '0';

    // Variáveis para controle de FPS
    double t1 = timeSinceEpoch(); // Tempo inicial
    int frameCount = 0; // Contador de quadros recebidos
    double fps = 0.0; // Variável para armazenar FPS

    while (true) {
        client.receiveImg(frame);
        if (frame.empty()) break;

        imshow("Recebendo Quadro", frame);
        frameCount++; // Incrementa a contagem de quadros

        // Verifica se 10 segundos se passaram
        double currentTime = timeSinceEpoch();
        if (currentTime - t1 >= 10.0) {
            double t2 = timeSinceEpoch();
            double t = t2 - t1;
            fps = frameCount / t; // Calcula FPS

            // Imprime a quantidade de quadros e FPS
            printf("Quadros=%d Tempo=%8.2fs FPS=%8.2f\n", frameCount, t, fps);

            // Reseta contadores para o próximo período
            t1 = t2;
            frameCount = 0; // Reseta a contagem de quadros
        }

        int ch = waitKey(30);
        if (ch == 27) {  // Encerra ao pressionar ESC
            confirm = 's';
            client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
            break;
        }

        client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
    }
    destroyAllWindows();
    return 0;
}
