#include <iostream>
#include "SDLApp.hpp"
#include "Level.hpp"

const auto win_height = 480;
const auto win_width  = 640;
const auto tick_interval = 60;
const auto fov_change_amount = 10;

int main(int argc, char* argv[])
{
    SDLApp app(win_width, win_height);
    Level level;
    auto video_mode = 0;
    
    SDL_Event event;
    auto run = true;
    while(run)
    {
        auto next_frame = SDL_GetTicks() + tick_interval;
        auto *state = SDL_GetKeyboardState(NULL);
        
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    run = false;
                    break;
                case SDL_KEYDOWN:
                {
                    if (state[SDL_SCANCODE_ESCAPE])
                    {
                        run = false;
                        break;
                    }
                    else if (state[SDL_SCANCODE_LEFTBRACKET])
                    {
                        level.m_player_fov += fov_change_amount;
                    }
                    else if (state[SDL_SCANCODE_RIGHTBRACKET])
                    {
                        level.m_player_fov -= fov_change_amount;
                    }
                    else if (state[SDL_SCANCODE_EQUALS])
                    {
                        video_mode = video_mode == 2 ? 0 : video_mode+1;
                        printf("Video mode %d\n", video_mode);
                    }
                    else if (state[SDL_SCANCODE_MINUS])
                    {
                        video_mode = video_mode == 0 ? 2 : video_mode-1;
                        printf("Video mode %d\n", video_mode);
                    }
                }
            }
        }
        
        level.apply_movement(state);
        
        /*
        TODO:
        When you look into the distance and two shapes overlap we should
        know how to draw an edge to differentiate them.
         This would also break the shape and make the size difference more
         obvious.
        
        Perpsecitve correct the rays so that you don't get corners in the middle
        of a wall close up.
        */
        
        auto heights = level.get_line_heights(win_width);
        switch (video_mode)
        {
            case 0:
                app.draw_line_heights(heights);
                break;
            case 1:
                app.draw_lines_from_points(heights);
                break;
            case 2:
                app.draw_lines_as_polygons(heights);
                break;
        }
        
        app.draw_minimap(level);
        app.draw_to_screen();
        
        auto now = SDL_GetTicks();
        if (now < next_frame)
        {
            SDL_Delay(next_frame-now);
        }
        next_frame += tick_interval;
    }
    
    return 0;
}
