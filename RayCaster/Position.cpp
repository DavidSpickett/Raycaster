//
//  Position.cpp
//  RayCaster
//
//  Created by David Spickett on 07/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include "Position.hpp"
#include <utility>

double to_radians(double degrees)
{
    return (degrees/180) * M_PI;
}

Position Position::operator+(int distance) const
{
    auto ret(*this);
    
    //Convert distance and angle into cartesian X and Y to change the position
    
    //Translate everything so it's using the y axis north as it's 'x' side.
    auto calc_angle = angle;
    
    if ((calc_angle >= 90) && (calc_angle < 180))
    {
        calc_angle -= 90;
    }
    else if ((calc_angle >= 180) && (calc_angle < 270))
    {
        calc_angle -= 180;
    }
    else if (calc_angle >= 270)
    {
        calc_angle -= 270;
    }
    
    auto triangle_angle = to_radians(calc_angle.GetValue());
    auto y = cos(triangle_angle)*distance;
    auto x = sin(triangle_angle)*distance;
    
    if ((angle >= 90) && (angle < 180))
    {
        std::swap(x, y);
        y *= -1;
    }
    else if ((angle >= 180) && (angle < 270))
    {
        y *= -1;
        x *= -1;
    }
    else if (angle >= 270)
    {
        std::swap(x, y);
        x *= -1;
    }
    
    ret.x += x;
    ret.y += y;
    
    return ret;
}
