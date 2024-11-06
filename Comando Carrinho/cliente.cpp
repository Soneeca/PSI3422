#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int estado = 0;  // Comando inicial é parado
bool mousePressed = false;  // Estado do mouse

// Função de callback para o mouse
void on_mouse(int event, int c, int l, int, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        if (160 <= l && l < 240 && 0 <= c && c < 80) estado = 1;
        else if (160 <= l && l < 240 && 80 <= c && c < 160) estado = 2;
        else if (160 <= l && l < 240 && 160 <= c && c < 240) estado = 3;
        else if (80 <= l && l < 160 && 0 <= c && c < 80) estado = 4;
        else if (80 <= l && l < 160 && 80 <= c && c < 160) estado = 5;
        else if (80 <= l && l < 160 && 160 <= c && c < 240) estado = 6;
        else if (0 <= l && l < 80 && 0 <= c && c < 80) estado = 7;
        else if (0 <= l && l < 80 && 80 <= c && c < 160) estado = 8;
        else if (0 <= l && l < 80 && 160 <= c && c < 240) estado = 9;
        else estado = 0;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: cliente <IP do servidor>\n";
        return 1;
    }

    CLIENT client(argv[1]);
    Mat_<COR> a, gui(240, 240, COR(128, 128, 128)), tudo;

    namedWindow("janela", WINDOW_NORMAL);
    setMouseCallback("janela", on_mouse);

    while (true) {
        gui.setTo(COR(128, 128, 128));  // Define fundo cinza para o teclado

        client.receiveImgComp(a);

        // Marca a região correspondente ao estado em vermelho
        if (estado == 1) rectangle(gui, Rect(0, 160, 80, 80), Scalar(0, 0, 255), -1);
        else if (estado == 2) rectangle(gui, Rect(80, 160, 80, 80), Scalar(0, 0, 255), -1);
        else if (estado == 3) rectangle(gui, Rect(160, 160, 80, 80), Scalar(0, 0, 255), -1);
        else if (estado == 4) rectangle(gui, Rect(0, 80, 80, 80), Scalar(0, 0, 255), -1);
        else if (estado == 5) rectangle(gui, Rect(80, 80, 80, 80), Scalar(0, 0, 255), -1);
        else if (estado == 6) rectangle(gui, Rect(160, 80, 80, 80), Scalar(0, 0, 255), -1);
        else if (estado == 7) rectangle(gui, Rect(0, 0, 80, 80), Scalar(0, 0, 255), -1);
        else if (estado == 8) rectangle(gui, Rect(80, 0, 80, 80), Scalar(0, 0, 255), -1);
        else if (estado == 9) rectangle(gui, Rect(160, 0, 80, 80), Scalar(0, 0, 255), -1);

        hconcat(gui, a, tudo);
        imshow("janela", tudo);

        client.sendUint(estado);  // Envia o comando atual

        int comando = (signed char)(waitKey(1));  // Captura o comando de saída
        client.sendUint(comando);
        if (comando == 27) break;  // Encerra com a tecla ESC
    }

    destroyAllWindows();
    return 0;
}
