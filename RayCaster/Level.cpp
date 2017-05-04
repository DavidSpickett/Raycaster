//
//  Level.cpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include "Level.hpp"
#include <math.h>

Position Position::operator+(int distance)
{
    auto ret(*this);
    
    //We could generalise this since cos(0)=1 etc...
    if (angle == 0)
    {
        ret.y += distance;
    }
    else if (angle == 90)
    {
        ret.x += distance;
    }
    else if (angle == 180)
    {
        ret.y -= distance;
    }
    else if (angle == 270)
    {
        ret.x -= distance;
    }
    else
    {
        //Convert distance and angle into cartesian X and Y to change the position
        
        //Translate everything so it's using the y axis north as it's 'x' side.
        auto calc_angle = angle;
        
        if ((calc_angle > 90) && (calc_angle < 180))
        {
            calc_angle -= 90;
        }
        else if ((calc_angle > 180) && (calc_angle < 270))
        {
            calc_angle -= 180;
        }
        else if ((calc_angle > 270) && (calc_angle < 360))
        {
            calc_angle -= 270;
        }
        
        auto triangle_angle = (calc_angle.GetValue()/180) * M_PI;
        auto y = cos(triangle_angle)*distance;
        auto x = sin(triangle_angle)*distance;
        
        if ((angle > 90) && (angle < 180))
        {
            std::swap(x, y);
            y *= -1;
        }
        else if ((angle > 180) && (angle < 270))
        {
            y *= -1;
            x *= -1;
        }
        else if ((angle > 270) && (angle < 360))
        {
            std::swap(x, y);
            x *= -1;
        }
        
        ret.x += x;
        ret.y += y;
    }
    
    return ret;
}

void Level::apply_movement(const uint8_t* state)
{
    if (state[SDL_SCANCODE_LEFT])
    {
        m_player_pos.angle -= m_turn_amount;
    }
    else if (state[SDL_SCANCODE_RIGHT])
    {
        m_player_pos.angle += m_turn_amount;
    }
    else if (state[SDL_SCANCODE_UP])
    {
        m_player_pos += m_move_amount;
    }
    else if (state[SDL_SCANCODE_DOWN])
    {
        m_player_pos.angle -= 180;
        m_player_pos += m_move_amount;
        m_player_pos.angle += 180;
    }
}

std::vector<float> Level::get_line_heights(int view_width)
{
    //View width is the screenwidth.
    std::vector<float> ret;
    for (int x=0; x != view_width; ++x)
    {
        ret.push_back(get_line_height_factor(x, view_width));
    }
    return ret;
}

float Level::get_line_height_factor(int x, int view_width)
{
    /*In steps of some number of pixels take a line out from this X point on the player's
    view and see what we collide with.*/
    Position pos(m_player_pos);
    
    //Each ray comes from the player at a slightly different angle according to the FOV.
    double angle = - (m_player_fov.GetValue()/2) + ((m_player_fov.GetValue()/view_width)*x);
    
    pos.angle += angle;
    
    if (pos.angle > 360)
    {
        pos.angle -= 360;
    }
    else if (pos.angle < 0)
    {
        pos.angle += 360;
    }
    
    auto distance = 0;
    const auto distance_step = 100;
    auto am_in_wall = false;
    
    while (in_map(pos))
    {
        if (in_wall(pos))
        {
            am_in_wall = true;
            break;
        }
        
        distance += distance_step;
        pos += distance_step;
    }

    /*The the scale of the object falls off with distance. Assuming at 0 distance from
     the player the wall is as high as the screen.*/
    if (am_in_wall)
    {
        return 1/(float(distance)/100);
    }
    return 0;
}

bool Level::in_map(Position pos)
{
    return (pos.x >= 0) && (pos.y >= 0) && (pos.x < m_map_width) && (pos.y < m_map_height);
}

bool Level::in_wall(Position pos)
{
    pos.x /= m_tile_side;
    pos.y /= m_tile_side;
    
    //Our map data is laid out to look like a normal map but our y co-ord is inverted.
    pos.y = MAP_SIDE - pos.y - 1;
    
    return m_tiles[pos.x+(pos.y*MAP_SIDE)] == 1;
}
