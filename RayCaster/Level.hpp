//
//  Level.hpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#ifndef Level_hpp
#define Level_hpp

#include <stdio.h>
#include <array>
#include <vector>

const auto MAP_SIDE = 6;

class LimitedAngle
{
public:
    LimitedAngle(double value):
        m_value(value)
    {}
    
    LimitedAngle operator+(double value)
    {
        auto res = value + m_value;
        return LimitedAngle(limit_value(res));
    }
    
    LimitedAngle operator-(double value)
    {
        auto res = m_value + value;
        return LimitedAngle(limit_value(res));
    }
    
    bool operator==(const LimitedAngle& other) const
    {
        return other.GetValue() == m_value;
    }
    bool operator==(double other) const
    {
        return m_value == other;
    }
    
    bool operator >(double other) const
    {
        return m_value > other;
    }
    bool operator <(double other) const
    {
        return m_value < other;
    }
    
    LimitedAngle& operator+=(double angle)
    {
        m_value = limit_value(m_value+angle);
        return *this;
    }
    
    LimitedAngle& operator-=(double angle)
    {
        m_value = limit_value(m_value-angle);
        return *this;
    }
    
    double GetValue() const { return m_value; }
    
private:
    double limit_value(double value)
    {
        while (value >= 360)
        {
            value -= 360;
        }
        while (value < 0)
        {
            value = 360 + value;
        }
        return value;
    }
    
    double m_value;
};

struct Position
{
    Position(int x, int y, double angle):
        x(x), y(y), angle(angle)
    {}
    
    Position(const Position& other):
        x(other.x), y(other.y), angle(other.angle)
    {}
    
    int x;
    int y;
    LimitedAngle angle; //0 means facing North/forward
    
    Position& operator+=(int distance)
    {
        auto new_pos = *this + distance;
        x = new_pos.x;
        y = new_pos.y;
        angle = new_pos.angle;
        return *this;
    }
    
private:
    Position operator+(int distance);
};

struct Level
{
    Level():
        m_tiles{0,0,0,0,0,0,
                0,1,1,1,0,0,
                0,0,0,0,0,0,
                0,1,0,0,0,0,
                0,0,0,0,0,0,
                0,1,0,0,0,1},
        m_tile_side(500),
        m_player_pos(2000, 1500, 0),
        m_player_fov(60),
        m_turn_amount(10),
        m_move_amount(10)
    {
        m_map_width = MAP_SIDE*m_tile_side;
        m_map_height = MAP_SIDE*m_tile_side;
    }
    
    std::vector<float> get_line_heights(int view_width);
    Position m_player_pos;
    
    bool in_map(Position);
    bool in_wall(Position);
    
    void apply_movement(const uint8_t* state);
    
    int m_map_width;
    int m_map_height;
    LimitedAngle m_player_fov;
    
    float get_line_height_factor(int x, int view_width);
    
    std::array<int, MAP_SIDE*MAP_SIDE> m_tiles;
    int m_tile_side;
    
private:
    const int m_turn_amount;
    const int m_move_amount;
};

#endif /* Level_hpp */
