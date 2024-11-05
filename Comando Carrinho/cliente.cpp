#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int estado = 0;  // 0 = sem ação, 1-9 = comandos do teclado virtual
bool mousePressed = false;  // Estado do mouse

// Função de callback para o mouse
void on_mouse(int event, int x, int y, int, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        mousePressed = true;
        if (y < 80) {
            if (x < 80) estado = 1;         // ↖
            else if (x < 160) estado = 2;    // ↑
            else estado = 3;                 // ↗
        } else if (y < 160) {
            if (x < 80) estado = 4;         // ↰ (L invertido)
            else if (x < 160) estado = 5;   // ⏹ (parada)
            else estado = 6;                // ↱ (L)
        } else {
            if (x < 80) estado = 7;         // ↙
            else if (x < 160) estado = 8;   // ↓
            else estado = 9;                // ↘
        }
    } else if (event == EVENT_LBUTTONUP) {
        mousePressed = false;
    }
}

void desenhaTeclado(Mat_<COR> &teclado) {
    teclado.setTo(Scalar(128, 128, 128));  // Fundo cinza

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int x = j * 80;
            int y = i * 80;
            rectangle(teclado, Rect(x, y, 80, 80), Scalar(200, 200, 200), -1);

            Scalar cor = (estado == (i * 3 + j + 1) && mousePressed) ? Scalar(0, 0, 255) : Scalar(100, 0, 0);

            // Desenha setas para os botões
            if (i == 0 && j == 0) { // ↖
                line(teclado, Point(x + 60, y + 20), Point(x + 20, y + 60), cor, 2);
            } else if (i == 0 && j == 1) { // ↑
                line(teclado, Point(x + 20, y + 60), Point(x + 60, y + 20), cor, 2);
            } else if (i == 0 && j == 2) { // ↗
                line(teclado, Point(x + 20, y + 60), Point(x + 60, y + 20), cor, 2);
            } else if (i == 1 && j == 0) { // ↰ (L invertido)
                line(teclado, Point(x + 60, y + 40), Point(x + 20, y + 40), cor, 2);
                line(teclado, Point(x + 20, y + 40), Point(x + 20, y + 60), cor, 2);
            } else if (i == 1 && j == 1) { // ⏹ (parada)
                circle(teclado, Point(x + 40, y + 40), 10, cor, 2);
            } else if (i == 1 && j == 2) { // ↱ (L)
                line(teclado, Point(x + 20, y + 40), Point(x + 60, y + 40), cor, 2);
                line(teclado, Point(x + 60, y + 40), Point(x + 60, y + 60), cor, 2);
            } else if (i == 2 && j == 0) { // ↙
                line(teclado, Point(x + 60, y + 20), Point(x + 20, y + 60), cor, 2);
            } else if (i == 2 && j == 1) { // ↓
                line(teclado, Point(x + 40, y + 20), Point(x + 40, y + 60), cor, 2);
            } else if (i == 2 && j == 2) { // ↘
                line(teclado, Point(x + 20, y + 20), Point(x + 60, y + 60), cor, 2);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Uso: cliente1 <IP do servidor> [videosaida.avi] [t/c]" << endl;
        return 1;
    }

    CLIENT client(argv[1]);

    string video_out = (argc > 2) ? argv[2] : "";
    char modo = (argc > 3) ? argv[3][0] : 't';
    VideoWriter vo;
    if (!video_out.empty()) vo.open(video_out, VideoWriter::fourcc('X', 'V', 'I', 'D'), 20, Size(640, 240));

    namedWindow("Controle", WINDOW_NORMAL);
    setMouseCallback("Controle", on_mouse);

    Mat_<COR> teclado(240, 240, Vec3b(128, 128, 128));
    Mat_<COR> frame, display;
    char confirm = '0';

    while (true) {
        client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
        client.receiveImgComp(frame);
        if (frame.empty()) break;

        if (modo == 't') hconcat(teclado, frame, display);
        else display = frame;

        desenhaTeclado(teclado);  // Desenha o teclado com o estado atualizado

        imshow("Controle", display);
        if (!video_out.empty()) vo << display;

        char ch = waitKey(1);
        if (ch == 27) {  // ESC para sair
            confirm = 's';
            client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
            break;
        }

        if (estado != 0) {
            confirm = '0' + estado;
        } else {
            confirm = '0';
        }
    }

    if (vo.isOpened()) vo.release();
    destroyAllWindows();
    return 0;
}
