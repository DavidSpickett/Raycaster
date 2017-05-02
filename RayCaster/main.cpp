#include <iostream>
#include "SDLApp.hpp"
#include "Level.hpp"

const auto win_height = 480;
const auto win_width  = 640;

int main(int argc, char* argv[])
{
    SDLApp app(win_width, win_height);
    Level level;
    
    SDL_Event event;
    auto run = true;
    while(run)
    {
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
            }
        }
        auto lines = level.get_line_heights(win_width);
        app.draw_lines(lines);
    }
    
    return 0;
}
