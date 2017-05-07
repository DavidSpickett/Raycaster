//
//  PositionTests.cpp
//  RayCaster
//
//  Created by David Spickett on 07/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include <stdio.h>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Position.hpp"
#include <array>

TEST_CASE("limited angle throws")
{
    REQUIRE_THROWS(LimitedAngle(500));
    REQUIRE_THROWS(LimitedAngle(-1));
    REQUIRE_THROWS(LimitedAngle(360));
}

TEST_CASE("limited angle operations")
{
    //Boundary of 360->0
    REQUIRE((LimitedAngle(350) + 10) == 0);
    REQUIRE((LimitedAngle(350) + 20) == 10);
    REQUIRE((LimitedAngle(10) - 10) == 0);
    REQUIRE((LimitedAngle(10) - 20) == 350);
    
    REQUIRE((LimitedAngle(100) + 200) == 300);
    REQUIRE((LimitedAngle(100) + 280) == 20);
    
    //Full circle
    REQUIRE((LimitedAngle(320) - 360) == 320);
    //..and a bit more
    REQUIRE((LimitedAngle(320) - 380) == 300);
    
    //Go round more than once clockwise
    REQUIRE((LimitedAngle(10) + 730) == 20);
    //...anti-clockwise
    REQUIRE((LimitedAngle(10) - 740) == 350);
}

struct PosChangeTest
{
    double angle;
    int change;
    int expected_x;
    int expected_y;
};

TEST_CASE("position addition/subtraction")
{
    std::vector<PosChangeTest> cases = {
        PosChangeTest{ 0,  10, 100, 110},
        PosChangeTest{ 0, -10, 100,  90},
        
        PosChangeTest{45,  10, 107, 107},
        PosChangeTest{45,  -10, 93, 93},
        
        PosChangeTest{90,  10, 110, 100},
        PosChangeTest{90, -10,  90, 100},
        
        PosChangeTest{135,  10, 107, 93},
        PosChangeTest{135,  -10, 93, 107},
        
        PosChangeTest{180,  10, 100,  90},
        PosChangeTest{180, -10, 100, 110},
        
        PosChangeTest{225,  10, 93, 93},
        PosChangeTest{225,  -10, 107, 107},
        
        PosChangeTest{270,  10,  90, 100},
        PosChangeTest{270, -10, 110, 100},
        
        PosChangeTest{315,  10, 93, 107},
        PosChangeTest{315,  -10, 107, 93},
    };
    
    std::for_each(cases.begin(), cases.end(), [] (const PosChangeTest& _case)
    {
        Position a(100, 100, _case.angle);
        a += _case.change;
        
        REQUIRE(a.x == _case.expected_x);
        REQUIRE(a.y == _case.expected_y);
    });
}
