//
//  Level.cpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include "Level.hpp"
#include <math.h>

namespace
{
    double to_radians(double degrees)
    {
        return (degrees/180) * M_PI;
    }
}

Position Position::operator+(int distance) const
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
        
        auto triangle_angle = to_radians(calc_angle.GetValue());
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
        
        while (!in_map(m_player_pos))
        {
            m_player_pos -= m_move_amount;
        }
        while (in_wall(m_player_pos))
        {
            m_player_pos -= m_move_amount;
        }
    }
    else if (state[SDL_SCANCODE_DOWN])
    {
        m_player_pos -= m_move_amount;
        
        while (!in_map(m_player_pos))
        {
            m_player_pos += m_move_amount;
        }
        while (in_wall(m_player_pos))
        {
            m_player_pos += m_move_amount;
        }
    }
}

std::vector<float> Level::get_line_heights(int view_width)
{
    //View width is the screenwidth.
    std::vector<float> ret;
    for (int x=0; x != view_width; ++x)
    {
        ret.push_back(get_line_height_factor(x, view_width));
        //ret.push_back(get_line_height_factor_using_gridlines(x, view_width));
    }
    return ret;
}

namespace
{
    float point_distance(Position p1, Position p2)
    {
        auto x_change = p2.x - p1.x;
        //Just want magnitude
        if (x_change < 0)
        {
            x_change *= -1;
        }
        auto y_change = p2.y - p1.y;
        if (y_change < 0)
        {
            y_change *= -1;
        }
        
        return sqrt((pow(x_change, 2) + pow(y_change, 2)));
    }
}

float Level::get_line_height_factor_using_gridlines(int x, int view_width)
{
    Position pos(m_player_pos);
    
    //Each ray comes from the player at a different angle depdning on its X
    double angle = - (m_player_fov.GetValue()/2) + ((m_player_fov.GetValue()/view_width)*x);
    
    pos.angle += angle;
    
    auto found_horizontal = false;
    auto found_vertical = false;
    
    Position horiz_pos(pos);
    Position vert_pos(pos);
    
    //First check collision with horizontal grid lines
    auto going_up_angle = (pos.angle.GetValue() < 90) || (pos.angle.GetValue() > 270);
    auto going_right_angle = pos.angle.GetValue() < 180;
    auto change_grid_angle = pos.angle.GetValue();
    while (change_grid_angle >= 90)
    {
        change_grid_angle -= 90;
    }
    
    //Dont bother checking if it goes in a straight line horiz (unless you're directly on the line?)
    if ((pos.angle.GetValue() != 90) && (pos.angle.GetValue() != 270))
    {
        //Move until we hit a horizontal line
        auto y_to_next_gridline = horiz_pos.y % m_tile_side;
        if (going_up_angle)
        {
            y_to_next_gridline = m_tile_side - y_to_next_gridline;
        }
        auto x_to_next_gridline = y_to_next_gridline / tan(to_radians(change_grid_angle));
        
        if (!going_right_angle)
        {
            x_to_next_gridline *= -1;
        }
        if (!going_up_angle)
        {
            y_to_next_gridline *= -1;
        }
        
        horiz_pos.x += x_to_next_gridline;
        horiz_pos.y += y_to_next_gridline;
        
        //From here we need to move 1 grid cell each Y and some X each time
        auto change_in_y = going_up_angle ? m_tile_side : -m_tile_side;
        auto change_in_x = m_tile_side / tan(to_radians(change_grid_angle));
        
        while (in_map(horiz_pos))
        {
            //Might need adjusting depending on whether we're going up or down?
            if (in_wall(horiz_pos))
            {
                found_horizontal = true;
                break;
            }
            
            horiz_pos.x += change_in_x;
            horiz_pos.y += change_in_y;
        }
    }
    
    //Check vertical intersections
    if ((pos.angle.GetValue() != 0) && (pos.angle.GetValue() != 180))
    {
        auto x_to_next_gridline = vert_pos.x % m_tile_side;
        if (going_right_angle)
        {
            x_to_next_gridline = m_tile_side - x_to_next_gridline;
        }
        auto y_to_next_gridline = x_to_next_gridline*tan(to_radians(change_grid_angle));
        
        if (!going_right_angle)
        {
            x_to_next_gridline *= -1;
        }
        if (!going_up_angle)
        {
            y_to_next_gridline *= -1;
        }
        
        vert_pos.x += x_to_next_gridline;
        vert_pos.y += y_to_next_gridline;
        
        //Same distance between each grid line once we've found the 1st one
        auto change_in_x = going_right_angle ? m_tile_side : -m_tile_side;
        auto change_in_y = m_tile_side / tan(to_radians(change_grid_angle));
        
        while (in_map(vert_pos))
        {
            if (in_wall(vert_pos))
            {
                found_vertical = true;
                break;
            }
            
            vert_pos.x += change_in_x;
            vert_pos.y += change_in_y;
        }
    }
    
    if (found_vertical || found_horizontal)
    {
        auto horiz_distance = -1;
        auto vert_distance = -1;
        
        if (found_vertical)
        {
            vert_distance = point_distance(vert_pos, m_player_pos);
        }
        if (found_horizontal)
        {
            horiz_distance = point_distance(horiz_pos, m_player_pos);
        }
        
        bool use_horizontal = (horiz_distance < vert_distance) || (vert_distance == -1);
        auto distance = use_horizontal ? horiz_distance : vert_distance;
        
        return 1/(float(distance)/100);
    }
    else
    {
        //Ended up off of the map somewhere.
        return 0;
    }
}

float Level::get_line_height_factor(int x, int view_width)
{
    /*In steps of some number of pixels take a line out from this X point on the player's
    view and see what we collide with.*/
    Position pos(m_player_pos);
    
    //Each ray comes from the player at a slightly different angle according to the FOV.
    double angle = - (m_player_fov.GetValue()/2) + ((m_player_fov.GetValue()/view_width)*x);
    
    pos.angle += angle;
    
    auto distance = 0;
    const auto distance_step = 50;
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
    
    /*Problem here being that we don't check on the grid lines, but
     check whether someting is inside the wall, so lines to the same wall can have
     different distances if they go further. Then when we correct everything 
     gets a little worse.*/
    
    //Correct for the angle of the ray (get y component of the vector basically)
    //distance *= cos(to_radians(angle));

    /*The the scale of the object falls off with distance. Assuming at 0 distance from
     the player the wall is as high as the screen.*/
    if (am_in_wall)
    {
        return 1/(float(distance)/50);
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
