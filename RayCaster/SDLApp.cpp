//
//  SDLApp.cpp
//  RayCaster
//
//  Created by David Spickett on 02/05/2017.
//  Copyright © 2017 David Spickett. All rights reserved.
//

#include <stdexcept>
#include <string>
#include "SDLApp.hpp"

namespace
{
    SDL_Point pos_to_minimap_pos(Position pos, int level_height, int tile_size, int cell_size)
    {
        return SDL_Point{
            static_cast<int>((float(pos.x)/tile_size) * cell_size),
            static_cast<int>((float(level_height-pos.y)/tile_size) * cell_size)
        };
    }
}

SDLApp::SDLApp(int width, int height):
    m_width(width), m_height(height), m_minimap_cell_size(30), m_use_texture(true)
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
    m_wall_texture = load_texture("wall.bmp");
}

SDL_Texture* SDLApp::load_texture(std::string name)
{
    auto bmp = SDL_LoadBMP(name.c_str());
    auto texture = SDL_CreateTextureFromSurface(m_renderer, bmp);
    SDL_FreeSurface(bmp);
    return texture;
}

void SDLApp::draw_minimap(const Level& level, const std::vector<line_height>& heights)
{
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
            r.h = m_minimap_cell_size;
            r.w = m_minimap_cell_size;
            r.x = x*m_minimap_cell_size;
            r.y = y*m_minimap_cell_size;
            
            SDL_RenderFillRect(m_renderer, &r);
        }
    }
    
    //Gridlines
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
    for (auto x=0; x != MAP_SIDE; ++x)
    {
        for (auto y=0; y != MAP_SIDE; ++y)
        {
            std::array<SDL_Point, 3> points;
            
            //Bottom left
            points[0].x = x*m_minimap_cell_size;
            points[0].y = (y+1)*m_minimap_cell_size;
            
            //Top left
            points[1].x = x*m_minimap_cell_size;
            points[1].y = y*m_minimap_cell_size;
            
            //Top right
            points[2].x = (x+1)*m_minimap_cell_size;
            points[2].y = y*m_minimap_cell_size;
            
            SDL_RenderDrawLines(m_renderer, &points[0], points.size());
        }
    }
    
    //Right/bottom side of minimap
    std::array<SDL_Point, 3> points;
    
    points[0].x = m_minimap_cell_size*MAP_SIDE;
    points[0].y = 0;
    
    points[1].x = m_minimap_cell_size*MAP_SIDE;
    points[1].y = m_minimap_cell_size*MAP_SIDE;
    
    points[2].x = 0;
    points[2].y = m_minimap_cell_size*MAP_SIDE;
    
    SDL_RenderDrawLines(m_renderer, &points[0], points.size());
    
    draw_vision_cone(level, level.m_player_fov);
    
    //Player position
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
    SDL_Point player_pos = pos_to_minimap_pos(level.m_player_pos,
        level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    
    SDL_Rect player;
    player.x = player_pos.x;
    player.y = player_pos.y;
    player.w = 2;
    player.h = 2;
    SDL_RenderDrawRect(m_renderer, &player);
    
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
    std::for_each(heights.begin(), heights.end(), [=](const line_height& lh)
    {
        std::for_each(lh.points_checked.begin(), lh.points_checked.end(), [=](const SDL_Point& point)
        {
            SDL_RenderDrawPoint(m_renderer,
                                (double(point.x)/level.m_tile_side)*m_minimap_cell_size,
                                (double(point.y)/level.m_tile_side*m_minimap_cell_size)
                                );
        });
    });
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
    
    points[0] = pos_to_minimap_pos(level.m_player_pos,
            level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    points[1] = pos_to_minimap_pos(left_extent,
            level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    points[2] = pos_to_minimap_pos(right_extent,
                level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    points[3] = points[0];
    
    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    SDL_RenderDrawLines(m_renderer, points, 4);
    
    //Then a shorter line to show the heading
    const auto heading_length = 300;
    SDL_Point start_heading = pos_to_minimap_pos(level.m_player_pos,
            level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    SDL_Point end_heading = pos_to_minimap_pos((level.m_player_pos+heading_length),
            level.m_map_height, level.m_tile_side, m_minimap_cell_size);
    
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
    SDL_RenderDrawLine(m_renderer,
        start_heading.x, start_heading.y,
        end_heading.x, end_heading.y);
}

void SDLApp::draw_lines_as_polygons(std::vector<line_height> height_factors)
{
    clear();
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    auto midscreen = m_height/2;
    
    std::vector<int> last_heights;
    std::vector<SDL_Point> forward_points;
    std::vector<SDL_Point> reverse_points;
    //Number of points equal to the last stored height aka last_heights[1]
    auto equal_heights_run = 0;
    
    for (auto x=0; x < height_factors.size(); ++x)
    {
        //Divide by two for each side of the horizon line
        int line_height = (m_height*height_factors[x].height)/2;
        
        if (line_height && (x != (height_factors.size()-1)))
        {
            //Got enough to check the trend
            if (last_heights.size() == 2)
            {
                //Was decreasing, new line is taller
                auto was_decreasing = ((last_heights[0] > last_heights[1]) &&
                 (line_height > last_heights[1]));
                
                //Was increasing, new line is shorter
                auto was_increasing = ((last_heights[0] < last_heights[1]) && ((line_height < last_heights[1])));
                
                //If the trend has changed
                if (was_increasing || was_decreasing)
                {
                    /*
                     Add a vertical line when gradient changes (corners)
                     X coord is half way back through the last run of equal heights if we had one.
                    
                     All points renderer might look like this:
                           -------
                     ------       -----
                     We want to draw a corner somewhere there. By counting the run of highest lines
                     we draw it here:
                           -------
                     ------       -----
                              ^
                     Not here:
                           -------
                     ------       -----
                                 ^
                     */
                    //-1 because this line caused the change but we want the previous height
                    auto vert_x = x-1-(equal_heights_run/2);
                    auto vert_line_height = last_heights.back();
                    
                    auto top_point = SDL_Point{vert_x, midscreen-vert_line_height};
                    auto bottom_point = SDL_Point{vert_x, midscreen+vert_line_height};
                    
                    //Draw down then go back up
                    forward_points.push_back(top_point);
                    forward_points.push_back(bottom_point);
                    forward_points.push_back(top_point);
                    
                    reverse_points.push_back(bottom_point);
                    
                    //Start a new trend history
                    last_heights.clear();
                    last_heights.push_back(line_height);
                    equal_heights_run = 0;
                }
                else if (line_height != last_heights.back())
                {
                    //Remove older height
                    std::swap(last_heights[0], last_heights.back());
                    last_heights.pop_back();
                    
                    last_heights.push_back(line_height);
                    equal_heights_run = 0;
                }
                else if (line_height == last_heights.back())
                {
                    /*
                    We don't count runs of equal as part of the trend, but count
                    them so we can go back to place corners
                    */
                    equal_heights_run += 1;
                }
            }
            else
            {
                //Starting first/new polygon
                if (last_heights.size() == 0)
                {
                    forward_points.push_back(SDL_Point{x, midscreen-line_height});
                    reverse_points.push_back(SDL_Point{x, midscreen+line_height});
                }
                
                last_heights.push_back(line_height);
            }
        }
        else if ((line_height == 0) || (x == (height_factors.size()-1)))
        {
            //Either the wall ended or we've hit the end of the screen
            //Or we never started a polygon at all
            if (forward_points.size())
            {
                //Draw this polygon
 
                //If the line height just became 0, the shape should have ended on the previous X
                //Otherwise we're on the edge of the screen
                auto last_x = x;
                auto last_line_height = line_height;
                if (line_height == 0)
                {
                    last_x -= 1;
                    last_line_height = last_heights.back();
                }
                
                forward_points.push_back(SDL_Point{last_x, midscreen-last_line_height});
                reverse_points.push_back(SDL_Point{last_x, midscreen+last_line_height});
                
                /*
                Add the fwd and reverse lists together with the reverse list reversed
                As if we went from top left to top right, then bottom right to bottom left
                */
                std::vector<SDL_Point> all_points;
                all_points.reserve(forward_points.size()+reverse_points.size());
                all_points.insert(all_points.end(), forward_points.begin(), forward_points.end());
                //Note: reverse begin
                all_points.insert(all_points.end(), reverse_points.rbegin(), reverse_points.rend());
                
                //Join up the two ends of the polgyon (bottom left to top left)
                all_points.push_back(all_points[0]);
                
                SDL_RenderDrawLines(m_renderer, &all_points[0], static_cast<int>(all_points.size()));
                
                last_heights.clear();
                forward_points.clear();
                reverse_points.clear();
                equal_heights_run = 0;
            }
        }
    }
    
}

void SDLApp::draw_lines_from_points(std::vector<line_height> height_factors)
{
    clear();
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    auto midscreen = m_height/2;
    
    using LineChunk = std::pair<int, std::vector<int>>;
    std::vector<LineChunk> chunks;
    LineChunk current_chunk = std::make_pair(0, std::vector<int>());
    
    for (auto x=0; x != height_factors.size(); ++x)
    {
        int line_height = (m_height*height_factors[x].height)/2;
        
        if (line_height != 0)
        {
            if (current_chunk.second.size())
            {
                current_chunk.second.push_back(line_height);
            }
            else
            {
                current_chunk = std::make_pair(x, std::vector<int>{line_height});
            }
        }
        else
        {
            if (current_chunk.second.size())
            {
                chunks.push_back(current_chunk);
                current_chunk.second.clear();
            }
        }
    }
    
    if (current_chunk.second.size())
    {
        chunks.push_back(current_chunk);
    }
    
    //Now we have runs of non 0 line heights, draw them.
    std::for_each(chunks.begin(), chunks.end(), [=](LineChunk& chunk)
    {
        std::vector<SDL_Point> points;
        
        //1st we go forward in the list (start at top left)
        auto x = chunk.first;
        std::vector<int>::iterator it=chunk.second.begin();
        for ( ; it != chunk.second.end(); ++it, ++x)
        {
            points.push_back(SDL_Point{x, midscreen-*it});
        }
        
        //Then go backwards from the bottom right
        x = chunk.first + static_cast<int>(chunk.second.size()) -1;
        std::vector<int>::reverse_iterator r_it = chunk.second.rbegin();
        for ( ; r_it != chunk.second.rend(); ++r_it, --x)
        {
            points.push_back(SDL_Point{x, midscreen+*r_it});
        }
        
        //Connect the two parts
        points.push_back(points[0]);
        
        SDL_RenderDrawLines(m_renderer, &points[0], static_cast<int>(points.size()));
    });
}

void SDLApp::draw_line_heights(std::vector<line_height> height_factors)
{
    clear();
    auto midscreen = m_height/2;
    
    //Sky
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 100, 255);
    SDL_Rect sky_rect{0, 0, m_width, midscreen};
    SDL_RenderFillRect(m_renderer, &sky_rect);
    
    for (auto x=0; x != height_factors.size(); ++x)
    {
        // Divide by 2 because we draw above and below the middle
        int line_height = (m_height*height_factors[x].height)/2;
        
        // Make colours darker with distance (scaling alpha would show the sky through walls)
        auto colour_val = std::max(float(10), 255*height_factors[x].height);
        
        if (m_use_texture)
        {
            if (height_factors[x].vertical_intersect)
            {
                //Make Y intersected walls always darker
                colour_val /= 2;
            }
            
            SDL_SetTextureColorMod(m_wall_texture, colour_val, colour_val, colour_val);
            
            auto source_rect = SDL_Rect{height_factors[x].texture_offset, 0, 1, 500};
            auto dest_rect = SDL_Rect{x, midscreen-line_height, 1, line_height*2};
            SDL_RenderCopy(m_renderer,
                           m_wall_texture,
                           &source_rect,
                           &dest_rect);
        }
        else
        {
            if (height_factors[x].vertical_intersect)
            {
                SDL_SetRenderDrawColor(m_renderer, 0, colour_val, 0, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(m_renderer, colour_val, 0, 0, 255);
            }
            
            if (line_height)
            {
                SDL_RenderDrawLine(m_renderer,
                                   x, midscreen-line_height,
                                   x, midscreen+line_height);
            }
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




