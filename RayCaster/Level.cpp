//
//  Level.cpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include "Level.hpp"
#include <math.h>

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

std::vector<line_height> Level::get_line_heights(int view_width)
{
    //View width is the screenwidth.
    std::vector<line_height> ret;
    for (int x=0; x != view_width; ++x)
    {
        //ret.push_back(get_line_height_factor(x, view_width));
        ret.push_back(get_line_height_factor_using_gridlines(x, view_width));
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

    int wrap_to_tile(int pos, int tile_side)
    {
        auto remainder = pos % tile_side;
        auto min = pos - remainder;
        auto half_way = min+(tile_side/2);
        
        if (pos >= half_way)
        {
            pos = pos - remainder + tile_side;
        }
        else
        {
            pos -= remainder;
        }
        
        return pos;
    }
}

bool Level::grid_in_wall(Position pos, bool horiz_gridlines)
{
    /*Because of rounding errors sometimes the positions aren't exactly on the 
     gridline which presents a problem when trying to find the righ cell to check.
     Luckily we know what one of the components *should* be.
     */
    if (horiz_gridlines)
    {
        if (pos.y % m_tile_side)
        {
            pos.y = wrap_to_tile(pos.y, m_tile_side);
        }
    }
    else
    {
        if (pos.x % m_tile_side)
        {
            pos.x = wrap_to_tile(pos.x, m_tile_side);
        }
    }
    
    auto x = pos.x/m_tile_side;
    auto y = pos.y/m_tile_side;
    
    //One of these will be an exact multiple of tile side
    if (horiz_gridlines)
    {
        //We're checking horizontal grid lines
        if ((pos.angle > 90) && (pos.angle < 180))
        {
            y += 1;
        }
    }
    else
    {
        //We're checking vertical gridlines
        if (pos.angle >180)
        {
            x -= 1;
        }
    }
    
    //Also -1 here?
    y = MAP_SIDE - y;
    
    //Y co-ord is inverted for map data
    return m_tiles[x+(y*MAP_SIDE)] == 1;
}

line_height Level::get_line_height_factor_using_gridlines(int x, int view_width)
{
    Position pos(m_player_pos);
    
    //Each ray comes from the player at a different angle depdning on its X
    double angle = - (m_player_fov.GetValue()/2) + ((m_player_fov.GetValue()/view_width)*x);
    
    pos.angle += angle;
    
    auto found_horizontal = false;
    auto found_vertical = false;
    
    Position horiz_pos(pos);
    Position vert_pos(pos);
    std::vector<SDL_Point> points_checked;
    
    //First check collision with horizontal grid lines
    auto going_up_angle = (pos.angle.GetValue() < 90) || (pos.angle.GetValue() > 270);
    auto going_right_angle = pos.angle.GetValue() < 180;
    auto change_grid_angle = pos.angle.GetValue();
    
    while (change_grid_angle >= 90)
    {
        change_grid_angle -= 90;
    }
    
    if ((pos.angle > 270) ||
        ((pos.angle > 90) && (pos.angle < 180)))
    {
        change_grid_angle = 90 - change_grid_angle;
    }
    
    //Dont bother checking if it goes in a straight line horiz (unless you're directly on the line?)
    if ((pos.angle.GetValue() != 90) && (pos.angle.GetValue() != 270))
    {
        //Move until we hit a horizontal line
        double y_to_next_gridline = horiz_pos.y % m_tile_side;
        if (going_up_angle && (y_to_next_gridline != 0))
        {
            y_to_next_gridline = m_tile_side - y_to_next_gridline;
        }
        auto x_to_next_gridline = y_to_next_gridline * tan(to_radians(change_grid_angle));
        
        //Work out the hypotinuse of the triangle that that describes
        auto vect_distance = sqrt(pow(y_to_next_gridline, 2) + pow(x_to_next_gridline, 2));
        horiz_pos += vect_distance;
        
        //From here we need to move 1 grid cell each Y and some X each time
        double change_in_y = m_tile_side;
        auto change_in_x = change_in_y * tan(to_radians(change_grid_angle));
        
        auto grid_distance = sqrt(pow(change_in_y, 2) + pow(change_in_x, 2));
        
        while (in_map(horiz_pos))
        {
            points_checked.push_back(SDL_Point{horiz_pos.x, m_map_height - horiz_pos.y});
            
            if (grid_in_wall(horiz_pos, true))
            {
                found_horizontal = true;
                break;
            }
            
            horiz_pos += grid_distance;
        }
    }
    
    //Check vertical intersections
    if ((pos.angle.GetValue() != 0) && (pos.angle.GetValue() != 180))
    {
        auto x_to_next_gridline = vert_pos.x % m_tile_side;
        if (going_right_angle && (x_to_next_gridline != 0))
        {
            x_to_next_gridline = m_tile_side - x_to_next_gridline;
        }
        auto y_to_next_gridline = x_to_next_gridline / tan(to_radians(change_grid_angle));
        
        auto vect_distance = sqrt(pow(y_to_next_gridline, 2) + pow(x_to_next_gridline, 2));
        horiz_pos += vect_distance;
        
        vert_pos += vect_distance;
        
        //Same distance between each grid line once we've found the 1st one
        auto change_in_x = m_tile_side;
        auto change_in_y = change_in_x / tan(to_radians(change_grid_angle));
        
        auto grid_distance = sqrt(pow(change_in_y, 2) + pow(change_in_x, 2));
        
        while (in_map(vert_pos))
        {
            points_checked.push_back(SDL_Point{vert_pos.x, m_map_height - vert_pos.y});
            
            if (grid_in_wall(vert_pos, false))
            {
                found_vertical = true;
                break;
            }
            
            vert_pos += grid_distance;
        }
    }
    
    if (found_vertical || found_horizontal)
    {
        auto horiz_distance = found_horizontal ? point_distance(horiz_pos, m_player_pos) : -1;
        auto vert_distance = found_vertical ? point_distance(vert_pos, m_player_pos) : -1;
        
        auto distance = 0;
        if (
            ((found_horizontal && found_vertical) && (horiz_distance < vert_distance)) ||
            !found_vertical)
        {
            //Use horizontal projection distance
            distance = horiz_distance;
        }
        else
        {
            distance = vert_distance;
        }
        
        return line_height{1/(float(distance)/100), points_checked};;
    }
    else
    {
        //Ended up off of the map somewhere.
        return line_height{0, points_checked};
    }
}

line_height Level::get_line_height_factor(int x, int view_width)
{
    /*In steps of some number of pixels take a line out from this X point on the player's
    view and see what we collide with.*/
    Position pos(m_player_pos);
    std::vector<SDL_Point> points_checked;
    
    //Each ray comes from the player at a slightly different angle according to the FOV.
    double angle = - (m_player_fov.GetValue()/2) + ((m_player_fov.GetValue()/view_width)*x);
    
    pos.angle += angle;
    
    auto distance = 0;
    const auto distance_step = 10;
    auto am_in_wall = false;
    
    while (in_map(pos))
    {
        points_checked.push_back(SDL_Point{pos.x, m_map_height-pos.y});
        
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
        return line_height{1/(float(distance)/50), points_checked};
    }
    return line_height{0, points_checked};
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
