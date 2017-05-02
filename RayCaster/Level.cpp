//
//  Level.cpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include "Level.hpp"

std::vector<int> Level::get_line_heights(int view_width)
{
    //View width is the screenwidth.
    std::vector<int> ret;
    for (int x=0; x != view_width; ++x)
    {
        ret.push_back(get_line_height(x));
    }
    return ret;
}

int Level::get_line_height(int x)
{
    /*In steps of 100 pixels take a line out from this X point on the player's
    view and see what we collide with.*/
    return 10;
}

bool Level::in_map(int x, int y)
{
    return (x < m_map_width) && (y < m_map_height);
}

bool Level::in_wall(int x, int y)
{
    x /= m_tile_side;
    y /= m_tile_side;
    
    //Our map data is laid out to look like a normal map but our y co-ord is inverted.
    y = MAP_SIDE - y;
    
    return m_tiles[(x*MAP_SIDE)+]
}
