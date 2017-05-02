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
    Position(int x, int y, int angle):
        x(x), y(y), angle(angle)
    {}
    
    int x;
    int y;
    int angle; //0 means facing North/forward
};

class Level
{
public:
    Level():
        m_tiles{0,0,0,0,0,0,
                0,1,1,1,0,0,
                0,1,0,0,0,0,
                0,1,0,0,0,0,
                0,1,0,0,0,0,
                0,0,0,0,0,0},
        m_tile_side(500),
        m_player_pos(2000, 1500, 270)
    {
        m_map_width = MAP_SIDE*m_tile_side;
        m_map_height = MAP_SIDE*m_tile_side;
    }
    
    std::vector<float> get_line_heights(int view_width);
    Position m_player_pos;
    
private:
    bool in_map(int x, int y);
    bool in_wall(int x, int y);
    
    int m_map_width;
    int m_map_height;
    
    float get_line_height_factor(int x, int view_width);
    
    std::array<int, MAP_SIDE*MAP_SIDE> m_tiles;
    int m_tile_side;
};

#endif /* Level_hpp */
