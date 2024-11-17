#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int estado = 5; // Estado inicial como "Parado"
bool mousePressed = false;

void on_mouse(int event, int x, int y, int, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        mousePressed = true;
        if (y < 80) {
            estado = (x < 80) ? 1 : (x < 160) ? 2 : 3;
        } else if (y < 160) {
            estado = (x < 80) ? 4 : (x < 160) ? 5 : 6;
        } else {
            estado = (x < 80) ? 7 : (x < 160) ? 8 : 9;
        }
    } else if (event == EVENT_LBUTTONUP) {
        mousePressed = false;
    }
}

// Função para desenhar o teclado na janela
void desenhaTeclado(Mat_<Vec3b> &teclado, int estado_atual) {
    teclado.setTo(Scalar(128, 128, 128)); // Fundo cinza
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int x = j * 80;
            int y = i * 80;
            rectangle(teclado, Rect(x, y, 80, 80), Scalar(200, 200, 200), -1);
            Scalar cor = (estado_atual == (i * 3 + j + 1)) ? Scalar(0, 0, 255) : Scalar(100, 0, 0);
            circle(teclado, Point(x + 40, y + 40), 10, cor, 2);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Uso: cliente <IP do servidor>" << endl;
        return 1;
    }

    CLIENT client(argv[1]);
    namedWindow("Controle", WINDOW_NORMAL);
    setMouseCallback("Controle", on_mouse);

    Mat_<Vec3b> teclado(240, 240, Vec3b(128, 128, 128));
    Mat_<Vec3b> frame, display;

    while (true) {
        client.receiveImgComp(frame); // Recebe a imagem comprimida do servidor
        if (frame.empty()) break;

        desenhaTeclado(teclado, estado);

        // Concatena o teclado com a imagem recebida
        hconcat(teclado, frame, display);
        imshow("Controle", display);

        if (waitKey(1) == 27) { // Pressione ESC para sair
            client.sendUint(0);
            break;
        }
    }
    destroyAllWindows();
    return 0;
}
