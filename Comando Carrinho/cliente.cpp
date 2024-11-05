#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int estado = 0;  // 0 = sem ação, 1-9 = comandos do teclado virtual

// Função de callback para o mouse
void on_mouse(int event, int x, int y, int, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        if (y < 100) {
            if (x < 100) estado = 7;         // Setas para esquerda (↖)
            else if (x < 200) estado = 8;    // Setas para cima (↑)
            else estado = 9;                 // Setas para direita (↗)
        } else if (y < 200) {
            if (x < 100) estado = 4;         // Setas para esquerda (←)
            else if (x < 200) estado = 5;    // Centro (parar) (⏹)
            else estado = 6;                 // Setas para direita (→)
        } else {
            if (x < 100) estado = 1;         // Setas para esquerda para trás (↙)
            else if (x < 200) estado = 2;    // Setas para baixo (↓)
            else estado = 3;                 // Setas para direita para trás (↘)
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

    Mat_<COR> teclado(240, 320, Vec3b(128, 128, 128));
    Mat_<COR> frame, display;
    char confirm = '0';

    while (true) {
        client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
        client.receiveImgComp(frame);
        if (frame.empty()) break;

        // Atualiza o display de acordo com o modo
        if (modo == 't') hconcat(teclado, frame, display);
        else display = frame;

        // Desenha o teclado virtual com setas
        vector<string> labels = {"↙", "↓", "↘", "←", "⏹", "→", "↖", "↑", "↗"};
        for (int i = 0; i < labels.size(); i++) {
            int x = (i % 3) * 100;
            int y = (i / 3) * 100;
            rectangle(teclado, Rect(x, y, 100, 100), Scalar(200, 200, 200), -1);
            putText(teclado, labels[i], Point(x + 30, y + 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        }

        imshow("Controle", display);
        if (!video_out.empty()) vo << display;

        char ch = waitKey(1);
        if (ch == 27) {  // ESC para sair
            confirm = 's';
            client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
            break;
        }

        // Envia o comando correspondente ao botão clicado
        if (estado != 0) {
            confirm = '0' + estado;
            estado = 0;  // Reinicia o estado
        } else {
            confirm = '0';
        }
    }

    if (vo.isOpened()) vo.release();
    destroyAllWindows();
    return 0;
}
