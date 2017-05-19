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
#include "Position.hpp"

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
            SDL_DestroyTexture(m_wall_texture);
            SDL_DestroyRenderer(m_renderer);
            SDL_DestroyWindow(m_window);
            SDL_Quit();
        }
    }
    
    void draw_lines_as_polygons(std::vector<line_height> height_factors);
    void draw_lines_from_points(std::vector<line_height> height_factors);
    void draw_line_heights(std::vector<line_height> height_factors);
    void draw_minimap(const Level& level, const std::vector<line_height>& heights);
    void draw_to_screen();
    
    bool m_use_texture;

private:
    SDL_Texture* load_texture(std::string name);
    void draw_vision_cone(const Level& level, LimitedAngle fov);
    
    void clear();
    
    int m_width;
    int m_height;
    int m_minimap_cell_size;
    
    SDL_Texture* m_wall_texture;
    SDL_Texture* m_alt_wall_texture;
    SDL_Renderer* m_renderer;
    SDL_Window* m_window;
};

#endif /* SDLApp_hpp */
