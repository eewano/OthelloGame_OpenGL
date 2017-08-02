#ifndef GameManager_hpp
#define GameManager_hpp

#include <memory>
#include "Vec2.hpp"
#include "Panel.hpp"
#include "Text.hpp"

static const int BOARD_SIZE = 8;
static constexpr Vec2f BASE_POS = { -0.35f, 0.35f };
static constexpr Vec2i TOP_LEFT_POS = { 136, 24 };
static constexpr Vec2i PANEL_SIZE = { 64, 64 };

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
    void DrawPanel(const int id);
    void DrawTriangle(const int id);
    
private:
    size_t TryFlip(const Vec2i& index, const Panel::Type type);
    void PlacePanel(const Vec2i& index, const Panel::Type type);
    bool IsPanelNONE(Panel::Type type) const;
    
private:
    Panel mBoard[BOARD_SIZE][BOARD_SIZE];
    Panel::Type mTurn = Panel::Type::BLACK;
    std::unique_ptr<Text> mNext = std::make_unique<Text>(TRIANGLE_SIZE, Vec2f{ 0.0f, 0.0f });
};

#endif /* GameManager_hpp */
