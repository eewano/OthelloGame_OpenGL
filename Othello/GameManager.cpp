#include "GameManager.hpp"

GameManager::GameManager()
{
    ResetBoard();
}

GameManager::~GameManager()
{
}

void GameManager::SetPanel(const Vec2i& aPos)
{
    if(aPos.x < TOP_LEFT_POS.x ||
       TOP_LEFT_POS.x + PANEL_SIZE.x * BOARD_SIZE < aPos.x ||
       aPos.y < TOP_LEFT_POS.y ||
       TOP_LEFT_POS.y + PANEL_SIZE.y * BOARD_SIZE < aPos.y)
    {
        std::cout << "Click position is out of range.\n";
        return;
    }
    
    Vec2i fixedPosition = aPos - TOP_LEFT_POS;
    Vec2i index = fixedPosition / PANEL_SIZE;
    
    if(IsPanelNONE(mBoard[index.y][index.x].GetType()) == false)
    {
        std::cout << "Index (" << index.x << ", " << index.y << ") already exists.\n";
        return;
    }
    
    if(TryFlip(index, mTurn) > 0)
    {
        PlacePanel(index, mTurn);
        
        SwitchTurn();
    }
    else
    {
        std::cout << "Invalid position.\n";
    }
}

void GameManager::ResetBoard()
{
    for(size_t i = 0; i < BOARD_SIZE; i++)
    {
        for(size_t j = 0; j < BOARD_SIZE; j++)
        {
            mBoard[i][j] = Panel(PANEL_TEX_SIZE, BASE_POS + Vec2f(PANEL_TEX_SIZE.x / 2 * j, -PANEL_TEX_SIZE.y / 2 * i));
        }
    }
    
    mBoard[3][3].SetType(Panel::Type::WHITE);
    mBoard[3][4].SetType(Panel::Type::BLACK);
    mBoard[4][3].SetType(Panel::Type::BLACK);
    mBoard[4][4].SetType(Panel::Type::WHITE);
    
    mTurn = Panel::Type::BLACK;
}

bool GameManager::CheckPlayable()
{
    bool isPlayable = false;
    
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            auto checkCount = TryFlip({ j, i }, mTurn);
            if(checkCount > 0)
            {
                isPlayable = true;
            }
        }
    }
    
    return isPlayable;
}

void GameManager::SwitchTurn()
{
    switch(mTurn)
    {
        case Panel::Type::BLACK:
            mTurn = Panel::Type::WHITE;
            break;
        case Panel::Type::WHITE:
            mTurn = Panel::Type::BLACK;
            break;
        case Panel::Type::NONE:
            default:
            std::cout << "Unknown turn.\n";
            break;
    }
}

Vec2i GameManager::GetScore() const
{
    Vec2i scoreCount;
    
    for(size_t i = 0; i < BOARD_SIZE; i++)
    {
        for(size_t j = 0; j < BOARD_SIZE; j++)
        {
            switch(mBoard[i][j].GetType())
            {
                case Panel::Type::BLACK:
                    ++scoreCount.x;
                    break;
                case Panel::Type::WHITE:
                    ++scoreCount.y;
                    break;
                case Panel::Type::NONE:
                    break;
                    default:
                    std::cout << "Unknown _Type.\n";
                    break;
            }
        }
    }
    return scoreCount;
}

void GameManager::Draw(const int id)
{
    for(size_t i = 0; i < BOARD_SIZE; i++)
    {
        for(size_t j = 0; j < BOARD_SIZE; j++)
        {
            mBoard[i][j].Draw(id);
        }
    }
}

size_t GameManager::TryFlip(const Vec2i &index, const Panel::Type type)
{
    Panel::Type playerType = type;
    size_t totalFlipCount = 0;
    
    auto nowType = mBoard[index.y][index.x].GetType();
    if(IsPanelNONE(nowType) == false)
    {
        return 0;
    }
    
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            Vec2i target = index;
            
            while(true)
            {
                target += { x, y };
                
                if(target.x < 0 ||
                   BOARD_SIZE <= target.x ||
                   target.y < 0 ||
                   BOARD_SIZE <= target.y)
                {
                    break;
                }
                
                auto targetType = mBoard[target.y][target.x].GetType();
                
                if(targetType == playerType)
                {
                    size_t flipCount = 0;
                    
                    target -= { x, y };
                    
                    while(target != index)
                    {
                        ++flipCount;
                        target -= { x, y };
                    }
                    totalFlipCount += flipCount;
                    break;
                }
                else if(IsPanelNONE(targetType) == true)
                {
                    break;
                }
                else
                {
                    //相手の石なら何もせず続ける
                }
            }
        }
    }
    return totalFlipCount;
}

void GameManager::PlacePanel(const Vec2i &index, const Panel::Type type)
{
    if(IsPanelNONE(type) == true)
    {
        std::cout << "Invalid place.\n";
    }
    
    mBoard[index.y][index.x].SetType(type);
    
    Panel::Type playerType = type;
    
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            Vec2i target = index;
            
            while(true)
            {
                target += { x, y };
                
                if(target.x < 0 ||
                   BOARD_SIZE <= target.x ||
                   target.y < 0 ||
                   BOARD_SIZE <= target.y)
                {
                    break;
                }
                
                auto targetType = mBoard[target.y][target.x].GetType();
                
                if(targetType == playerType)
                {
                    while(target != index)
                    {
                        mBoard[target.y][target.x].SetType(playerType);
                        target -= { x, y };
                    }
                    break;
                }
                else if(IsPanelNONE(targetType) == true)
                {
                    break;
                }
                else
                {
                    //相手の石なら何もせず続ける
                }
            }
        }
    }
}

bool GameManager::IsPanelNONE(Panel::Type type) const
{
    switch(type)
    {
        case Panel::Type::BLACK:
        case Panel::Type::WHITE:
            return false;
        case Panel::Type::NONE:
            return true;
        default:
            return false;
    }
}
