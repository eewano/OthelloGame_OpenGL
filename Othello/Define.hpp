#ifndef Define_h
#define Define_h

#include "Vec2.hpp"

//現時点では WINDOW_SIZE が 800,600 でないとカーソル位置の取得がおかしくなり、正常にゲームが遊べない
static Vec2i WINDOW_SIZE = { 800, 600 };
static float ASPECT_RATIO = static_cast<float>(WINDOW_SIZE.x) / WINDOW_SIZE.y;
static const int BOARD_SIZE = 8;
static const float BOARD_POS_X = -0.35f;
static const float BOARD_POS_Y = -0.35f;
static constexpr int VERTS_COUNT = 4;

#undef min
#undef max

#endif /* Define_h */
