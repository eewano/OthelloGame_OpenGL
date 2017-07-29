#include <iostream>
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>
#include <linmath.h>
#include <numeric>
#include <memory>

#include "Define.hpp"
#include "Shader.hpp"
#include "Sprite.hpp"
#include "Loader.hpp"
#include "Vec2.hpp"
#include "Panel.hpp"
#include "Text.hpp"
#include "Score.hpp"
#include "Utility.hpp"
#include "Input.hpp"

int pointBlack = 0;
int pointWhite = 0;
int setCount = 0; //石を置いた数(最大64枚まで)
bool isPassed = false; //パスの判定
bool turnChanged = false;

enum class Player
{
    TURN_BLACK,
    TURN_WHITE
};

double cposX, cposY; //マウスのカーソルの座標

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void EnableCheck(); //石を置けるかの判定と、ひっくり返す処理関数
void Init();

Input input;
Player turn = Player::TURN_BLACK;
Shader shader;

std::unique_ptr<Panel> panel[BOARD_SIZE][BOARD_SIZE];

int main(int argc, const char * argv[]) {
    
    std::cout << "Current directory is " << GetCurrentWorkingDir().c_str() << ".\n";
    
    if(!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE.x, WINDOW_SIZE.y, "Othello", NULL, NULL);
    
    if(!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetErrorCallback(ErrorCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    
    glfwMakeContextCurrent(window);
    auto addr = (GLADloadproc)glfwGetProcAddress;
    gladLoadGLLoader(addr);
    glfwSwapInterval(1);
    
    shader.SetUp();
    
    GLuint othelloId = LoadBmp("Othello.bmp");
    GLuint textBlackId = LoadBmp("Black.bmp");
    GLuint textWhiteId = LoadBmp("White.bmp");
    GLuint scoreId = LoadBmp("Numbers.bmp");
    GLuint triangleId = LoadBmp("Triangle.bmp");
    
    auto textBlack = std::make_unique<Text>(TEXT_BLACK_SIZE, Vec2f{ -0.525f, 0.2f });
    auto textWhite = std::make_unique<Text>(TEXT_WHITE_SIZE, Vec2f{ 0.525f, 0.2f });
    auto triangle = std::make_unique<Text>(TRIANGLE_SIZE, Vec2f{ 0.0f, 0.0f });
    auto scoreBlack01 = std::make_unique<Score>(SCORE_SIZE, Vec2f{ -0.475f, 0.05f });
    auto scoreBlack10 = std::make_unique<Score>(SCORE_SIZE, Vec2f{ -0.55f, 0.05f });
    auto scoreWhite01 = std::make_unique<Score>(SCORE_SIZE, Vec2f{ 0.575f, 0.05f });
    auto scoreWhite10 = std::make_unique<Score>(SCORE_SIZE, Vec2f{ 0.5f, 0.05f });
    
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            panel[i][j] = std::make_unique<Panel>(STONE_SIZE, Vec2f{ BOARD_POS_X + j * 0.1f, BOARD_POS_Y + i * 0.1f });
        }
    }
    
    Init();
    triangle->TurnChange(1);
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);
        
        glfwGetCursorPos(window, &cposX, &cposY);
        
        //--------------------現在の黒白の枚数状況--------------------
        
        //常に黒白の個数を初期化しては現枚数を加算し、毎フレームごとにスコアに反映する
        pointBlack = 0;
        pointWhite = 0;
        
        for(int i = 0; i < BOARD_SIZE; i++)
        {
            for(int j = 0; j < BOARD_SIZE; j++)
            {
                //Update関数の引数に直接mStateの値を入れる事でswitch文が不要となる
                panel[i][j]->Update(panel[i][j]->mStatus);
                if(panel[i][j]->mStatus == Panel::Type::BLACK)
                {
                    pointBlack++;
                }
                else if(panel[i][j]->mStatus == Panel::Type::WHITE)
                {
                    pointWhite++;
                }
                panel[i][j]->Draw(othelloId);
            }
        }
        //--------------------現在の黒白の枚数状況--------------------
        
        //--------------------スコアの反映--------------------
        scoreBlack01->Update(pointBlack);
        scoreBlack10->Update(pointBlack / 10);
        scoreWhite01->Update(pointWhite);
        scoreWhite10->Update(pointWhite / 10);
        //--------------------スコアの反映--------------------
        
        if(turnChanged == true)
        {
            switch(turn)
            {
                case Player::TURN_BLACK:
                    triangle->TurnChange(1);
                    turnChanged = false;
                    break;
                case Player::TURN_WHITE:
                    triangle->TurnChange(2);
                    turnChanged = false;
                    break;
            }
        }
        
        textBlack->Draw(textBlackId);
        textWhite->Draw(textWhiteId);
        triangle->Draw(triangleId);
        scoreBlack01->Draw(scoreId);
        scoreBlack10->Draw(scoreId);
        scoreWhite01->Draw(scoreId);
        scoreWhite10->Draw(scoreId);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

void Init()
{
    setCount = 0;
    turn = Player::TURN_BLACK;
    turnChanged = true;
    
    //--------------------1度、盤面を全てNONEにしてから初期石をセットする--------------------
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            panel[i][j]->Update(Panel::Type::NONE);
        }
    }
    panel[4][3]->Update(Panel::Type::WHITE);
    panel[4][4]->Update(Panel::Type::BLACK);
    panel[3][3]->Update(Panel::Type::BLACK);
    panel[3][4]->Update(Panel::Type::WHITE);
    //--------------------1度、盤面を全てNONEにしてから初期石をセットする--------------------
    
    std::cout << "Game Start!\n";
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        input.mKeyStates[key].pressed = true;
    }
    else if(action == GLFW_RELEASE)
    {
        input.mKeyStates[key].pressed = false;
    }
    
    if(input.mKeyStates[GLFW_KEY_R].pressed == true)
    {
        Init();
    }
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    static double pastX = 0, pastY = 0;
    if(pastX == cposX && pastY == cposY)
    {
        return;
    }
    
    //カーソル位置の更新
    input.mCursorPosition = { static_cast<int>(xpos), static_cast<int>(ypos) };
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        input.mMouseStates[button].pressed = true;
    }
    else if(action == GLFW_RELEASE)
    {
        input.mMouseStates[button].pressed = false;
    }
    
    if(input.mMouseStates[GLFW_MOUSE_BUTTON_LEFT].pressed == true)
    {
        for(int i = 0; i < BOARD_SIZE; i++)
        {
            for(int j = 0; j < BOARD_SIZE; j++)
            {
                //マウスのカーソル位置と盤面の位置の一致場所を検出する
                if((j * 60 + 160 < cposX && cposX < (j + 1) * 60 + 160) &&
                   (i * 60 + 60 < cposY && cposY < (i + 1) * 60 + 60))
                {
                    if(setCount < 64)
                    {
                        //盤面がNONEの場所でないと石が置けない
                        if(panel[(BOARD_SIZE - 1) - i][j]->mStatus == Panel::Type::NONE)
                        {
                            switch(turn)
                            {
//黒番----------------------------------------------------------------------------------------------------
                                case Player::TURN_BLACK:
                                {
                                    int changeCount = 0;
                                    int x01 = j;
                                    bool continueFlag = true;
                                    bool changeFlag = false;
                                    //-------------------------左方向の処理-------------------------
                                    while(x01 > 0)
                                    {
                                        x01--;
                                        //確認先に同色の石が見つかったら...
                                        if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::BLACK)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            //今度は同色の石の場所から元の位置に戻り、その間の相手石をひっくり返す
                                            //間に相手石が無ければchangeFlagが立たない
                                            while(x02 < j - 1)
                                            {
                                                x02++;
                                                panel[(BOARD_SIZE - 1) - i][x02]->Update(Panel::Type::BLACK);
                                                changeFlag = true;
                                            }
                                        }
                                        else if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------左方向の処理-------------------------
                                    x01 = j;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------右方向の処理-------------------------
                                    while(x01 < BOARD_SIZE - 1)
                                    {
                                        x01++;
                                        if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::BLACK)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            while(x02 > j + 1)
                                            {
                                                x02--;
                                                panel[(BOARD_SIZE - 1) - i][x02]->Update(Panel::Type::BLACK);
                                                changeFlag = true;
                                            }
                                        }
                                        else if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------右方向の処理-------------------------
                                    int y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------上方向の処理-------------------------
                                    while(y01 < BOARD_SIZE - 1)
                                    {
                                        y01++;
                                        if(panel[y01][j]->mStatus == Panel::Type::BLACK)
                                        {
                                            continueFlag = false;
                                            int y02 = y01;
                                            while(y02 > (BOARD_SIZE - 1) - i + 1)
                                            {
                                                y02--;
                                                panel[y02][j]->Update(Panel::Type::BLACK);
                                                changeFlag = true;
                                            }
                                        }
                                        else if(panel[y01][j]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------上方向の処理-------------------------
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------下方向の処理-------------------------
                                    while(y01 > 0)
                                    {
                                        y01--;
                                        if(panel[y01][j]->mStatus == Panel::Type::BLACK)
                                        {
                                            continueFlag = false;
                                            int y02 = y01;
                                            while(y02 < (BOARD_SIZE - 1) - i - 1)
                                            {
                                                y02++;
                                                panel[y02][j]->Update(Panel::Type::BLACK);
                                                changeFlag = true;
                                            }
                                        }
                                        else if(panel[y01][j]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------下方向の処理-------------------------
                                    x01 = j;
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------左斜め上方向の処理-------------------------
                                    while(x01 > 0 && y01 < BOARD_SIZE - 1)
                                    {
                                        x01--;
                                        y01++;
                                        if(panel[y01][x01]->mStatus == Panel::Type::BLACK)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            int y02 = y01;
                                            while(x02 < j - 1 && y02 > (BOARD_SIZE - 1) - i + 1)
                                            {
                                                x02++;
                                                y02--;
                                                if(panel[y02][x02]->mStatus == Panel::Type::WHITE)
                                                {
                                                    panel[y02][x02]->Update(Panel::Type::BLACK);
                                                    changeFlag = true;
                                                }
                                            }
                                        }
                                        else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------左斜め上方向の処理-------------------------
                                    x01 = j;
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------右斜め下方向の処理-------------------------
                                    while(x01 < BOARD_SIZE - 1 && y01 > 0)
                                    {
                                        x01++;
                                        y01--;
                                        if(panel[y01][x01]->mStatus == Panel::Type::BLACK)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            int y02 = y01;
                                            while(x02 > j + 1 && y02 < (BOARD_SIZE - 1) - i - 1)
                                            {
                                                x02--;
                                                y02++;
                                                if(panel[y02][x02]->mStatus == Panel::Type::WHITE)
                                                {
                                                    panel[y02][x02]->Update(Panel::Type::BLACK);
                                                    changeFlag = true;
                                                }
                                            }
                                        }
                                        else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------右斜め下方向の処理-------------------------
                                    x01 = j;
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------右斜め上方向の処理-------------------------
                                    while(x01 < BOARD_SIZE - 1 && y01 < BOARD_SIZE - 1)
                                    {
                                        x01++;
                                        y01++;
                                        if(panel[y01][x01]->mStatus == Panel::Type::BLACK)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            int y02 = y01;
                                            while(x02 > j + 1 && y02 > (BOARD_SIZE - 1) - i + 1)
                                            {
                                                x02--;
                                                y02--;
                                                if(panel[y02][x02]->mStatus == Panel::Type::WHITE)
                                                {
                                                    panel[y02][x02]->Update(Panel::Type::BLACK);
                                                    changeFlag = true;
                                                }
                                            }
                                        }
                                        else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------右斜め上方向の処理-------------------------
                                    x01 = j;
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------左斜め下方向の処理-------------------------
                                    while(x01 > 0 && y01 > 0)
                                    {
                                        x01--;
                                        y01--;
                                        if(panel[y01][x01]->mStatus == Panel::Type::BLACK)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            int y02 = y01;
                                            while(x02 < j - 1 && y02 < (BOARD_SIZE - 1) - i - 1)
                                            {
                                                x02++;
                                                y02++;
                                                if(panel[y02][x02]->mStatus == Panel::Type::WHITE)
                                                {
                                                    panel[y02][x02]->Update(Panel::Type::BLACK);
                                                    changeFlag = true;
                                                }
                                            }
                                        }
                                        else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------左斜め下方向の処理-------------------------
                                    
                                    //相手石を1個でもひっくり返せる場所であれば、石を置く処理を行なう
                                    //ひっくり返す作業が発生しない(changeCount = 0)場合は、その位置に石は置けない
                                    if(changeCount > 0)
                                    {
                                        setCount++;
                                        panel[(BOARD_SIZE - 1) - i][j]->Update(Panel::Type::BLACK);
                                        turn = Player::TURN_WHITE;
                                        turnChanged = true;
                                        //相手の番になった瞬間に、相手が石を置けるかどうかの判定を行なう
                                        EnableCheck();
                                    }
                                    
                                    break;
                                }
                                    
//白番----------------------------------------------------------------------------------------------------
                                    
                                case Player::TURN_WHITE:
                                {
                                    int changeCount = 0;
                                    int x01 = j;
                                    int continueFlag = true;
                                    bool changeFlag = false;
                                    //-------------------------左方向の処理-------------------------
                                    while(x01 > 0)
                                    {
                                        x01--;
                                        if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::WHITE)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            while(x02 < j - 1)
                                            {
                                                x02++;
                                                panel[(BOARD_SIZE - 1) - i][x02]->Update(Panel::Type::WHITE);
                                                changeFlag = true;
                                            }
                                        }
                                        else if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------左方向の処理-------------------------
                                    x01 = j;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------右方向の処理-------------------------
                                    while(x01 < BOARD_SIZE - 1)
                                    {
                                        x01++;
                                        if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::WHITE)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            while(x02 > j + 1)
                                            {
                                                x02--;
                                                panel[(BOARD_SIZE - 1) - i][x02]->Update(Panel::Type::WHITE);
                                                changeFlag = true;
                                            }
                                        }
                                        else if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------右方向の処理-------------------------
                                    int y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------上方向の処理-------------------------
                                    while(y01 < BOARD_SIZE - 1)
                                    {
                                        y01++;
                                        if(panel[y01][j]->mStatus == Panel::Type::WHITE)
                                        {
                                            continueFlag = false;
                                            int y02 = y01;
                                            while(y02 > (BOARD_SIZE - 1) - i + 1)
                                            {
                                                y02--;
                                                panel[y02][j]->Update(Panel::Type::WHITE);
                                                changeFlag = true;
                                            }
                                        }
                                        else if(panel[y01][j]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------上方向の処理-------------------------
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------下方向の処理-------------------------
                                    while(y01 > 0)
                                    {
                                        y01--;
                                        if(panel[y01][j]->mStatus == Panel::Type::WHITE)
                                        {
                                            continueFlag = false;
                                            int y02 = y01;
                                            while(y02 < (BOARD_SIZE - 1) - i - 1)
                                            {
                                                y02++;
                                                panel[y02][j]->Update(Panel::Type::WHITE);
                                                changeFlag = true;
                                            }
                                        }
                                        else if(panel[y01][j]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------下方向の処理-------------------------
                                    x01 = j;
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------左斜め上方向の処理-------------------------
                                    while(x01 > 0 && y01 < BOARD_SIZE - 1)
                                    {
                                        x01--;
                                        y01++;
                                        if(panel[y01][x01]->mStatus == Panel::Type::WHITE)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            int y02 = y01;
                                            while(x02 < j - 1 && y02 > (BOARD_SIZE - 1) - i + 1)
                                            {
                                                x02++;
                                                y02--;
                                                if(panel[y02][x02]->mStatus == Panel::Type::BLACK)
                                                {
                                                    panel[y02][x02]->Update(Panel::Type::WHITE);
                                                    changeFlag = true;
                                                }
                                            }
                                        }
                                        else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------左斜め上方向の処理-------------------------
                                    x01 = j;
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------右斜め下方向の処理-------------------------
                                    while(x01 < BOARD_SIZE - 1 && y01 > 0)
                                    {
                                        x01++;
                                        y01--;
                                        if(panel[y01][x01]->mStatus == Panel::Type::WHITE)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            int y02 = y01;
                                            while(x02 > j + 1 && y02 < (BOARD_SIZE - 1) - i - 1)
                                            {
                                                x02--;
                                                y02++;
                                                if(panel[y02][x02]->mStatus == Panel::Type::BLACK)
                                                {
                                                    panel[y02][x02]->Update(Panel::Type::WHITE);
                                                    changeFlag = true;
                                                }
                                            }
                                        }
                                        else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------右斜め下方向の処理-------------------------
                                    x01 = j;
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------右斜め上方向の処理-------------------------
                                    while(x01 < BOARD_SIZE - 1 && y01 < BOARD_SIZE - 1)
                                    {
                                        x01++;
                                        y01++;
                                        if(panel[y01][x01]->mStatus == Panel::Type::WHITE)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            int y02 = y01;
                                            while(x02 > j + 1 && y02 > (BOARD_SIZE - 1) - i + 1)
                                            {
                                                x02--;
                                                y02--;
                                                if(panel[y02][x02]->mStatus == Panel::Type::BLACK)
                                                {
                                                    panel[y02][x02]->Update(Panel::Type::WHITE);
                                                    changeFlag = true;
                                                }
                                            }
                                        }
                                        else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------右斜め上方向の処理-------------------------
                                    x01 = j;
                                    y01 = (BOARD_SIZE - 1) - i;
                                    continueFlag = true;
                                    changeFlag = false;
                                    //-------------------------左斜め下方向の処理-------------------------
                                    while(x01 > 0 && y01 > 0)
                                    {
                                        x01--;
                                        y01--;
                                        if(panel[y01][x01]->mStatus == Panel::Type::WHITE)
                                        {
                                            continueFlag = false;
                                            int x02 = x01;
                                            int y02 = y01;
                                            while(x02 < j - 1 && y02 < (BOARD_SIZE - 1) - i - 1)
                                            {
                                                x02++;
                                                y02++;
                                                if(panel[y02][x02]->mStatus == Panel::Type::BLACK)
                                                {
                                                    panel[y02][x02]->Update(Panel::Type::WHITE);
                                                    changeFlag = true;
                                                }
                                            }
                                        }
                                        else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                                        {
                                            break;
                                        }
                                        
                                        if(changeFlag == true)
                                        {
                                            changeCount++;
                                            break;
                                        }
                                        
                                        if(continueFlag == false)
                                        {
                                            break;
                                        }
                                    }
                                    //-------------------------左斜め下方向の処理-------------------------
                                    
                                    if(changeCount > 0)
                                    {
                                        setCount++;
                                        panel[(BOARD_SIZE - 1) - i][j]->Update(Panel::Type::WHITE);
                                        turn = Player::TURN_BLACK;
                                        turnChanged = true;
                                        EnableCheck();
                                    }
                                    
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------

void EnableCheck()
{
    int changeCount = 0;
    
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            if(panel[(BOARD_SIZE - 1) - i][j]->mStatus == Panel::Type::NONE)
            {
                switch(turn)
                {
                    case Player::TURN_BLACK:
                    {
                        int x01 = j;
                        bool continueFlag = true;
                        bool changeFlag = false;
                        //-------------------------左方向の処理-------------------------
                        while(x01 > 0)
                        {
                            x01--;
                            if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::BLACK)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                while(x02 < j - 1)
                                {
                                    x02++;
                                    changeFlag = true;
                                }
                            }
                            else if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------左方向の処理-------------------------
                        x01 = j;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------右方向の処理-------------------------
                        while(x01 < BOARD_SIZE - 1)
                        {
                            x01++;
                            if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::BLACK)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                while(x02 > j + 1)
                                {
                                    x02--;
                                    changeFlag = true;
                                }
                            }
                            else if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------右方向の処理-------------------------
                        int y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------上方向の処理-------------------------
                        while(y01 < BOARD_SIZE - 1)
                        {
                            y01++;
                            if(panel[y01][j]->mStatus == Panel::Type::BLACK)
                            {
                                continueFlag = false;
                                int y02 = y01;
                                while(y02 > (BOARD_SIZE - 1) - i + 1)
                                {
                                    y02--;
                                    changeFlag = true;
                                }
                            }
                            else if(panel[y01][j]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------上方向の処理-------------------------
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------下方向の処理-------------------------
                        while(y01 > 0)
                        {
                            y01--;
                            if(panel[y01][j]->mStatus == Panel::Type::BLACK)
                            {
                                continueFlag = false;
                                int y02 = y01;
                                while(y02 < (BOARD_SIZE - 1) - i - 1)
                                {
                                    y02++;
                                    changeFlag = true;
                                }
                            }
                            else if(panel[y01][j]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------下方向の処理-------------------------
                        x01 = j;
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------左斜め上方向の処理-------------------------
                        while(x01 > 0 && y01 < BOARD_SIZE - 1)
                        {
                            x01--;
                            y01++;
                            if(panel[y01][x01]->mStatus == Panel::Type::BLACK)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                int y02 = y01;
                                while(x02 < j - 1 && y02 > (BOARD_SIZE - 1) - i + 1)
                                {
                                    x02++;
                                    y02--;
                                    if(panel[y02][x02]->mStatus == Panel::Type::WHITE)
                                    {
                                        changeFlag = true;
                                    }
                                }
                            }
                            else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------左斜め上方向の処理-------------------------
                        x01 = j;
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------右斜め下方向の処理-------------------------
                        while(x01 < BOARD_SIZE - 1 && y01 > 0)
                        {
                            x01++;
                            y01--;
                            if(panel[y01][x01]->mStatus == Panel::Type::BLACK)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                int y02 = y01;
                                while(x02 > j + 1 && y02 < (BOARD_SIZE - 1) - i - 1)
                                {
                                    x02--;
                                    y02++;
                                    if(panel[y02][x02]->mStatus == Panel::Type::WHITE)
                                    {
                                        changeFlag = true;
                                    }
                                }
                            }
                            else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------右斜め下方向の処理-------------------------
                        x01 = j;
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------右斜め上方向の処理-------------------------
                        while(x01 < BOARD_SIZE - 1 && y01 < BOARD_SIZE - 1)
                        {
                            x01++;
                            y01++;
                            if(panel[y01][x01]->mStatus == Panel::Type::BLACK)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                int y02 = y01;
                                while(x02 > j + 1 && y02 > (BOARD_SIZE - 1) - i + 1)
                                {
                                    x02--;
                                    y02--;
                                    if(panel[y02][x02]->mStatus == Panel::Type::WHITE)
                                    {
                                        changeFlag = true;
                                    }
                                }
                            }
                            else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------右斜め上方向の処理-------------------------
                        x01 = j;
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------左斜め下方向の処理-------------------------
                        while(x01 > 0 && y01 > 0)
                        {
                            x01--;
                            y01--;
                            if(panel[y01][x01]->mStatus == Panel::Type::BLACK)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                int y02 = y01;
                                while(x02 < j - 1 && y02 < (BOARD_SIZE - 1) - i - 1)
                                {
                                    x02++;
                                    y02++;
                                    if(panel[y02][x02]->mStatus == Panel::Type::WHITE)
                                    {
                                        changeFlag = true;
                                    }
                                }
                            }
                            else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------左斜め下方向の処理-------------------------
                        
                        break;
                    }
                        
//----------------------------------------------------------------------------------------------------
                        
                    case Player::TURN_WHITE:
                    {
                        int x01 = j;
                        int continueFlag = true;
                        bool changeFlag = false;
                        //-------------------------左方向の処理-------------------------
                        while(x01 > 0)
                        {
                            x01--;
                            if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::WHITE)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                while(x02 < j - 1)
                                {
                                    x02++;
                                    changeFlag = true;
                                }
                            }
                            else if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------左方向の処理-------------------------
                        x01 = j;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------右方向の処理-------------------------
                        while(x01 < BOARD_SIZE - 1)
                        {
                            x01++;
                            if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::WHITE)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                while(x02 > j + 1)
                                {
                                    x02--;
                                    changeFlag = true;
                                }
                            }
                            else if(panel[(BOARD_SIZE - 1) - i][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------右方向の処理-------------------------
                        int y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------上方向の処理-------------------------
                        while(y01 < BOARD_SIZE - 1)
                        {
                            y01++;
                            if(panel[y01][j]->mStatus == Panel::Type::WHITE)
                            {
                                continueFlag = false;
                                int y02 = y01;
                                while(y02 > (BOARD_SIZE - 1) - i + 1)
                                {
                                    y02--;
                                    changeFlag = true;
                                }
                            }
                            else if(panel[y01][j]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------上方向の処理-------------------------
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------下方向の処理-------------------------
                        while(y01 > 0)
                        {
                            y01--;
                            if(panel[y01][j]->mStatus == Panel::Type::WHITE)
                            {
                                continueFlag = false;
                                int y02 = y01;
                                while(y02 < (BOARD_SIZE - 1) - i - 1)
                                {
                                    y02++;
                                    changeFlag = true;
                                }
                            }
                            else if(panel[y01][j]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------下方向の処理-------------------------
                        x01 = j;
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------左斜め上方向の処理-------------------------
                        while(x01 > 0 && y01 < BOARD_SIZE - 1)
                        {
                            x01--;
                            y01++;
                            if(panel[y01][x01]->mStatus == Panel::Type::WHITE)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                int y02 = y01;
                                while(x02 < j - 1 && y02 > (BOARD_SIZE - 1) - i + 1)
                                {
                                    x02++;
                                    y02--;
                                    if(panel[y02][x02]->mStatus == Panel::Type::BLACK)
                                    {
                                        changeFlag = true;
                                    }
                                }
                            }
                            else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------左斜め上方向の処理-------------------------
                        x01 = j;
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------右斜め下方向の処理-------------------------
                        while(x01 < BOARD_SIZE - 1 && y01 > 0)
                        {
                            x01++;
                            y01--;
                            if(panel[y01][x01]->mStatus == Panel::Type::WHITE)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                int y02 = y01;
                                while(x02 > j + 1 && y02 < (BOARD_SIZE - 1) - i - 1)
                                {
                                    x02--;
                                    y02++;
                                    if(panel[y02][x02]->mStatus == Panel::Type::BLACK)
                                    {
                                        changeFlag = true;
                                    }
                                }
                            }
                            else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------右斜め下方向の処理-------------------------
                        x01 = j;
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------右斜め上方向の処理-------------------------
                        while(x01 < BOARD_SIZE - 1 && y01 < BOARD_SIZE - 1)
                        {
                            x01++;
                            y01++;
                            if(panel[y01][x01]->mStatus == Panel::Type::WHITE)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                int y02 = y01;
                                while(x02 > j + 1 && y02 > (BOARD_SIZE - 1) - i + 1)
                                {
                                    x02--;
                                    y02--;
                                    if(panel[y02][x02]->mStatus == Panel::Type::BLACK)
                                    {
                                        changeFlag = true;
                                    }
                                }
                            }
                            else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------右斜め上方向の処理-------------------------
                        x01 = j;
                        y01 = (BOARD_SIZE - 1) - i;
                        continueFlag = true;
                        changeFlag = false;
                        //-------------------------左斜め下方向の処理-------------------------
                        while(x01 > 0 && y01 > 0)
                        {
                            x01--;
                            y01--;
                            if(panel[y01][x01]->mStatus == Panel::Type::WHITE)
                            {
                                continueFlag = false;
                                int x02 = x01;
                                int y02 = y01;
                                while(x02 < j - 1 && y02 < (BOARD_SIZE - 1) - i - 1)
                                {
                                    x02++;
                                    y02++;
                                    if(panel[y02][x02]->mStatus == Panel::Type::BLACK)
                                    {
                                        changeFlag = true;
                                    }
                                }
                            }
                            else if(panel[y01][x01]->mStatus == Panel::Type::NONE)
                            {
                                break;
                            }
                            
                            if(changeFlag == true)
                            {
                                changeCount++;
                                break;
                            }
                            
                            if(continueFlag == false)
                            {
                                break;
                            }
                        }
                        //-------------------------左斜め下方向の処理-------------------------
                        
                        break;
                    }
                }
            }
        }
    }
    
    //石が置けなくなった場合の処理
    if(changeCount == 0)
    {
        switch(turn)
        {
            case Player::TURN_BLACK:
                //----------白番がすでにパスしている場合----------
                if(isPassed)
                {
                    std::cout << "BLACK Pass!\n";
                    
                    if(pointBlack > pointWhite)
                    {
                        std::cout << "BLACK Won!\n";
                    }
                    else if(pointBlack < pointWhite)
                    {
                        std::cout << "WHITE Won!\n";
                    }
                    
                    isPassed = false; //ここでゲームが終了
                }
                //----------白番がすでにパスしている場合----------
                else
                {
                    std::cout << "BLACK Pass!\n";
                    isPassed = true;
                    turn = Player::TURN_WHITE;
                }
                break;
                
            case Player::TURN_WHITE:
                //----------黒番がすでにパスしている場合----------
                if(isPassed)
                {
                    std::cout << "WHITE Pass!\n";
                    
                    if(pointBlack > pointWhite)
                    {
                        std::cout << "BLACK Won!\n";
                    }
                    else if(pointBlack < pointWhite)
                    {
                        std::cout << "WHITE Won!\n";
                    }
                    
                    isPassed = false;
                }
                //----------黒番がすでにパスしている場合----------
                else
                {
                    std::cout << "WHITE Pass!\n";
                    isPassed = true;
                    turn = Player::TURN_BLACK;
                }
                break;
        }
    }
    else
    {
        isPassed = false; //どちらかが石が置ける状態であれば、isPassedのフラグを初期化する
    }
    
    if(isPassed)
    {
        EnableCheck();
    }
}
