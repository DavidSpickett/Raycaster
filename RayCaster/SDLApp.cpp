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
    m_width(width), m_height(height), m_minimap_cell_size(10)
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

namespace
{
    int to_minimap_coord(int coord, const Level& level, int cell_size)
    {
        return (float(coord)/level.m_tile_side) * cell_size;
    }
}

void SDLApp::draw_minimap(const Level& level)
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
    
    draw_vision_cone(level, level.m_player_fov);
    
    //Player position
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
    SDL_Point player_pos = level.m_player_pos.to_minimap_point(level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    
    SDL_Rect player;
    player.x = player_pos.x;
    player.y = player_pos.y;
    player.w = 2;
    player.h = 2;
    SDL_RenderDrawRect(m_renderer, &player);
}

void SDLApp::draw_vision_cone(const Level& level, LimitedAngle fov)
{
    const auto vision_length = 500; //In full size map pixels
    
    SDL_Point points[4];
    
    //Cone itself
    Position left_extent(level.m_player_pos);
    left_extent.angle -= level.m_player_fov.GetValue()/2;
    left_extent += vision_length;
    
    Position right_extent(level.m_player_pos);
    right_extent.angle += level.m_player_fov.GetValue()/2;
    right_extent += vision_length;
    
    points[0] = level.m_player_pos.to_minimap_point(level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    points[1] = left_extent.to_minimap_point(level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    points[2] = right_extent.to_minimap_point(level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    points[3] = points[0];
    
    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    SDL_RenderDrawLines(m_renderer, points, 4);
    
    //Then a shorter line to show the heading
    const auto heading_length = 300;
    SDL_Point start_heading = level.m_player_pos.to_minimap_point(level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    SDL_Point end_heading = (level.m_player_pos+heading_length).to_minimap_point(level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
    SDL_RenderDrawLine(m_renderer,
        start_heading.x, start_heading.y,
        end_heading.x, end_heading.y);
}

void SDLApp::draw_lines(std::vector<float> height_factors)
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




