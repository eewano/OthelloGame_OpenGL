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
#include "GameManager.hpp"

enum class GameState
{
    INIT,
    PLAYING,
    GAMEOVER
};

Input input;
Shader shader;

GLFWwindow* window = nullptr;

std::unique_ptr<GameManager> gameManager;
std::unique_ptr<Panel> panel[BOARD_SIZE][BOARD_SIZE];
std::unique_ptr<Text> textBlack;
std::unique_ptr<Text> textWhite;
std::unique_ptr<Score> scoreBlack01;
std::unique_ptr<Score> scoreBlack10;
std::unique_ptr<Score> scoreWhite01;
std::unique_ptr<Score> scoreWhite10;

GLuint panelId;
GLuint textBlackId;
GLuint textWhiteId;
GLuint scoreId;
GLuint triangleId;

GameState gameState;
bool isGameOver = false;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double posX, double posY);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

int LibraryInit();
void SetUpBmp();
void InitGame();
void PlayingGame();
void GameOver();
void Draw();
void ProcessGameover();



//------------------------------------------------------------------------------------------
int main(int argc, const char * argv[]) {
    
    if(LibraryInit() == true)
    {
        exit(EXIT_FAILURE);
    }
    
    shader.SetUp();
    
    SetUpBmp();
    
    gameManager = std::make_unique<GameManager>();
    textBlack = std::make_unique<Text>(TEXT_BLACK_SIZE, Vec2f{ -0.525f, 0.2f });
    textWhite = std::make_unique<Text>(TEXT_WHITE_SIZE, Vec2f{ 0.525f, 0.2f });
    scoreBlack01 = std::make_unique<Score>(SCORE_SIZE, Vec2f{ -0.475f, 0.05f });
    scoreBlack10 = std::make_unique<Score>(SCORE_SIZE, Vec2f{ -0.55f, 0.05f });
    scoreWhite01 = std::make_unique<Score>(SCORE_SIZE, Vec2f{ 0.575f, 0.05f });
    scoreWhite10 = std::make_unique<Score>(SCORE_SIZE, Vec2f{ 0.5f, 0.05f });
    
    gameState = GameState::INIT;
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0)
    {
        switch(gameState)
        {
            case GameState::INIT:
                InitGame();
                break;
            case GameState::PLAYING:
                PlayingGame();
                break;
            case GameState::GAMEOVER:
                GameOver();
                break;
            default:
                std::cout << "Unknown GameState.\n";
                break;
        }
        
        input.Update();
        input.ResetNow();
        
        Draw();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
//------------------------------------------------------------------------------------------



int LibraryInit()
{
    std::cout << "Current directory is " << GetCurrentWorkingDir().c_str() << ".\n";
    
    if(!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(WINDOW_SIZE.x, WINDOW_SIZE.y, "Tetris", NULL, NULL);
    
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
    
    return 0;
}

void SetUpBmp()
{
    panelId = LoadBmp("Othello.bmp");
    textBlackId = LoadBmp("Black.bmp");
    textWhiteId = LoadBmp("White.bmp");
    scoreId = LoadBmp("Numbers.bmp");
    triangleId = LoadBmp("Triangle.bmp");
}

void InitGame()
{
    gameManager->ResetBoard();
    const auto& score = gameManager->GetScore();
    
    scoreBlack01->Update(score.x);
    scoreBlack10->Update(score.x / 10);
    scoreWhite01->Update(score.y);
    scoreWhite10->Update(score.y / 10);
    
    gameManager->CheckPlayable();
    
    gameState = GameState::PLAYING;
    std::cout << "Game Start.\n";
}

void PlayingGame()
{
    if(input.mMouseStates[GLFW_MOUSE_BUTTON_LEFT].pressed)
    {
        gameManager->SetPanel(input.mCursorPosition);
        
        const auto& score = gameManager->GetScore();
        scoreBlack01->Update(score.x);
        scoreBlack10->Update(score.x / 10);
        scoreWhite01->Update(score.y);
        scoreWhite10->Update(score.y / 10);
        
        if(gameManager->CheckPlayable() == false)
        {
            std::cout << "Can't set Panel. Turn changed.\n";
            gameManager->SwitchTurn();
            
            if(gameManager->CheckPlayable() == false)
            {
                std::cout << "Game Ended. BLACK : " << score.x << ", WHITE : " << score.y << ".\n";
                
                if(score.x > score.y)
                {
                    std::cout << "BLACK Won !\n";
                }
                else if(score.y > score.x)
                {
                    std::cout << "WHITE Won !\n";
                }
                else
                {
                    std::cout << "Draw game.\n";
                }
                
                std::cout << "Press R to restart.\n";
                
                gameState = GameState::GAMEOVER;
                
                return;
            }
        }
    }
    
    if(input.mKeyStates[GLFW_KEY_R].pressed)
    {
        gameState = GameState::INIT;
    }
}

void GameOver()
{
    if(input.mKeyStates[GLFW_KEY_R].pressed)
    {
        gameState = GameState::INIT;
    }
}

void Draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    
    textBlack->Draw(textBlackId);
    textWhite->Draw(textWhiteId);
    scoreBlack01->Draw(scoreId);
    scoreBlack10->Draw(scoreId);
    scoreWhite01->Draw(scoreId);
    scoreWhite10->Draw(scoreId);
    
    gameManager->DrawPanel(panelId);
    gameManager->DrawTriangle(triangleId);
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
        
    }
}

void CursorPosCallback(GLFWwindow* window, double posX, double posY)
{
    static double pastX = 0, pastY = 0;
    if(pastX == posX && pastY == posY)
    {
        return;
    }
    
    //カーソル位置の更新
    input.mCursorPosition = { static_cast<int>(posX), static_cast<int>(posY) };
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
}
