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
                auto x=0, y=0;
                auto angle = pos.angle;
                
                if (angle < 90)
                {
                    y = cos(angle) * distance;
                    x = sqrt(pow(distance, 2) - pow(y, 2));
                }
                else if ((angle > 90) && (angle < 180))
                {
                    angle -= 90;
                    x = cos(angle) * distance;
                    y = sqrt(pow(distance, 2) - pow(x, 2));
                    
                    y *= -1;
                }
                else if ((angle > 180) && (angle < 270))
                {
                    angle -= 180;
                    y = cos(angle) * distance;
                    x = sqrt(pow(distance, 2) - pow(y, 2));
                    
                    x *= -1;
                    y *= -1;
                }
                else if (angle > 270)
                {
                    angle -= 270;
                    x = cos(angle) * distance;
                    y = sqrt(pow(distance, 2) - pow(x, 2));
                    
                    x*= -1;
                }
                
                return Position(pos.x+x, pos.y+y, pos.angle);
            }
        }
    }
    
    Position translate_ray_pos(Position original, int x, int screen_width)
    {
        auto half_screen = screen_width/2;
        
        /*Make a new vector orthogonal to the original by 90
         degrees clockwise.*/
        original.angle += 90;
        original.angle %= 360;
        
        //We are modifying distance now, not x or y
        //always add the same amount as we go in the same dir
        auto translate_distance = - half_screen + x;
        original = add_to_pos(original, translate_distance);
        
        //Now move it back to the direction of the camera
        original.angle -= 90;
        if (original.angle < 0)
        {
            original.angle += 360;
        }
        
        return original;
    }
}

float Level::get_line_height_factor(int x, int view_width)
{
    /*In steps of 100 pixels take a line out from this X point on the player's
    view and see what we collide with.*/
    Position pos(m_player_pos.x, m_player_pos.y, m_player_pos.angle);
    
    //Remember that 'X' here is just the slice of the screen, not a directional thing.
    //When we rotate the camera, the camera's x=0 stays the same.

    pos = translate_ray_pos(pos, x, view_width);
    
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
