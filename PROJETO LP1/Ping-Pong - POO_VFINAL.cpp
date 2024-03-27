//BIBLIOTECAS DO OPENCV
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

//BIBLIOTECAS C++
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cv;

#ifndef PINGPONG_H
#define PINGPONG_H

class PingPong
{
public:
    PingPong();
    void render(Mat& smallImg, int lv);
    void renderStart(Mat& smallImg, int colorJogador, string n, bool save);
    void drawTransRect(Mat frame, Scalar color, double alpha, Rect region);
    bool getCreatedGame();
    void setCreatedGame(bool cg);
    string getNamePlayer();
    Scalar getColorJogador();
private:
    bool createdGame;
    string namePlayer;
    Scalar colorSelectJogador;
};

#endif //PINGPONG_H

//CLASSE COMPONENTES
#ifndef COMPONENTS_H
#define COMPONENTS_H

class Components
{
public:
    Components();
    int getPosX();
    int getPosY();
    void setPosX(int px);
    void setPosY(int py);
    virtual void exibirComponent(Mat& smallImg) = 0;
protected:
    int posx;
    int posy;
};

#endif // COMPONENTS_H

//CLASSE JOGADOR
#ifndef JOGADOR_H
#define JOGADOR_H

class Jogador : public Components
{
public:
    Jogador();
    void exibirComponent(Mat& smallImg);
    string getNameJogador();
    void setName(string name);
    void setScoreJogador(int point);
    int getScoreJogador();
    void setColor(Scalar c);
private:
    string nameJogador;
    int scoreJogador;
    Scalar color;
};

#endif // JOGADOR_H

//CLASSE CPU

#ifndef CPU_H
#define CPU_H


class Cpu : public Components
{
public:
    Cpu();
    void exibirComponent(Mat& smallImg);
    void setScoreCpu(int point);
    int getScoreCpu();
    void controlCpu(int pBallX, int pBallY, Mat& smallImg, bool colideCPU);
    void setVelCpu(int lv);
private:
    int scoreCpu;
    int velCpu;
    Scalar colorCpu;
};

#endif // CPU_H

//CLASSE BOLA
#ifndef BOLA_H
#define BOLA_H

class Bola : public Components
{
public:
    Bola();
    void exibirComponent(Mat& smallImg);
private:
    int dirBolaX;
};

#endif // BOLA_H

//CLASSE APP

#ifndef APP_H
#define APP_H

class App
{
public:
    App();
    void game();
    void detectAndDraw(Mat& img, CascadeClassifier& cascade, double scale, bool tryflip);
    static void callbackMouse(int event, int x, int y, int flags, void* userdata) {
        App* app = static_cast<App*>(userdata);
        app->onMouse(event, x, y, flags);
    };
    void startMouseCallback();
    void onMouse(int event, int x, int y, int flags);
    bool isMouseInsideButton(int x, int y, int buttonX, int buttonY, int buttonWidth, int buttonHeight);
    void playGame();
    void collision();
    void gameOver(Mat& smallImg);
    void salvarJogador();
    void carregarJogador();

private:
    VideoCapture webcam;
    Mat frame;
    bool tryflip;
    CascadeClassifier cascade;
    double scale;

    PingPong pongInterface; // OBJETO PING PONG

    Jogador player[10];
    Cpu npc;
    Bola ball;

    //VARIÁVEIS DE CONTROLE
    bool closeWindow; // FECHAR O JOGO
    bool play, start, colideJogador, colideCPU, gameover, save; // VARIÁVEIS DE CONTROLE DA BOLA
    int directionBola; // DIREÇÃO DA SAÍDA DA BOLA -> 1 - PARA BAIXO | 2 - PARA CIMA
    int velBola1; // VELOCIDADE DA BOLA

    //POSIÇÕES
    int posxJogador; // POSIÇÕES DO JOGADOR
    int posxCpu, velCpu; //POSIÇÕES DA CPU
    int posxBola, posyBola, dirBolaX, velBola; // POSIÇÕES DA BOLA

    //PONTUAÇÃO DOS JOGADORES
    int scoreJogador;
    int scoreCpu;
    int level;
    int setsJogador;
    int setsCpu;

    //COR DA BARRA DO JOGADOR
    int colorJogador = 0;

    bool createdGame;

};

#endif // APP_H

//ARQUIVO CPP DA CLASSE APP

//FUNÇÃO PARA IDENTIFICAR SE O CLIQUE DO MOUSE É NA POSIÇÃO DO BOTÃO
bool App::isMouseInsideButton(int x, int y, int buttonX, int buttonY, int buttonWidth, int buttonHeight) {
    return x >= buttonX && x <= buttonX + buttonWidth && y >= buttonY && y <= buttonY + buttonHeight;
};

//FUNÇÃO PARA IDENTIFICAR CLIQUE DO MOUSE
void App::onMouse(int event, int x, int y, int flags) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        //OBSERVAR CLIQUE SOBRE O BOTÃO DE SAIR
        if (isMouseInsideButton(x, y, 520, 10, 620, 50)) {
            cout << "Fechando o jogo..." << endl;
            closeWindow = true; // ENCERRA O JOGO
        }
        //OBSERVAR CLIQUE SOBRE O BOTÃO PLAY PARA INICIAR PARTIDA
        if (isMouseInsideButton(x, y, 10, 10, 110, 50) && !play && createdGame) {
            cout << "Iniciando partida..." << endl;
            play = true; // COMEÇA O JOGO
            start = true; // SAÍDA DE JOGO - QUANDO NÃO HOUVE NENHUMA COLISÃO AINDA

            //ESCOLHER ALEATORIAMENTE A DIREÇÃO DA BOLA NO INÍCIO DO JOGO
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<int> distribution(1, 2);
            directionBola = distribution(gen);
        }
        //OBSERVAR CLIQUE SOBRE O BOTÃO INICIAR
        if (isMouseInsideButton(x, y, 262, 390, 99.5, 40)) {
            if (!save) {
                player[0].setName(pongInterface.getNamePlayer());
            }
            if (player[0].getNameJogador() != "Insira seu Nome" && player[0].getNameJogador() != "") {
                cout << "Inicia game..." << endl;
                pongInterface.setCreatedGame(true); // COMEÇA O GAME
            }
        }

        //OBSERVAR CLIQUE SOBRE O BOTÃO EXCLUIR
        if (isMouseInsideButton(x, y, 262, 340, 99.5, 40)) {
            if (save && !createdGame) {
                save = false;
                player[0].setName("Insira seu Nome");
                level = 0;
            }
        }


        //OBSERVAR CLIQUE SOBRE AS CORES SELECIONADAS PELO JOGADOR
        if (1) {
            if (isMouseInsideButton(x, y, 222, 355, 30, 25)) {
                colorJogador = 0;
            }
            if (isMouseInsideButton(x, y, 252, 355, 30, 25)) {
                colorJogador = 1;
            }
            if (isMouseInsideButton(x, y, 282, 355, 30, 25)) {
                colorJogador = 2;
            }
            if (isMouseInsideButton(x, y, 312, 355, 30, 25)) {
                colorJogador = 3;
            }
            if (isMouseInsideButton(x, y, 342, 355, 30, 25)) {
                colorJogador = 4;
            }
            if (isMouseInsideButton(x, y, 372, 355, 30, 25)) {
                colorJogador = 5;
            }
        }
        //OBSERVAR CLIQUE SOBRE O BOTÃO REINICIAR
        if (isMouseInsideButton(x, y, 250, 240, 110, 50)) {
            gameover = false;
        }
    }
}

void App::startMouseCallback() {
    setMouseCallback("PingPong", callbackMouse, this);
}

App::App() {
    createdGame = false;
    play = false;
    start = false;
    posyBola = 0;
    dirBolaX = 0;
    velBola = 1;
    scoreCpu = npc.getScoreCpu();
    scoreJogador = player[0].getScoreJogador();
    level = 0;
    setsJogador = 0;
    setsCpu = 0;
    gameover = false;
    save = false;
}

void App::gameOver(Mat& smallImg) {
    if(gameover){
        rectangle(smallImg, Point(0, 0), Point(smallImg.cols, smallImg.rows), Scalar(0, 0, 0), -1);
        putText(smallImg, "GAME OVER", Point(125, 200), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 255, 255), 2);
        
        rectangle(smallImg, Point(250, 240), Point(360, 290), Scalar(0, 255, 0), -1);
        putText(smallImg, "Reiniciar", Point(260, 270), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
    }
}

void App::game() {

    carregarJogador();

    if (!cascade.load("haarcascade_frontalface_default.xml")) {
        cerr << "ERROR: Could not load classifier cascade: " << "haarcascade_frontalface_default.xml" << endl;
    }

    if (!webcam.open(0))
    {
        cout << "Capture from camera #0 didn't work" << endl;
    }

    if (webcam.isOpened()) {
        cout << "Capturando video ..." << endl;
        namedWindow("PingPong");

        while (1)
        {
            //CAPTURAR CLIQUE DO MOUSE
            startMouseCallback();

            webcam >> frame;

            if (frame.empty())
                break;
            if (createdGame) {
                detectAndDraw(frame, cascade, scale, tryflip);
            }
            else {
                pongInterface.renderStart(frame, colorJogador, player[0].getNameJogador(), save);
                
                if (!save) {
                    player[0].setName(pongInterface.getNamePlayer());
                    player[0].setColor(pongInterface.getColorJogador());
                }
                else {
                    switch (colorJogador)
                    {
                    case 0:
                        player[0].setColor(Scalar(255, 255, 255));
                        break;
                    case 1:
                        player[0].setColor(Scalar(0, 255, 0));
                        break;
                    case 2:
                        player[0].setColor(Scalar(255, 0, 0));
                        break;
                    case 3:
                        player[0].setColor(Scalar(0, 0, 255));
                        break;
                    case 4:
                        player[0].setColor(Scalar(0, 255, 255));
                        break;
                    default:
                        player[0].setColor(Scalar(152, 37, 214));
                    }
                }

                if (pongInterface.getCreatedGame()) {
                    createdGame = pongInterface.getCreatedGame();
                    save = true;
                    salvarJogador();
                }

                imshow("PingPong", frame);
            }

            char c = (char)waitKey(1);
            if (c == 27 || closeWindow)
                break;
        }
    }
}

void drawTransparency(Mat frame, Mat transp, int xPos, int yPos) {
    Mat mask;
    vector<Mat> layers;

    split(transp, layers); // seperate channels
    Mat rgb[3] = { layers[0],layers[1],layers[2] };
    mask = layers[3]; // png's alpha channel used as mask
    merge(rgb, 3, transp);  // put together the RGB channels, now transp insn't transparent 
    transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}

//FUNÇÃO CAPTURAR FACE
void App::detectAndDraw(Mat& img, CascadeClassifier& cascade, double scale, bool tryflip)
{

    
    double t = 0;
    vector<Rect> faces;
    Mat smallImg, gray;
    Scalar color = Scalar(255, 0, 0);

    resize(img, smallImg, Size(), 1, 1, INTER_LINEAR_EXACT);
       
    flip(smallImg, smallImg, 1);
    cvtColor(smallImg, gray, COLOR_BGR2GRAY);
    equalizeHist(gray, gray);

    t = (double)getTickCount();

    cascade.detectMultiScale(gray, faces,
        1.3, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        | CASCADE_SCALE_IMAGE,
        Size(40, 40));
    t = (double)getTickCount() - t;
    printf("detection time = %g ms\n", t * 1000 / getTickFrequency());

    //CHAMANDO A INTERFACE DO JOGO
    pongInterface.render(smallImg, level);
    player[0].exibirComponent(smallImg);
    npc.exibirComponent(smallImg);
    ball.exibirComponent(smallImg);
    npc.setVelCpu(level);

    // PERCORRE AS FACES ENCONTRADAS
    for (size_t i = 0; i < faces.size(); i++)
    {
        Rect r = faces[i];
        //rectangle(smallImg, Point(cvRound(r.x), cvRound(r.y)),
           // Point(cvRound((r.x + r.width - 1)), cvRound((r.y + r.height - 1))),
            //color, 3);

        double rectWidth = smallImg.cols / 1.5;
        double x = (smallImg.cols - rectWidth) / 2;

        //CAPTURAR A POSIÇÃO X PARA INDICAR A BARRA DO JOGADOR
        if (cvRound(r.x) >= x && cvRound(r.x) <= rectWidth) {
            player[0].setPosX(cvRound(r.x));
        }
        break;
    }

    static int positionFace = 0;

    if (scoreJogador<1 && level==0 && setsJogador == 0 && setsCpu == 0) {
        Mat tutors = cv::imread("tutors.png", IMREAD_UNCHANGED);
        if (player[0].getPosX()<380 && player[0].getPosX() > 10) {
            drawTransparency(smallImg, tutors, player[0].getPosX()-80, 105);
            positionFace = player[0].getPosX();
            putText(smallImg, "Mova o seu rosto para", Point(player[0].getPosX()-80, 348), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
            putText(smallImg, "movimentar o jogador!", Point(player[0].getPosX()-80, 372), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        }
        else {
            drawTransparency(smallImg, tutors, positionFace-50, 105);
            putText(smallImg, "Mova o seu rosto para", Point(positionFace, 348), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
            putText(smallImg, "movimentar o jogador!", Point(positionFace, 372), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        }
        
    }

    //CHAMAR O CONTROLE DA CPU
    npc.controlCpu(ball.getPosX(), ball.getPosY(), smallImg, colideCPU);

    //CHAMA A FUNÇÃO PARA INICIAR O JOGO CASO O BOTÃO PLAY TENHA SIDO ACIONADO
    collision();
    playGame();

    string fase;

    switch (level)
    {
    case 0:
        fase = "PRIMEIRA FASE";
        break;
    case 1:
        fase = "SEGUNDA FASE";
        break;
    case 2:
        fase = "TERCEIRA FASE";
        break;
    Default:
        fase = "FASE FINAL";
        break;
    }

    //TÍTULO DO LEVEL DE FASE
    if (!play) {
        putText(smallImg, fase, Point(240, 100), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2);
        putText(smallImg, "INICIE O SET", Point(270, 120), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
    }
    else {
        putText(smallImg, fase, Point(15, 28), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
    }

    //PLACARES
    int xEspacamento;

    if (player[0].getScoreJogador() > 9 || npc.getScoreCpu() > 9) {
        xEspacamento = 20;
    }
    else {
        xEspacamento = 0;
    }

    int rectHeight = smallImg.rows / 1.2;
    int y = (smallImg.rows - rectHeight) / 2;

    rectangle(smallImg, Point(10, (rectHeight / 2) + y - 40), Point(90 + xEspacamento, (rectHeight / 2) + y), Scalar(0, 0, 0), -1);
    rectangle(smallImg, Point(10, (rectHeight / 2) + y - 40), Point(90 + xEspacamento, (rectHeight / 2) + y), Scalar(255, 255, 255), 1);
    putText(smallImg, to_string(npc.getScoreCpu()) + "/10", Point(22, (rectHeight / 2) + y - 13), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

    rectangle(smallImg, Point(10, (rectHeight / 2) + y), Point(90 + xEspacamento, (rectHeight / 2) + y + 40), Scalar(0, 0, 0), -1);
    rectangle(smallImg, Point(10, (rectHeight / 2) + y), Point(90 + xEspacamento, (rectHeight / 2) + y + 40), Scalar(255, 255, 255), 1);
    putText(smallImg, to_string(player[0].getScoreJogador()) + "/10", Point(22, (rectHeight / 2) + y + 28), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

    //SETS
    putText(smallImg, "SETS CPU", Point(505, 40), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(255, 255, 255), 1);
    rectangle(smallImg, Point(510, 46), Point(550, 90), Scalar(255, 255, 255), -1);
    rectangle(smallImg, Point(510, 46), Point(550, 90), Scalar(0, 0, 0), 1);
    putText(smallImg, to_string(setsCpu), Point(524, 75), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2);

    putText(smallImg, "SETS JOGADOR", Point(500, 390), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(255, 255, 255), 1);
    rectangle(smallImg, Point(510, 396), Point(550, 440), Scalar(255, 255, 255), -1);
    rectangle(smallImg, Point(510, 396), Point(550, 440), Scalar(0, 0, 0), 1);
    putText(smallImg, to_string(setsJogador), Point(524, 425), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2);

    //BOTÃO PLAY
    if (!play) {
        rectangle(smallImg, Point(10, 10), Point(110, 50), Scalar(0, 255, 0), -1);
        putText(smallImg, " Play", Point(20, 35), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
    }

    //VERIFICAR DERROTA
    gameOver(smallImg);

    //BOTÃO SAIR DO JOGO
    rectangle(smallImg, Point(560, 10), Point(620, 50), Scalar(0, 0, 255), -1);
    putText(smallImg, "Sair", Point(565, 35), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

    // Desenha o frame na tela
    imshow("PingPong", smallImg);
}

//FUNÇÃO DE COLISÕES DA BOLA COM OS COMPONENTES DO JOGO
void App::collision() {
    int rectWidth = frame.cols / 1.8;
    int rectHeight = frame.rows / 1.2;
    int x = (frame.cols - rectWidth) / 2;
    int y = (frame.rows - rectHeight) / 2;

    //ÁREA DOS JOGADORES E DA BOLA
    Rect rect1(((x + rectWidth / 2) - 8) + ball.getPosX(), ((y + rectHeight / 2) - 8) + ball.getPosY(), 16, 16); // ÁREA DA BOLA
    Rect rect2(player[0].getPosX(), 450, 93, 10); // ÁREA DA BARRA DO JOGADOR
    Rect rect3(npc.getPosX()+4, y - 20, 85, 10); // ÁREA DA BARRA DA CPU

    // CALCULANDO A INTERSEÇÃO DOS ENVOLVIDOS
    Rect intersectBolaJogador = rect1 & rect2; // INTERSEÇÃO DA BOLA COM O JOGADOR
    Rect intersectBolaCpu = rect1 & rect3; // INTERSEÇÃO DA BOLA COM A CPU

    // COLISÃO COM O JOGADOR
    if (intersectBolaJogador.area() > 0 && ball.getPosY() > 204) {
        cout << "Colisão com jogador detectada!" << endl;
        colideJogador = true;
        colideCPU = false;
        start = false;

        // CALCULAR O CENTRO DA BARRA DO JOGADOR
        Point centerRect2(rect2.x + rect2.width / 2, rect2.y + rect2.height / 2);

        // CALCULAR O CENTRO DO RETANGULO DE INTERSEÇÃO
        Point centerIntersect(intersectBolaJogador.x + intersectBolaJogador.width / 2, intersectBolaJogador.y + intersectBolaJogador.height / 2);

        // DISTANCIA HORIZONTAL DA BOLA ATÉ O CENTRO DO JOGADOR
        int distanceX = centerIntersect.x - centerRect2.x;

        // DETERMINANDO A VELOCIDADE DA BOLA DE ACORDO COM A LOCALIZAÇÃO DE COLISÃO DO JOGADOR
        if (distanceX < 0) {
            // COLISÃO À ESQUERDA DA BARRA
            if (distanceX > -rect2.width / 4) {
                velBola = 3;
                dirBolaX = -2;
            }
            else if (distanceX > -rect2.width / 2) {
                velBola = 4;
                dirBolaX = -8;
            }
            else if (distanceX > -rect2.width) {
                velBola = 5;
                dirBolaX = -14;
            }
            else {
                velBola = 4;
                dirBolaX = -18;
            }
        }
        else {
            //  COLISÃO À DIREITA DA BARRA
            if (distanceX < rect2.width / 4) {
                velBola = 3;
                dirBolaX = 2;
            }
            else if (distanceX < rect2.width / 2) {
                velBola = 4;
                dirBolaX = 8;
            }
            else if (distanceX < rect2.width) {
                velBola = 5;
                dirBolaX = 14;
            }
            else {
                velBola = 4;
                dirBolaX = 18;
            }
        }

    }

    // COLISÃO COM A BARRA DA CPU
    if (intersectBolaCpu.area() > 0) {
        cout << "Colisão com CPU detectada!" << endl;
        colideJogador = false;
        colideCPU = true;
        start = false;

        // CALCULAR O CENTRO DA BARRA DA CPU
        Point centerRect3(rect3.x + rect3.width / 2, rect3.y + rect3.height / 2);

        // CALCULAR O CENTRO DO RETANGULO DE INTERSEÇÃO
        Point centerIntersectCpu(intersectBolaCpu.x + intersectBolaCpu.width / 2, intersectBolaCpu.y + intersectBolaCpu.height / 2);

        // DISTANCIA HORIZONTAL DA BOLA ATÉ O CENTRO DO JOGADOR
        int distanceXCpu = centerIntersectCpu.x - centerRect3.x;

        // DETERMINANDO A VELOCIDADE DA BOLA DE ACORDO COM A LOCALIZAÇÃO DE COLISÃO DO JOGADOR
        if (distanceXCpu < 0) {
            // COLISÃO À ESQUERDA DA BARRA
            if (distanceXCpu > -rect3.width / 4) {
                velBola = 2;
                dirBolaX = -1;
            }
            else if (distanceXCpu > -rect3.width / 2) {
                velBola = 3;
                dirBolaX = -5;
            }
            else if (distanceXCpu > -rect3.width) {
                velBola = 4;
                dirBolaX = -10;
            }
            else {
                velBola = 5;
                dirBolaX = -15;
            }
        }
        else {
            //  COLISÃO À DIREITA DA BARRA
            if (distanceXCpu < rect3.width / 4) {
                velBola = 2;
                dirBolaX = 1;
            }
            else if (distanceXCpu < rect3.width / 2) {
                velBola = 3;
                dirBolaX = 5;
            }
            else if (distanceXCpu < rect3.width) {
                velBola = 4;
                dirBolaX = 10;
            }
            else {
                velBola = 5;
                dirBolaX = 15;
            }
        }

    }

    // VERIFICAR SE HOUVE COLISÃO NAS LATERAIS
    if (posxBola > 160) { // DIREITA
        dirBolaX *= -1;
    }
    else if (posxBola < -160) { // ESQUERDA
        dirBolaX *= -1;
    }

    // VERIFICAR SE A BOLA SAIU POR BAIXO OU POR CIMA E DECRETAR PONTO
    if (posyBola > 225) { // PONTO PARA A CPU
        colideJogador = false;
        colideCPU = false;
        start = false;
        velBola = 1;
        dirBolaX = 0;
        ball.setPosX(0);
        scoreCpu++;
        npc.setScoreCpu(scoreCpu);

        //ESCOLHER ALEATORIAMENTE A DIREÇÃO DA BOLA NO INÍCIO DO JOGO
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> distribution(1, 2);
        directionBola = distribution(gen);

        if (scoreCpu>=10) {
            setsCpu++;
            scoreCpu = 0;
            scoreJogador = 0;
            npc.setScoreCpu(0);
            player[0].setScoreJogador(0);
            play = false;
        }
    }
    else if (posyBola < -225) { // PONTO PARA O JOGADOR
        colideJogador = false;
        colideCPU = false;
        start = false;
        npc.setPosX(272);
        velBola = 1;
        dirBolaX = 0;
        ball.setPosX(0);
        scoreJogador++;
        player[0].setScoreJogador(scoreJogador);

        //ESCOLHER ALEATORIAMENTE A DIREÇÃO DA BOLA NO INÍCIO DO JOGO
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> distribution(1, 2);
        directionBola = distribution(gen);

        if (scoreJogador >= 10) {
            setsJogador++;
            scoreCpu = 0;
            scoreJogador = 0;
            npc.setScoreCpu(0);
            player[0].setScoreJogador(0);
            play = false;
        }
    }

    //VERIFICAR SE HOUVE VITÓRIA NA PARTIDA
    if ((setsJogador==2&&setsCpu==0)||(setsJogador==3 && setsCpu==1) || (setsJogador == 3 && setsCpu == 2)) {
        level++;
        setsCpu = 0;
        setsJogador = 0;
        salvarJogador();
    }
    else if ((setsJogador == 0 && setsCpu == 2) || (setsJogador == 1 && setsCpu == 3) || (setsJogador == 2 && setsCpu == 3)) {
        level = 0;
        setsCpu = 0;
        setsJogador = 0;
        gameover = true;
        salvarJogador();
    }

}

void App::playGame() {
    //INICIOU O JOGO
    if (play && start) {
        if (directionBola == 1) {
            posyBola += 10 * velBola;
            ball.setPosY(posyBola);
        }
        else {
            posyBola -= 10 * velBola;
            ball.setPosY(posyBola);
        }
    }
    //COLIDIU COM O JOGADOR
    else if (play && colideJogador) {
        posyBola -= 10 * velBola;
        posxBola += velBola * dirBolaX;
        ball.setPosY(posyBola);
        ball.setPosX(posxBola);
    }
    //COLIDIU COM A CPU
    else if (play && colideCPU) {
        posyBola += 10 * velBola;
        posxBola += velBola * dirBolaX;
        ball.setPosY(posyBola);
        ball.setPosX(posxBola);
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
        ball.setPosY(0);
        ball.setPosY(0);
    }
}

void App::salvarJogador() {
    ofstream perfil("perfil.txt");
    perfil << player[0].getNameJogador() << endl;
    perfil << colorJogador << endl;
    perfil << level << endl;
    perfil << save;
    perfil.close(); 
}
void App::carregarJogador() {
    ifstream perfil("perfil.txt");
        string linha;
        int contador = 0;
        while (getline(perfil, linha)) {
            switch (contador) {
                case 0: player[0].setName(linha); break;
                case 1: colorJogador = stoi(linha); break;
                case 2: level = stoi(linha); break;
                case 3: save = stoi(linha); break;
            }
            contador++;
        }
    
    perfil.close();
}

//ARQUIVO CPP DA CLASSE PINGPONG

PingPong::PingPong() {
    createdGame = false;
    namePlayer = "Insira seu Nome";
}

void PingPong::render(Mat& smallImg, int lv) {

    Scalar colorFundo, colorCampo;

    switch (lv)
    {
        case 0:
            colorFundo = Scalar(166, 102, 4);
            colorCampo = Scalar(0, 80, 0);
            break;
        case 1:
            colorFundo = Scalar(25, 0, 51);
            colorCampo = Scalar(0, 0, 180);
            break;
        case 2:
            colorFundo = Scalar(64, 64, 64);
            colorCampo = Scalar(102, 0, 0);
            break;
        default:
            colorFundo = Scalar(0, 0, 0);
            colorCampo = Scalar(102, 102, 0);
            break;
    }

    //INTERFACE DA MESA DE PING PONG

    //FUNDO
    rectangle(smallImg, Point(0, 0), Point(smallImg.cols, smallImg.rows), colorFundo, -1);

    //VARIÁVEIS PARA VERIFICAR A RESOLUÇÃO DA WEBCAM E APLICAR AO CENÁRIO DO JOGO
    int rectWidth = smallImg.cols / 1.8;
    int rectHeight = smallImg.rows / 1.2;
    int x = (smallImg.cols - rectWidth) / 2;
    int y = (smallImg.rows - rectHeight) / 2;

    //MESA
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, y + rectHeight), colorCampo, -1);
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, y + rectHeight), cv::Scalar(255, 255, 255), 2);
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, (rectHeight / 2) + y), cv::Scalar(255, 255, 255), 2);
}

void PingPong::renderStart(Mat& smallImg, int colorJogador, string n, bool save) {
    //INTERFACE DA MESA DE PING PONG

     //FUNDO
    rectangle(smallImg, Point(0, 0), Point(smallImg.cols, smallImg.rows), Scalar(166, 102, 4), -1);

    //VARIÁVEIS PARA VERIFICAR A RESOLUÇÃO DA WEBCAM E APLICAR AO CENÁRIO DO JOGO
    int rectWidth = smallImg.cols / 1.8;
    int rectHeight = smallImg.rows / 1.2;
    int x = (smallImg.cols - rectWidth) / 2;
    int y = (smallImg.rows - rectHeight) / 2;

    //MESA
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, y + rectHeight), cv::Scalar(0, 80, 0), -1);
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, y + rectHeight), cv::Scalar(255, 255, 255), 2);
    rectangle(smallImg, cv::Point(x, y), cv::Point(x + rectWidth, (rectHeight / 2) + y), cv::Scalar(255, 255, 255), 2);

    //BARRA DA CPU (EXIBIÇÃO)
    rectangle(smallImg, cv::Point(272, y - 20), cv::Point(365, y - 10), cv::Scalar(255, 255, 255), -1);
    putText(smallImg, "CPU", Point(307, y - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

    //BARRA DO JOGADOR (EXIBIÇÃO)
    rectangle(smallImg, cv::Point(272, 450), cv::Point(365, 460), cv::Scalar(255, 255, 255), -1);
    putText(smallImg, "Player", Point(292, 460), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

    //FUNDO
    drawTransRect(smallImg, Scalar(0, 0, 0), 0.8, Rect(0, 0, smallImg.cols, smallImg.rows));

    //TÍTULO
    putText(smallImg, "Ping Pong", Point(x - 5, 210), FONT_HERSHEY_COMPLEX_SMALL, 3, Scalar(0, 0, 255), 3);
    putText(smallImg, "Deteccao de Face", Point(x + 30, 245), FONT_HERSHEY_COMPLEX_SMALL, 1.5, Scalar(255, 255, 255), 2);

    //BOTÃO SAIR DO JOGO
    rectangle(smallImg, Point(560, 10), Point(620, 50), Scalar(0, 0, 255), -1);
    putText(smallImg, "Sair", Point(565, 35), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

    if (!save) {
        //CAIXA DE TEXTO
        rectangle(smallImg, cv::Point(x + 30, y + 250), cv::Point(x + (rectWidth * 1.8) / 2, y + (rectHeight * 1.2) / 2 + 50), cv::Scalar(255, 255, 255), -1);
        putText(smallImg, namePlayer, Point(x + 40, y + 280), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2);

        //CORES
        putText(smallImg, "Escolha a cor do seu jogador", Point(x + 68, 345), FONT_HERSHEY_COMPLEX_SMALL, 0.6, Scalar(255, 255, 255), 1);
        rectangle(smallImg, cv::Point(x + 80, 355), cv::Point(x + 110, 380), cv::Scalar(255, 255, 255), -1);
        rectangle(smallImg, cv::Point(x + 110, 355), cv::Point(x + 140, 380), cv::Scalar(0, 255, 0), -1);
        rectangle(smallImg, cv::Point(x + 140, 355), cv::Point(x + 170, 380), cv::Scalar(255, 0, 0), -1);
        rectangle(smallImg, cv::Point(x + 170, 355), cv::Point(x + 200, 380), cv::Scalar(0, 0, 255), -1);
        rectangle(smallImg, cv::Point(x + 200, 355), cv::Point(x + 230, 380), cv::Scalar(0, 255, 255), -1);
        rectangle(smallImg, cv::Point(x + 230, 355), cv::Point(x + 260, 380), cv::Scalar(152, 37, 214), -1);

        switch (colorJogador)
        {
        case 0:
            rectangle(smallImg, cv::Point(x + 80, 355), cv::Point(x + 110, 380), cv::Scalar(0, 165, 255), 2);
            colorSelectJogador = Scalar(255, 255, 255);
            break;
        case 1:
            rectangle(smallImg, cv::Point(x + 110, 355), cv::Point(x + 140, 380), cv::Scalar(0, 165, 255), 2);
            colorSelectJogador = Scalar(0, 255, 0);
            break;
        case 2:
            rectangle(smallImg, cv::Point(x + 140, 355), cv::Point(x + 170, 380), cv::Scalar(0, 165, 255), 2);
            colorSelectJogador = Scalar(255, 0, 0);
            break;
        case 3:
            rectangle(smallImg, cv::Point(x + 170, 355), cv::Point(x + 200, 380), cv::Scalar(0, 165, 255), 2);
            colorSelectJogador = Scalar(0, 0, 255);
            break;
        case 4:
            rectangle(smallImg, cv::Point(x + 200, 355), cv::Point(x + 230, 380), cv::Scalar(0, 165, 255), 2);
            colorSelectJogador = Scalar(0, 255, 255);
            break;
        default:
            rectangle(smallImg, cv::Point(x + 230, 355), cv::Point(x + 260, 380), cv::Scalar(0, 165, 255), 2);
            colorSelectJogador = Scalar(152, 37, 214);
        }
  
        char tecla = (char)waitKey(1);

        if ((tecla >= 65 && tecla <= 90) || (tecla >= 97 && tecla <= 122)) {
            char letra = static_cast<char>(tecla);
            if (namePlayer == "Insira seu Nome") {
                namePlayer = tecla;
            }
            else {
                namePlayer += tecla;
            }

        }
        if (tecla == 8) {
            if (namePlayer == "Insira seu Nome") {
                namePlayer = "";
            }
            else {
                namePlayer.erase(namePlayer.end() - 1);
            }
        }
        if (tecla == 13 && namePlayer != "Insira seu Nome" && namePlayer != "") {
            createdGame = true;
        }
    }
    else {
        //NOME DO PERFIL SALVO
        putText(smallImg, "Perfil Salvo: " + n, Point(x + 10, y + 260), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

        //BOTÃO COMEÇAR (EXIBIÇÃO)
        rectangle(smallImg, cv::Point(x + 120, 340), cv::Point(x + (rectWidth * 1.8) / 2 - 100, 380), cv::Scalar(0, 0, 255), -1);
        putText(smallImg, "Excluir", Point(x + 125, 364), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 0), 1.5);
    }

    //BOTÃO COMEÇAR (EXIBIÇÃO)
    rectangle(smallImg, cv::Point(x + 120, 390), cv::Point(x + (rectWidth * 1.8) / 2 - 100, 430), cv::Scalar(0, 255, 0), -1);
    putText(smallImg, "Iniciar", Point(x + 130, 414), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 0), 1.5);

    
}

bool PingPong::getCreatedGame() {
    return createdGame;
}

void PingPong::setCreatedGame(bool cg) {
    createdGame = cg;
}

string PingPong::getNamePlayer() {
    return namePlayer;
}

Scalar PingPong::getColorJogador() {
    return colorSelectJogador;
}

void PingPong::drawTransRect(Mat frame, Scalar color, double alpha, Rect region) {
    Mat roi = frame(region);
    Mat rectImg(roi.size(), CV_8UC3, color);
    addWeighted(rectImg, alpha, roi, 1.0 - alpha, 0, roi);
}

//ARQUIVO CPP DA CLASSE COMPONENTS

Components::Components(){}

int Components::getPosX() {
    return posx;
}
int Components::getPosY() {
    return posy;
}
void Components::setPosX(int px) {
    posx = px;
}
void Components::setPosY(int py) {
    posy = py;
}

//ARQUIVO CPP DA CLASSE JOGADOR
Jogador::Jogador()
{
    nameJogador = "Insira seu Nome";
    posx = 272;
    scoreJogador = 0;
}

void Jogador::exibirComponent(Mat& smallImg) {
    rectangle(smallImg, cv::Point(posx, 450), cv::Point(posx + 93, 460), color, -1);
    if (color == Scalar(255, 0, 0) || color == Scalar(0, 0, 255) || color == Scalar(152, 37, 214)) {
        putText(smallImg, nameJogador, Point(posx + 20, 460), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
        rectangle(smallImg, cv::Point(posx, 450), cv::Point(posx + 93, 460), cv::Scalar(255, 255, 255), 1);
    }
    else {
        putText(smallImg, nameJogador, Point(posx + 20, 460), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
        rectangle(smallImg, cv::Point(posx, 450), cv::Point(posx + 93, 460), cv::Scalar(0, 0, 0), 1);
    }
    
}

string Jogador::getNameJogador() {
    return nameJogador;
}
void Jogador::setName(string name) {
    nameJogador = name;
}

void Jogador::setScoreJogador(int point) {
    scoreJogador = point;
}
int Jogador::getScoreJogador() {
    return scoreJogador;
}

void Jogador::setColor(Scalar c) {
    color = c;
}

//ARQUIVO CPP DA CLASSE CPU
Cpu::Cpu()
{
    posx = 272;
    scoreCpu = 0;
    velCpu = 1;
    colorCpu = Scalar(255, 255, 255);
}

void Cpu::setVelCpu(int lv) {
    velCpu = lv + 2;
    switch (lv)
    {
        case 0:
            colorCpu = Scalar(255, 255, 255);
            break;
        case 1:
            colorCpu = Scalar(255, 255, 102);
            break;
        case 2:
            colorCpu = Scalar(255, 178, 102);
            break;
        default:
            colorCpu = Scalar(0, 255, 0);
            break;
    }
}

void Cpu::exibirComponent(Mat& smallImg) {
    int rectHeight = smallImg.rows / 1.2;
    int y = (smallImg.rows - rectHeight) / 2;
    rectangle(smallImg, Point(posx, y - 20), Point(posx + 93, y - 10), colorCpu, -1);
    putText(smallImg, "CPU", Point(posx + 35, y - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
}

void Cpu::setScoreCpu(int point) {
    scoreCpu = point;
}
int Cpu::getScoreCpu() {
    return scoreCpu;
}

void Cpu::controlCpu(int pBallX, int pBallY, Mat& smallImg, bool colideCPU) {
    int rectWidth = smallImg.cols / 1.8;
    int rectHeight = smallImg.rows / 1.2;
    int x = (smallImg.cols - rectWidth) / 2;
    int y = (smallImg.rows - rectHeight) / 2;

    if (pBallY < 0 && !colideCPU) {
        if (pBallX > 0) {
            if (posx > ((x + rectWidth / 2) - 8) - 46 + pBallX) {
                posx += -4 * velCpu;
            }
            else {
                posx += 4 * velCpu;
            }
        }
        else if (pBallX < 0) {
            if (posx < ((x + rectWidth / 2)) - 46 + pBallX) {
                posx += 4 * velCpu;
            }
            else {
                posx += -4 * velCpu;
            }
        }
    }
    else {
        if (posx != 272) {
            if (posx > 272) {
                posx += -2 * velCpu;
            }
            else {
                posx += 2 * velCpu;
            }
        }
        else {
            posx = 272;
        }
    }
}

//ARQUIVO CPP DA CLASSE BOLA

Bola::Bola()
{
    posx = 0;
    posy = 0;
    dirBolaX = 0;
}

void Bola::exibirComponent(Mat& smallImg) {
    int rectWidth = smallImg.cols / 1.8;
    int rectHeight = smallImg.rows / 1.2;
    int x = (smallImg.cols - rectWidth) / 2;
    int y = (smallImg.rows - rectHeight) / 2;
    rectangle(smallImg, cv::Point(((x + rectWidth / 2) - 8) + posx, ((y + rectHeight / 2) - 8) + posy), cv::Point(((x + rectWidth / 2) + 8) + posx, ((y + rectHeight / 2) + 8) + posy), cv::Scalar(255, 255, 255), -1);
}


//ARQUIVO MAIN

//DIEGO ARAÚJO DOS SANTOS
//LAILTON VILARIM
//VINYCIUS OLIVEIRA

int main()
{
    App app;

    app.game();

    return 0;
}