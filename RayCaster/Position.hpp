//
//  Position.hpp
//  RayCaster
//
//  Created by David Spickett on 07/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#ifndef Position_hpp
#define Position_hpp

#include <stdio.h>
#include <math.h>
#include <stdexcept>

double to_radians(double degrees);

class LimitedAngle
{
public:
    LimitedAngle(double value)
    {
        if ((value >= 360) || (value < 0))
        {
            throw std::runtime_error("LimitedAngle must be > 0 and < 360 degrees");
        }
        m_value = value;
    }
    
    LimitedAngle operator+(double value) const
    {
        auto res = value + m_value;
        return LimitedAngle(limit_value(res));
    }
    
    LimitedAngle operator-(double value) const
    {
        auto res = m_value - value;
        return LimitedAngle(limit_value(res));
    }
    
    bool operator==(const LimitedAngle& other) const
    {
        return other.GetValue() == m_value;
    }
    bool operator==(double other) const
    {
        return m_value == other;
    }
    
    bool operator >(double other) const
    {
        return m_value > other;
    }
    bool operator <(double other) const
    {
        return m_value < other;
    }
    
    bool operator <=(double other) const
    {
        return (*this == other) || (*this < other);
    }
    bool operator >=(double other) const
    {
        return (*this == other) || (*this > other);
    }
    
    LimitedAngle& operator+=(double angle)
    {
        m_value = limit_value(m_value+angle);
        return *this;
    }
    
    LimitedAngle& operator-=(double angle)
    {
        m_value = limit_value(m_value-angle);
        return *this;
    }
    
    double GetValue() const { return m_value; }
    
private:
    double limit_value(double value) const
    {
        while (value >= 360)
        {
            value -= 360;
        }
        while (value < 0)
        {
            value = 360 + value;
        }
        return value;
    }
    
    double m_value;
};

struct Position
{
    Position(int x, int y, double angle):
    x(x), y(y), angle(angle)
    {}
    
    Position(const Position& other):
    x(other.x), y(other.y), angle(other.angle)
    {}
    
    Position operator+(int distance) const;
    Position operator-(int distance) const
    {
        Position ret(*this);
        ret.angle -= 180;
        ret += distance;
        ret.angle += 180;
        return ret;
    }
    
    Position& operator+=(int distance)
    {
        *this = *this + distance;
        return *this;
    }
    
    Position& operator-=(int distance)
    {
        *this = *this - distance;
        return *this;
    }
    
    Position& operator=(const Position& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        angle = rhs.angle;
        return *this;
    }
    
    int x;
    int y;
    LimitedAngle angle; //0 means facing North
};

#endif /* Position_hpp */
