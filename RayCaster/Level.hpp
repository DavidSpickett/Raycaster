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
    double angle; //0 means facing North/forward
};

Position add_to_pos(const Position& pos, unsigned distance);

struct Level
{
    Level():
        m_tiles{0,0,0,0,0,0,
                0,1,1,1,0,0,
                0,1,0,0,0,0,
                0,1,0,0,0,0,
                0,1,0,0,0,0,
                0,0,0,0,0,0},
        m_tile_side(500),
        m_player_pos(2000, 1500, 0)
    {
        m_map_width = MAP_SIDE*m_tile_side;
        m_map_height = MAP_SIDE*m_tile_side;
    }
    
    std::vector<float> get_line_heights(int view_width);
    Position m_player_pos;
    
    void player_forward(int amount);
    void player_backward(int amount);
    
    bool in_map(Position);
    bool in_wall(Position);
    
    int m_map_width;
    int m_map_height;
    
    float get_line_height_factor(int x, int view_width);
    
    std::array<int, MAP_SIDE*MAP_SIDE> m_tiles;
    int m_tile_side;
};

#endif /* Level_hpp */
