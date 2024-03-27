#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <random>

using namespace std;
using namespace cv;

//FUN��O CAPTURAR FACE
void detectAndDraw(Mat& img, CascadeClassifier& cascade, double scale, bool tryflip);

void initialGame(Mat& img);

//FUN��O DESENHAR MESA DE PING PONG / JOGADOR / CPU / BOLA
void drawTablePingPong(Mat& smallImg, float posyBola);

//FUN��O PARA INICIAR O JOGO
void playGame();

//FUN��O PARA IDENTIFICAR CLIQUE DO MOUSE
void onMouse(int event, int x, int y, int flags, void* userdata);

//FUN��O PARA IDENTIFICAR SE O CLIQUE DO MOUSE � NA POSI��O DO BOT�O
bool isMouseInsideButton(int x, int y, int buttonX, int buttonY, int buttonWidth, int buttonHeight);

//FUN��O DE TRANSPAR�NCIA
void drawTransRect(Mat frame, Scalar color, double alpha, Rect region);

//VARI�VEIS DE CONTROLE
bool closeWindow = false; // FECHAR O JOGO
bool play = false, start = false, colideJogador = false, colideCPU = false; // VARI�VEIS DE CONTROLE DA BOLA
int directionBola = 0; // DIRE��O DA SA�DA DA BOLA -> 1 - PARA BAIXO | 2 - PARA CIMA
int velBola = 1; // VELOCIDADE DA BOLA

//POSI��ES
int posxJogador = 272, posyJogador = 0; // POSI��ES DO JOGADOR
int posxCpu = 272, posyCpu = 0, velCpu = 3;
int posxBola = 0, posyBola = 0; // POSI��ES DA BOLA
int dirBolaX = 0; // DIRE��O DA BOLA NA HORIZONTAL

//PONTUA��O DOS JOGADORES
int scoreJogador = 0;
int scoreCpu = 0;

string nameJogador = "Insira seu Nome";
bool createdGame = false;

//FUN��O MAIN
int main(int argc, const char** argv)
{
    //INICIALIZA��O DOS OBJETOS
    VideoCapture webcam;
    Mat frame;
    bool tryflip;
    CascadeClassifier cascade;
    double scale;

    scale = 0.5; // usar 1, 2, 4.

    if (scale < 1)
        scale = 1;
    tryflip = true;

    if (!cascade.load("C:\\Users\\LailtonVilla\\Desktop\\cvtest2\\haar.xml")) {
        cerr << "ERROR: Could not load classifier cascade: " << "haarcascade_frontalface_default.xml" << endl;
        return -1;
    }

    if(!webcam.open(0)) // para testar com a webcam
    {
        cout << "Capture from camera #0 didn't work" << endl;
        return 1;
    }

    if (webcam.isOpened()) {
        cout << "Capturando video ..." << endl;
        namedWindow("PingPong");

        while (1)
        {
            webcam >> frame;
            if (frame.empty())
                break;

            if (createdGame) {
                detectAndDraw(frame, cascade, scale, tryflip);
            }
            else {
                initialGame(frame);
            }


            char c = (char)waitKey(1);
            if (c == 27 || closeWindow)
                break;
        }
    }

    return 0;
}

void initialGame(Mat& img) {
    Mat smallImg;
    resize(img, smallImg, Size(), 1, 1, INTER_LINEAR_EXACT);

    //FUNDO
    rectangle(smallImg, Point(0, 0), Point(smallImg.cols, smallImg.rows), Scalar(166, 102, 4), -1);

    //VARI�VEIS PARA VERIFICAR A RESOLU��O DA WEBCAM E APLICAR AO CEN�RIO DO JOGO
    int rectWidth = smallImg.cols / 1.8;
    int rectHeight = smallImg.rows / 1.2;
    int x = (smallImg.cols - rectWidth) / 2;
    int y = (smallImg.rows - rectHeight) / 2;

    //MESA
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, y + rectHeight), cv::Scalar(0, 80, 0), -1);
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, y + rectHeight), cv::Scalar(255, 255, 255), 2);
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, (rectHeight / 2) + y), cv::Scalar(255, 255, 255), 2);

    //FUNDO
    drawTransRect(smallImg, Scalar(0, 0, 0), 0.8, Rect(0, 0, smallImg.cols, smallImg.rows));

    //SELE��ES
    rectangle(smallImg, cv::Point(x + 30, y+250), cv::Point(x + (rectWidth * 1.8) / 2, y + (rectHeight * 1.2)/2 + 50), cv::Scalar(255, 255, 255), -1);
    putText(smallImg, nameJogador, Point(x + 40, y + 280), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2);

    //T�TULO
    putText(smallImg, "Ping Pong", Point(x-5, 210), FONT_HERSHEY_COMPLEX_SMALL, 3, Scalar(0, 0, 255), 3);
    putText(smallImg, "Deteccao de Face", Point(x+30, 245), FONT_HERSHEY_COMPLEX_SMALL, 1.5, Scalar(255, 255, 255), 2);

    //BOT�O SAIR DO JOGO
    rectangle(smallImg, Point(560, 10), Point(620, 50), Scalar(0, 0, 255), -1);
    putText(smallImg, "Sair", Point(565, 35), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

    //BOT�O COME�AR
    rectangle(smallImg, cv::Point(x + 120, y + 300), cv::Point(x + (rectWidth * 1.8) / 2 - 100, y + (rectHeight * 1.2) / 2 + 100), cv::Scalar(0, 255, 0), -1);
    putText(smallImg, "Iniciar", Point(x + 130, y + 325), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 0), 1.5);

    //CAPTURAR CLIQUE DO MOUSE
    setMouseCallback("PingPong", onMouse);

    char tecla = (char)waitKey(1);

    if ((tecla >= 65 && tecla <= 90) || (tecla >= 97 && tecla <= 122)) {
        char letra = static_cast<char>(tecla);
        if (nameJogador=="Insira seu Nome") {
            nameJogador = tecla;
        }
        else {
            nameJogador += tecla;
        }
        
    }
    if (tecla==8) {
        nameJogador = "Insira seu Nome";
    }
    if (tecla == 13 && nameJogador!="Insira seu Nome") {
        createdGame = true;
    }

    // Desenha o frame na tela
    imshow("PingPong", smallImg);
}

//FUN��O CAPTURAR FACE
void detectAndDraw(Mat& img, CascadeClassifier& cascade, double scale, bool tryflip)
{
    double t = 0;
    vector<Rect> faces;
    Mat smallImg,gray;
    Scalar color = Scalar(255, 0, 0);

    double fx = 1 / scale;
    resize(img, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT);
    if (tryflip)
        flip(smallImg, smallImg, 1);
    cvtColor(smallImg, gray, COLOR_BGR2GRAY);
    equalizeHist(gray, gray);

    t = (double)getTickCount();
    
    //CHAMAR A FUN��O PARA CONSTRUIR O CEN�RIO DO JOGO
    drawTablePingPong(smallImg, posyBola);

    cascade.detectMultiScale(gray, faces,
        1.3, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        | CASCADE_SCALE_IMAGE,
        Size(40, 40));
    t = (double)getTickCount() - t;
    printf("detection time = %g ms\\n", t * 1000 / getTickFrequency());

    // PERCORRE AS FACES ENCONTRADAS
    for (size_t i = 0; i < faces.size(); i++)
    {
        Rect r = faces[i];
        //rectangle(smallImg, Point(cvRound(r.x), cvRound(r.y)),
            //Point(cvRound((r.x + r.width - 1)), cvRound((r.y + r.height - 1))),
            //color, 3);

        int rectWidth = smallImg.cols / 1.5;
        int x = (smallImg.cols - rectWidth) / 2;

        //CAPTURAR A POSI��O X PARA INDICAR A BARRA DO JOGADOR
        if (cvRound(r.x)>=x && cvRound(r.x)<= rectWidth) {
            posxJogador = cvRound(r.x);
        }
	break;

    }    

    //CHAMA A FUN��O PARA INICIAR O JOGO CASO O BOT�O PLAY TENHA SIDO ACIONADO
    playGame();

    //BOT�O SAIR DO JOGO
    rectangle(smallImg, Point(560, 10), Point(620, 50), Scalar(0, 0, 255), -1);
    putText(smallImg, "Sair", Point(565, 35), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

    //BOT�O PLAY
    if (!play) {
        rectangle(smallImg, Point(10, 10), Point(110, 50), Scalar(0, 255, 0), -1);
        putText(smallImg, " Play", Point(20, 35), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
    }

    //CAPTURAR CLIQUE DO MOUSE
    setMouseCallback("PingPong", onMouse);

    // Desenha o frame na tela
    imshow("PingPong", smallImg);
}

void drawTablePingPong(Mat& smallImg, float posyBola) {
    //INTERFACE DA MESA DE PING PONG
        //FUNDO
        rectangle(smallImg, Point(0, 0), Point(smallImg.cols, smallImg.rows), Scalar(166, 102, 4), -1);

        //VARI�VEIS PARA VERIFICAR A RESOLU��O DA WEBCAM E APLICAR AO CEN�RIO DO JOGO
        int rectWidth = smallImg.cols / 1.8;
        int rectHeight = smallImg.rows / 1.2;
        int x = (smallImg.cols - rectWidth) / 2;
        int y = (smallImg.rows - rectHeight) / 2;

        //MESA
        rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, y + rectHeight), cv::Scalar(0, 80, 0), -1);
        rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, y + rectHeight), cv::Scalar(255, 255, 255), 2);
        rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, (rectHeight/2) + y), cv::Scalar(255, 255, 255), 2);

        //ADVERS�RIO
        rectangle(smallImg, cv::Point(posxCpu, y - 20), cv::Point(posxCpu + 93, y - 10), cv::Scalar(255, 255, 255), -1);
        putText(smallImg, "CPU", Point(posxCpu + 35, y-10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

        //BARRA DO JOGADOR
        rectangle(smallImg, cv::Point(posxJogador, 450), cv::Point(posxJogador + 93, 460), cv::Scalar(255, 255, 255), -1);
        putText(smallImg, nameJogador, Point(posxJogador+20, 460), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

        //BOLA
        rectangle(smallImg, cv::Point(((x + rectWidth / 2) - 8) + posxBola, ((y + rectHeight / 2) - 8) + posyBola), cv::Point(((x + rectWidth / 2) + 8) + posxBola, ((y + rectHeight / 2) + 8) + posyBola), cv::Scalar(255, 255, 255), -1);

        //�REA DOS JOGADORES E DA BOLA
        Rect rect1(((x + rectWidth / 2) - 8) + posxBola, ((y + rectHeight / 2) - 8) + posyBola, 16, 16); // �REA DA BOLA
        Rect rect2(posxJogador, 450, 93, 10); // �REA DA BARRA DO JOGADOR
        Rect rect3(posxCpu, y - 20, 93, 10); // �REA DA BARRA DA CPU

        // CALCULANDO A INTERSE��O DOS ENVOLVIDOS
        Rect intersectBolaJogador = rect1 & rect2; // INTERSE��O DA BOLA COM O JOGADOR
        Rect intersectBolaCpu = rect1 & rect3; // INTERSE��O DA BOLA COM A CPU

        // COLIS�O COM O JOGADOR
        if (intersectBolaJogador.area() > 0 && posyBola > 204) {
            cout << "Colis�o com jogador detectada!" << endl;
            colideJogador = true;
            colideCPU = false;
            start = false;

            // CALCULAR O CENTRO DA BARRA DO JOGADOR
            Point centerRect2(rect2.x + rect2.width / 2, rect2.y + rect2.height / 2);

            // CALCULAR O CENTRO DO RETANGULO DE INTERSE��O
            Point centerIntersect(intersectBolaJogador.x + intersectBolaJogador.width / 2, intersectBolaJogador.y + intersectBolaJogador.height / 2);

            // DISTANCIA HORIZONTAL DA BOLA AT� O CENTRO DO JOGADOR
            int distanceX = centerIntersect.x - centerRect2.x;

            // DETERMINANDO A VELOCIDADE DA BOLA DE ACORDO COM A LOCALIZA��O DE COLIS�O DO JOGADOR
            if (distanceX < 0) {
                // COLIS�O � ESQUERDA DA BARRA
                if (distanceX > -rect2.width / 4) {
                    velBola = 1;
                    dirBolaX = -1;
                }
                else if (distanceX > -rect2.width / 2) {
                    velBola = 2;
                    dirBolaX = -5;
                }
                else if (distanceX > -rect2.width) {
                    velBola = 3;
                    dirBolaX = -10;
                }
                else {
                    velBola = 4;
                    dirBolaX = -15;
                }
            }
            else {
                //  COLIS�O � DIREITA DA BARRA
                if (distanceX < rect2.width / 4) {
                    velBola = 1;
                    dirBolaX = 1;
                }
                else if (distanceX < rect2.width / 2) {
                    velBola = 2;
                    dirBolaX = 5;
                }
                else if (distanceX < rect2.width) {
                    velBola = 3;
                    dirBolaX = 10;
                }
                else {
                    velBola = 4;
                    dirBolaX = 15;
                }
            }

        }

        // COLIS�O COM A BARRA DA CPU
        if (intersectBolaCpu.area() > 0) {
            cout << "Colis�o com CPU detectada!" << endl;
            colideJogador = false;
            colideCPU = true;
            start = false;

            // CALCULAR O CENTRO DA BARRA DA CPU
            Point centerRect3(rect3.x + rect3.width / 2, rect3.y + rect3.height / 2);

            // CALCULAR O CENTRO DO RETANGULO DE INTERSE��O
            Point centerIntersectCpu(intersectBolaCpu.x + intersectBolaCpu.width / 2, intersectBolaCpu.y + intersectBolaCpu.height / 2);

            // DISTANCIA HORIZONTAL DA BOLA AT� O CENTRO DO JOGADOR
            int distanceXCpu = centerIntersectCpu.x - centerRect3.x;

            // DETERMINANDO A VELOCIDADE DA BOLA DE ACORDO COM A LOCALIZA��O DE COLIS�O DO JOGADOR
            if (distanceXCpu < 0) {
                // COLIS�O � ESQUERDA DA BARRA
                if (distanceXCpu > -rect3.width / 4) {
                    velBola = 1;
                    dirBolaX = -1;
                }
                else if (distanceXCpu > -rect3.width / 2) {
                    velBola = 2;
                    dirBolaX = -5;
                }
                else if (distanceXCpu > -rect3.width) {
                    velBola = 3;
                    dirBolaX = -10;
                }
                else {
                    velBola = 4;
                    dirBolaX = -15;
                }
            }
            else {
                //  COLIS�O � DIREITA DA BARRA
                if (distanceXCpu < rect3.width / 4) {
                    velBola = 1;
                    dirBolaX = 1;
                }
                else if (distanceXCpu < rect3.width / 2) {
                    velBola = 2;
                    dirBolaX = 5;
                }
                else if (distanceXCpu < rect3.width) {
                    velBola = 3;
                    dirBolaX = 10;
                }
                else {
                    velBola = 4;
                    dirBolaX = 15;
                }
            }

        }

        // VERIFICAR SE HOUVE COLIS�O NAS LATERAIS
        if (posxBola>160) { // DIREITA
            dirBolaX *= -1;
        }
        else if (posxBola<-160) { // ESQUERDA
            dirBolaX *= -1;
        }

        // VERIFICAR SE A BOLA SAIU POR BAIXO OU POR CIMA E DECRETAR PONTO
        if (posyBola>215) { // PONTO PARA A CPU
            colideJogador = false;
            colideCPU = false;
            start = false;
            velBola = 1;
            dirBolaX = 0;
            scoreCpu++;
        }
        else if (posyBola < - 215){ // PONTO PARA O JOGADOR
            colideJogador = false;
            colideCPU = false;
            start = false;
            velBola = 1;
            dirBolaX = 0;
            scoreJogador++;
        }

        //CONTROLE DA CPU
        if (posyBola<0) {
            if (posxBola>0) {
                if (posxCpu > ((x + rectWidth / 2) - 8) - 46 + posxBola) {
                    posxCpu += -10;
                }
                else {
                    posxCpu += 10;
                }
            }
            else if (posxBola < 0) {
                if (posxCpu < ((x + rectWidth / 2)) - 46 + posxBola) {
                    posxCpu += 10;
                }
                else {
                    posxCpu += -10;
                }
            }
        }
        else {
            if (posxCpu != 272) {
                if (posxCpu>272) {
                    posxCpu += -5;
                }
                else {
                    posxCpu += 5;
                }
            }
            else {
                posxCpu = 272;
            }
        }

        //PLACARES

        rectangle(smallImg, Point(10, (rectHeight / 2) + y - 40), Point(50, (rectHeight / 2) + y), Scalar(0, 0, 0), -1);
        rectangle(smallImg, Point(10, (rectHeight / 2) + y - 40), Point(50, (rectHeight / 2) + y), Scalar(255, 255, 255), 1);
        putText(smallImg, to_string(scoreCpu), Point(22, (rectHeight / 2) + y - 13), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

        rectangle(smallImg, Point(10, (rectHeight / 2) + y), Point(50, (rectHeight / 2) + y + 40), Scalar(0, 0, 0), -1);
        rectangle(smallImg, Point(10, (rectHeight / 2) + y), Point(50, (rectHeight / 2) + y + 40), Scalar(255, 255, 255), 1);
        putText(smallImg, to_string(scoreJogador), Point(22, (rectHeight / 2) + y + 28), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);


}

//FUN��O INICIAR JOGO
void playGame() {
    //INICIOU O JOGO
    if (play && start) {
        if (directionBola == 1) {
            posyBola += 10 * velBola;
        }
        else {
            posyBola -= 10 * velBola;
        }
    }
    //COLIDIU COM O JOGADOR
    else if (play && colideJogador) {
        posyBola -= 10 * velBola;
        posxBola += velBola * dirBolaX;
    }
    //COLIDIU COM A CPU
    else if (play && colideCPU) {
        posyBola += 10 * velBola;
        posxBola += velBola * dirBolaX;
    }
    //REINICIAR JOGO
    else {
        posxBola = 0;
        posyBola = 0;
        posxCpu = 272;
        if (play) {
            waitKey(1000);
        }
        start = true;
    }
}

//FUN��O PARA IDENTIFICAR CLIQUE DO MOUSE
void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        if (isMouseInsideButton(x, y, 520, 10, 620, 50)) {
            cout << "Fechando o jogo..." << endl;
            closeWindow = true; // ENCERRA O JOGO
        }
        if (isMouseInsideButton(x, y, 10, 10, 110, 50) && !play) {
            cout << "Iniciando partida..." << endl;
            play = true; // COME�A O JOGO
            start = true; // VARI�VEL DE IN�CIO

            //ESCOLHER ALEATORIAMENTE A DIRE��O DA BOLA NO IN�CIO DO JOGO
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<int> distribution(1, 2);
            directionBola = distribution(gen);
        }
        if (isMouseInsideButton(x, y, 262, 340, 99.5, 40)) {
            if (nameJogador != "Insira seu Nome") {
                cout << "Inicia game..." << endl;
                createdGame = true; // COME�A O GAME
            }
        }
    }
}

//FUN��O PARA IDENTIFICAR SE O CLIQUE DO MOUSE � NA POSI��O DO BOT�O
bool isMouseInsideButton(int x, int y, int buttonX, int buttonY, int buttonWidth, int buttonHeight) {
    return x >= buttonX && x <= buttonX + buttonWidth && y >= buttonY && y <= buttonY + buttonHeight;
}

//FUN��O DE TRANSPAR�NCIA
void drawTransRect(Mat frame, Scalar color, double alpha, Rect region) {
    Mat roi = frame(region);
    Mat rectImg(roi.size(), CV_8UC3, color);
    addWeighted(rectImg, alpha, roi, 1.0 - alpha, 0, roi);
}