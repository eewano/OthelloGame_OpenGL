#ifndef GameManager_hpp
#define GameManager_hpp

#include "Vec2.hpp"
#include "Panel.hpp"
#include <memory>

static const int BOARD_SIZE = 8;
static constexpr Vec2f BOARD_DOWN_LEFT_POS = { -0.35f, -0.35f };
static constexpr Vec2f BASE_POS = { -0.55f, 0.4f };
static constexpr Vec2i TOP_LEFT_POS = { 32, 24 };
static constexpr Vec2i PANEL_SIZE = { 48, 48 };

class GameManager
{
public:
    GameManager();
    ~GameManager();
    
    void SetPanel(const Vec2i& aPos);
    void ResetBoard();
    bool CheckPlayable();
    void SwitchTurn();
    Vec2i GetScore() const;
    
private:
    size_t TryFlip(const Vec2i& index, const Panel::Type type);
    void PlacePanel(const Vec2i& index, const Panel::Type type);
    bool IsPanelNONE(Panel::Type type) const;
    
private:
    Panel mBoard[BOARD_SIZE][BOARD_SIZE];
    Panel::Type mTurn = Panel::Type::BLACK;
};

#endif /* GameManager_hpp */
