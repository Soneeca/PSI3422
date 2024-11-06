#include "projeto.hpp"  // Inclua as funções dcReject, somaAbsDois, e converte de raspberry.hpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace cv;

// Função para redimensionar e pré-processar o modelo em várias escalas
vector<Mat_<FLT>> preprocessTemplate(const Mat_<COR>& modelo, int num_scales) {
    vector<Mat_<FLT>> modelos_preprocessados;
    for (int i = 0; i < num_scales; ++i) {
        float scale = 0.1721 * pow(0.7071, i);  // Escala geométrica entre 0.1721 e 0.0473
        Mat_<FLT> T;
        resize(modelo, T, Size(), scale, scale, INTER_NEAREST);  // INTER_NEAREST evita artefatos em "don’t care"
        converte(T, T);
        modelos_preprocessados.push_back(somaAbsDois(dcReject(T, 1.0)));  // "Don't care" no valor 1.0
    }
    return modelos_preprocessados;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Uso: fase3 <video_entrada> <modelo_entrada> <video_saida>" << endl;
        return 1;
    }

    VideoCapture video_in(argv[1]);
    if (!video_in.isOpened()) {
        cerr << "Erro ao abrir o vídeo de entrada." << endl;
        return 1;
    }

    Mat_<COR> modelo = imread(argv[2], 1);
    if (modelo.empty()) {
        cerr << "Erro ao abrir o modelo de placa." << endl;
        return 1;
    }

    int nl = 240, nc = 320;
    VideoWriter video_out(argv[3], VideoWriter::fourcc('X', 'V', 'I', 'D'), 20, Size(nc, nl));

    vector<Mat_<FLT>> modelos_preprocessados = preprocessTemplate(modelo, 10);

    Mat_<COR> frame, output_frame;
    Mat_<FLT> frame_gray;
    auto start = chrono::high_resolution_clock::now();
    int frame_count = 0;

    while (true) {
        video_in >> frame;
        if (frame.empty()) break;

        converte(frame, frame_gray);

        vector<Point> candidatos;
        vector<double> correlacoes;

        for (auto& T : modelos_preprocessados) {
            Mat_<FLT> R;
            matchTemplate(frame_gray, T, R, TM_CCORR);  // Correlação cruzada
            double minVal, maxVal;
            Point minLoc, maxLoc;
            minMaxLoc(R, &minVal, &maxVal, &minLoc, &maxLoc);

            candidatos.push_back(maxLoc);
            correlacoes.push_back(maxVal);
        }

        double melhor_correlacao = -1;
        Point melhor_posicao;
        int melhor_escala = -1;

        for (int i = 0; i < candidatos.size(); ++i) {
            double correlacao_ncc;
            matchTemplate(frame_gray, modelos_preprocessados[i], R, TM_CCOEFF_NORMED);
            minMaxLoc(R, nullptr, &correlacao_ncc, nullptr, nullptr);

            if (correlacao_ncc > melhor_correlacao) {
                melhor_correlacao = correlacao_ncc;
                melhor_posicao = candidatos[i];
                melhor_escala = i;
            }
        }

        if (melhor_correlacao > 0.55) {
            rectangle(frame, Rect(melhor_posicao, Size(modelos_preprocessados[melhor_escala].cols, modelos_preprocessados[melhor_escala].rows)), Scalar(0, 255, 255), 2);
            putText(frame, to_string(melhor_escala), melhor_posicao, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 255), 1);
        }

        video_out << frame;
        frame_count++;
    }

    auto end = chrono::high_resolution_clock::now();
    double duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout << "FPS: " << frame_count / duration << endl;

    video_out.release();
    video_in.release();
    return 0;
}
