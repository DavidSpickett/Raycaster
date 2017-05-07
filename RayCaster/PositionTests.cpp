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

TEST_CASE( "position addition right angles", "[pos_addition]" )
{
    Position a(0, 0, 0);
    a += 10;
    
    REQUIRE(a.x == 0);
    REQUIRE(a.y == 10);
}

    
