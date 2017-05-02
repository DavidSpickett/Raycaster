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
    
    void draw_lines(std::vector<float>& height_factors);

private:
    void clear();
    
    int m_width;
    int m_height;
    
    SDL_Renderer* m_renderer;
    SDL_Window* m_window;
};

#endif /* SDLApp_hpp */
