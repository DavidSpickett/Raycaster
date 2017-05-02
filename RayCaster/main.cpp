#include <iostream>
#include "SDLApp.hpp"
#include "Level.hpp"

const auto win_height = 480;
const auto win_width  = 640;
const auto tick_interval = 120;
const auto turn_amount = 90;
const auto move_amount = 10;

int main(int argc, char* argv[])
{
    SDLApp app(win_width, win_height);
    Level level;
    
    SDL_Event event;
    auto run = true;
    while(run)
    {
        auto next_frame = SDL_GetTicks() + tick_interval;
        
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
                run = false;
                break;
            case SDL_KEYDOWN:
            {
                auto *state = SDL_GetKeyboardState(NULL);
                if (state[SDL_SCANCODE_ESCAPE])
                {
                    run = false;
                    break;
                }
                else if (state[SDL_SCANCODE_LEFT])
                {
                    level.m_player_pos.angle -= turn_amount;
                    if (level.m_player_pos.angle < 0)
                    {
                        level.m_player_pos.angle += 360;
                    }
                }
                else if (state[SDL_SCANCODE_RIGHT])
                {
                    level.m_player_pos.angle += turn_amount;
                    level.m_player_pos.angle %= 360;
                }
                else if (state[SDL_SCANCODE_UP])
                {
                    level.player_forward(move_amount);
                }
                else if (state[SDL_SCANCODE_DOWN])
                {
                    level.player_backward(move_amount);
                }
            }
        }
        auto lines = level.get_line_heights(win_width);
        app.draw_lines(lines);
        
        auto now = SDL_GetTicks();
        if (now < next_frame)
        {
            SDL_Delay(next_frame-now);
        }
        next_frame += tick_interval;
        
        printf("Angle: %d\n", level.m_player_pos.angle);
    }
    
    return 0;
}
