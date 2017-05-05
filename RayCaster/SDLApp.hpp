//
//  SDLApp.hpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#ifndef SDLApp_hpp
#define SDLApp_hpp

#include <stdio.h>
#include <SDL2/SDL.h>
#include <vector>
#include "Level.hpp"

const auto SCREEN_FPS = 60;
const auto SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

class SDLApp
{
public:
    SDLApp(int width, int height);
    
    ~SDLApp()
    {
        if (m_window != NULL)
        {
            SDL_DestroyRenderer(m_renderer);
            SDL_DestroyWindow(m_window);
            SDL_Quit();
        }
    }
    
    void draw_lines_as_polygons(std::vector<float> height_factors);
    void draw_lines_from_points(std::vector<float> height_factors);
    void draw_line_heights(std::vector<float> height_factors);
    void draw_minimap(const Level& level);
    void draw_to_screen();

private:
    void draw_vision_cone(const Level& level, LimitedAngle fov);
    
    void clear();
    
    int m_width;
    int m_height;
    int m_minimap_cell_size;
    
    SDL_Renderer* m_renderer;
    SDL_Window* m_window;
};

#endif /* SDLApp_hpp */
