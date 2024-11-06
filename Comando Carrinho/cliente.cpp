#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int estado = 5;  // Começa com o comando 5

void on_mouse(int event, int x, int y, int, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        if (y < 100) {
            if (x < 100) estado = 7;
            else if (x < 200) estado = 8;
            else estado = 9;
        } else if (y < 200) {
            if (x < 100) estado = 4;
            else if (x < 200) estado = 5;
            else estado = 6;
        } else {
            if (x < 100) estado = 1;
            else if (x < 200) estado = 2;
            else estado = 3;
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
    if (!video_out.empty()) vo.open(video_out, VideoWriter::fourcc('X', 'V', 'I', 'D'), 30, Size(640, 240));

    namedWindow("Controle", WINDOW_NORMAL);
    setMouseCallback("Controle", on_mouse);

    Mat_<COR> teclado(240, 320, Vec3b(128, 128, 128));
    Mat_<COR> frame, display;

    char confirm = '0' + estado;  // Começa enviando o comando 5

    while (true) {
        client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
        client.receiveImgComp(frame);
        
        if (frame.empty()) break;

        // Cria o display com o teclado virtual
        if (modo == 't') hconcat(teclado, frame, display);
        else display = frame;

        // Desenha o teclado virtual
        for (int i = 1; i <= 9; i++) {
            string label = to_string(i);
            int x = ((i - 1) % 3) * 100;
            int y = ((i - 1) / 3) * 100;
            rectangle(teclado, Rect(x, y, 100, 100), Scalar(200, 200, 200), -1);
            putText(teclado, label, Point(x + 30, y + 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        }

        imshow("Controle", display);
        if (!video_out.empty()) vo << display;

        char ch = waitKey(1);
        if (ch == 27) {  // ESC termina o programa
            confirm = 's';
            client.sendBytes(1, reinterpret_cast<BYTE*>(&confirm));
            break;
        }

        // Envia o comando correspondente ao botão clicado
        if (estado != confirm - '0') {
            confirm = '0' + estado;
        }
    }

    if (vo.isOpened()) vo.release();
    destroyAllWindows();
    return 0;
}
