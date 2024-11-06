#include "projeto.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int estado = 5;  // Estado inicial como "Parado"
bool mousePressed = false;  // Indica se o mouse está pressionado

// Função de callback para o mouse
void on_mouse(int event, int x, int y, int, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        mousePressed = true;
        if (y < 80) {
            estado = (x < 80) ? 1 : (x < 160) ? 2 : 3; // Define direção com base na posição do clique
        } else if (y < 160) {
            estado = (x < 80) ? 4 : (x < 160) ? 5 : 6;
        } else {
            estado = (x < 80) ? 7 : (x < 160) ? 8 : 9;
        }
    } else if (event == EVENT_LBUTTONUP) {
        mousePressed = false;
    }
}

// Desenha o teclado visual com setas e cores
void desenhaTeclado(Mat_<Vec3b> &teclado, int estado_atual) {
    teclado.setTo(Scalar(128, 128, 128));  // Fundo cinza

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int x = j * 80;
            int y = i * 80;
            rectangle(teclado, Rect(x, y, 80, 80), Scalar(200, 200, 200), -1); // Células do teclado

            Scalar cor = (estado_atual == (i * 3 + j + 1)) ? Scalar(0, 0, 255) : Scalar(100, 0, 0);

            // Desenha as setas para os botões
            if (i == 0 && j == 0) { // ↖
                line(teclado, Point(x + 60, y + 60), Point(x + 20, y + 20), cor, 2);
            } else if (i == 0 && j == 1) { // ↑
                line(teclado, Point(x + 40, y + 20), Point(x + 40, y + 60), cor, 2);
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
        cerr << "Uso: cliente2 <IP do servidor>" << endl;
        return 1;
    }

    CLIENT client(argv[1]);  // Configuração do cliente para o IP fornecido
    namedWindow("Controle", WINDOW_NORMAL);
    setMouseCallback("Controle", on_mouse);

    Mat_<Vec3b> teclado(240, 240, Vec3b(128, 128, 128));  // Janela de controle visual do teclado

    while (true) {
        desenhaTeclado(teclado, estado);  // Atualiza o visual do teclado com base no estado atual

        if (mousePressed) {
            client.sendUint(estado);  // Envia o estado como uint32_t sempre que o mouse está pressionado
        }

        imshow("Controle", teclado);
        if (waitKey(1) == 27) {  // Pressione ESC para sair
            client.sendUint(0);  // Envia 0 para sinalizar parada
            break;
        }
    }
    destroyAllWindows();
    return 0;
}
