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
                }
            }
        }
        
        level.apply_movement(state);

        auto lines = level.get_line_heights(win_width);
        app.draw_lines(lines);
        app.draw_2d_map(level);
        app.draw_to_screen();
        
        auto now = SDL_GetTicks();
        if (now < next_frame)
        {
            SDL_Delay(next_frame-now);
        }
        next_frame += tick_interval;
        
        //printf("Angle: %f\n", level.m_player_pos.angle.GetValue());
    }
    
    return 0;
}
