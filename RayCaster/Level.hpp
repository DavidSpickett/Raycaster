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
    line_height(float height, bool vertical_intersect, std::vector<SDL_Point> points_checked,
                int texture_offset):
        height(height), vertical_intersect(vertical_intersect), points_checked(points_checked),
        texture_offset(texture_offset)
    {}
    
    line_height(std::vector<SDL_Point> points_checked):
        height(0), vertical_intersect(false), points_checked(points_checked),
        texture_offset(-1)
    {}
    
    int texture_offset;
    float height;
    std::vector<SDL_Point> points_checked;
    bool vertical_intersect;
};

const auto MAP_SIDE = 6;

struct Level
{
    Level():
        m_tiles{0,0,0,0,0,0,
                0,1,1,1,0,0,
                0,0,0,0,0,0,
                0,1,0,0,0,0,
                1,0,0,0,0,1,
                0,1,1,0,1,1},
        m_tile_side(500),
        m_player_pos(2000, 1500, 0),
        m_player_fov(60),
        m_turn_amount(15),
        m_move_amount(20),
        m_distance_scale(100),
        m_gridline_projection(true)
    {
        m_map_width = MAP_SIDE*m_tile_side;
        m_map_height = MAP_SIDE*m_tile_side;
    }
    
    std::vector<line_height> get_line_heights(int view_width);
    Position m_player_pos;
    
    void apply_movement(const uint8_t* state);
    
    int m_map_width;
    int m_map_height;
    LimitedAngle m_player_fov;
    
    std::array<int, MAP_SIDE*MAP_SIDE> m_tiles;
    int m_tile_side;
    bool m_gridline_projection;
    
private:
    float get_scaled_height_factor(double distance);
    bool in_map(Position);
    bool in_wall(Position);
    bool grid_in_wall(Position pos, bool horiz_gridlines);
    
    line_height get_line_height_factor(int x, int view_width);
    line_height get_line_height_factor_using_gridlines(int x, int view_width);
    
    const int m_distance_scale;
    const int m_turn_amount;
    const int m_move_amount;
};

#endif /* Level_hpp */
