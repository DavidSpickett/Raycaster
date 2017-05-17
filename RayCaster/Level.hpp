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
#include <SDL2/SDL.h>
#include "Position.hpp"

struct line_height
{
    float height;
    std::vector<SDL_Point> points_checked;
};

const auto MAP_SIDE = 6;

struct Level
{
    Level():
        m_tiles{0,0,0,0,0,0,
                0,1,1,1,0,0,
                0,0,0,0,0,0,
                0,1,0,0,0,0,
                0,0,0,0,0,1,
                0,1,0,0,1,1},
        m_tile_side(500),
        m_player_pos(2000, 1500, 0),
        m_player_fov(60),
        m_turn_amount(10),
        m_move_amount(10)
    {
        m_map_width = MAP_SIDE*m_tile_side;
        m_map_height = MAP_SIDE*m_tile_side;
    }
    
    std::vector<line_height> get_line_heights(int view_width);
    Position m_player_pos;
    
    bool in_map(Position);
    bool in_wall(Position);
    bool grid_in_wall(Position pos, bool horiz_gridlines);
    
    void apply_movement(const uint8_t* state);
    
    int m_map_width;
    int m_map_height;
    LimitedAngle m_player_fov;
    
    std::array<int, MAP_SIDE*MAP_SIDE> m_tiles;
    int m_tile_side;
    
private:
    line_height get_line_height_factor(int x, int view_width);
    line_height get_line_height_factor_using_gridlines(int x, int view_width);
    
    const int m_turn_amount;
    const int m_move_amount;
};

#endif /* Level_hpp */
