//
//  Level.cpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include "Level.hpp"
#include <math.h>

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

namespace
{
    Position add_to_pos(const Position& pos, double distance)
    {
        switch (pos.angle)
        {
            case 0:
                return Position(pos.x, pos.y+distance, pos.angle);
            case 90:
                return Position(pos.x+distance, pos.y, pos.angle);
            case 180:
                return Position(pos.x, pos.y-distance, pos.angle);
            case 270:
                return Position(pos.x-distance, pos.y, pos.angle);
            default:
            {
                //x = cos(angle) * distance
                auto x = cos(pos.angle) * distance;
                //Then solve pythagoras to get y distance
                auto y = sqrt(pow(distance, 2) - pow(x, 2));
                
                //Except that, the sign of these distances depends on the angle.
                if ((pos.angle > 90) && (pos.angle <= 180))
                {
                    y *= -1;
                }
                else if ((pos.angle > 180) && (pos.angle <= 270))
                {
                    y *= -1;
                    x *= -1;
                }
                else if (pos.angle > 270)
                {
                    x *= -1;
                }
                
                return Position(pos.x+x, pos.y+y, pos.angle);
            }
        }
    }
}

float Level::get_line_height_factor(int x, int view_width)
{
    /*In steps of 100 pixels take a line out from this X point on the player's
    view and see what we collide with.*/
    Position pos(m_player_pos.x, m_player_pos.y, m_player_pos.angle);
    
    //Remember that 'X' here is just the slice of the screen, not a directional thing.
    //When we rotate the camera, the camera's x=0 stays the same.
    auto half_screen = view_width/2;
    switch (m_player_pos.angle)
    {
        case 0:
            pos.x = pos.x - half_screen + x;
            break;
        case 90:
            pos.y = pos.y + half_screen - x;
            break;
        case 180:
            pos.x = pos.x + half_screen - x;
            break;
        case 270:
            pos.y = pos.y - half_screen + x;
            break;
        default:
            throw std::runtime_error("??");
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
        pos = add_to_pos(pos, distance_step);
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
