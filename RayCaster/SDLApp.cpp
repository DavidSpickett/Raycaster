//
//  SDLApp.cpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include <stdexcept>
#include "SDLApp.hpp"

SDLApp::SDLApp(int width, int height):
    m_width(width), m_height(height)
{
    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("SDL could not initialize!");
    }
    
    //Create window
    m_window = SDL_CreateWindow("Ray Caster",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            m_width, m_height,
            SDL_WINDOW_SHOWN);
    
    if(m_window == NULL)
    {
        throw std::runtime_error("Window could not be created!");
    }
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
}

void SDLApp::draw_2d_map(const Level& level)
{
    const auto cell_size = 10;
    
    //Walls and floor
    for (auto x=0; x != MAP_SIDE; ++x)
    {
        for (auto y=0; y != MAP_SIDE; ++y)
        {
            if (level.m_tiles[x+(y*MAP_SIDE)])
            {
                SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            }
            
            SDL_Rect r;
            r.h = cell_size;
            r.w = cell_size;
            r.x = x*cell_size;
            r.y = y*cell_size;
            
            SDL_RenderFillRect(m_renderer, &r);
        }
    }
    
    //Outline
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
    SDL_Rect outline;
    outline.x = -1;
    outline.y = -1;
    outline.w = (MAP_SIDE*cell_size)+1;
    outline.h = outline.w;
    
    SDL_RenderDrawRect(m_renderer, &outline);
    
    //Player position
    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    float x = (float(level.m_player_pos.x)/level.m_tile_side) * cell_size;
    float y = (float(level.m_map_height - level.m_player_pos.y)/level.m_tile_side)*cell_size;
    SDL_Rect player;
    player.x = x;
    player.y = y;
    player.w = 2;
    player.h = 2;
    SDL_RenderDrawRect(m_renderer, &player);
    
    //Player direction
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
    SDL_Point p_heading[2];
    p_heading[0].x = x;
    p_heading[0].y = y;
    
    //Add to player pos to get end of heading line
    Position heading_pos = add_to_pos(level.m_player_pos, 500);
    float heading_x = (float(heading_pos.x)/level.m_tile_side) * cell_size;
    float heading_y = (float(level.m_map_height - heading_pos.y)/level.m_tile_side)*cell_size;
    
    p_heading[1].x = heading_x;
    p_heading[1].y = heading_y;
    
    SDL_RenderDrawLines(m_renderer, p_heading, 2);
}

void SDLApp::draw_lines(std::vector<float>& height_factors)
{
    clear();
    
    auto midscreen = m_height/2;
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    for (auto x=0; x != height_factors.size(); ++x)
    {
        // Divide by 2 because we draw above and below the middle
        int line_height = (m_height*height_factors[x])/2;
        
        if (line_height)
        {
            SDL_RenderDrawLine(m_renderer,
                               x, midscreen-line_height,
                               x, midscreen+line_height);
        }
    }
}

void SDLApp::draw_to_screen()
{
    SDL_RenderPresent(m_renderer);
}

void SDLApp::clear()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    
    SDL_Rect r;
    r.h = m_height;
    r.w = m_width;
    r.x = 0;
    r.y = 0;
    
    SDL_RenderFillRect(m_renderer, &r);
}




