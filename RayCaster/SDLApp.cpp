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

void SDLApp::draw_lines(std::vector<int>& heights)
{
    clear();
    auto midscreen = m_height/2;
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    
    for (auto x=0; x != heights.size(); ++x)
    {
        SDL_RenderDrawLine(m_renderer,
            x, midscreen-(heights[x]/2),
            x, midscreen+(heights[x]/2));
    }

    SDL_RenderPresent(m_renderer);
}

void SDLApp::clear()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    
    SDL_Rect r;
    r.h = m_height;
    r.w = m_width;
    r.x = 0;
    r.y = 0;
    
    SDL_RenderFillRect(m_renderer, &r);
}




