#ifndef Vec2_h
#define Vec2_h

template<typename T>
struct Vec2
{
public:
    T x, y;
    
public:
    Vec2()
    : x(T())
    , y(T())
    {
    }
    
    constexpr Vec2(T a, T b)
    : x(a)
    , y(b)
    {
    }
    
    ~Vec2() = default;
    
    Vec2 operator*(T a)
    {
        return{ x * a, y * a };
    }
    
    Vec2 operator + (const Vec2& a)
    {
        return{ x + a.x, y + a.y };
    }
    
    const Vec2 operator+(const Vec2& a) const
    {
        return { x + a.x, y + a.y };
    }
    
    Vec2& operator+=(const Vec2& a)
    {
        x += a.x;
        y += a.y;
        return *this;
    }
    
    const Vec2& operator+=(const Vec2& a) const
    {
        x += a.x;
        y += a.y;
        return *this;
    }
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

#endif /* Vec2_h */
