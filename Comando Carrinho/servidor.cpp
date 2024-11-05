#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    SERVER server;
    server.waitConnection();

    VideoCapture cap(0);  // Abre a câmera
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a câmera." << endl;
        return 1;
    }

    cap.set(CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CAP_PROP_FRAME_HEIGHT, 240);

    Mat_<COR> frame;
    char comando;

    while (true) {
        // Recebe o comando do cliente
        server.receiveBytes(1, reinterpret_cast<BYTE*>(&comando));

        if (comando == 's') break;  // Termina se o comando for "s"

        // Captura imagem da câmera
        cap >> frame;
        if (frame.empty()) break;

        // Insere o comando na imagem para visualização
        putText(frame, string(1, comando), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        // Envia a imagem comprimida
        server.sendImgComp(frame);
    }

    cout << "Servidor finalizado." << endl;
    return 0;
}
