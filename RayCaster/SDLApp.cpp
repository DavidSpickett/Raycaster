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

void SDLApp::draw_lines_as_polygons(std::vector<float> height_factors)
{
    clear();
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    auto midscreen = m_height/2;
    
    std::vector<int> last_heights;
    std::vector<SDL_Point> forward_points;
    std::vector<SDL_Point> reverse_points;
    
    for (auto x=0; x < height_factors.size(); ++x)
    {
        int line_height = (m_height*height_factors[x])/2;
        
        if (line_height && (x != (height_factors.size()-1)))
        {
            //Got enough to check the trend
            if (last_heights.size() == 2)
            {
                //Check whether trend has changed
                if (
                    //Was decreasing, new line is taller
                    ((last_heights[0] > last_heights[1]) &&
                    (line_height > last_heights[1]))
                    ||
                    //Was increasing, new line is shorter
                    ((last_heights[0] < last_heights[1]) &&
                     ((line_height < last_heights[1])))
                    ||
                    //Straight line starting to go up OR down
                    ((last_heights[0] == last_heights[1]) &&
                     (line_height != last_heights[1]))
                    )
                {
                    //Add a vertical line here (must be a corner)
                    auto top_point = SDL_Point{x, midscreen-line_height};
                    auto bottom_point = SDL_Point{x, midscreen+line_height};
                    
                    //Draw down then go back up
                    forward_points.push_back(top_point);
                    forward_points.push_back(bottom_point);
                    forward_points.push_back(top_point);
                    
                    reverse_points.push_back(bottom_point);
                    
                    //Start a new trend history
                    last_heights.clear();
                    last_heights.push_back(line_height);
                }
                else
                {
                    //Remove older height
                    std::swap(last_heights[0], last_heights[1]);
                    last_heights.pop_back();
                    last_heights.push_back(line_height);
                }
            }
            else
            {
                //Starting first or new polygon
                if (last_heights.size() == 0)
                {
                    forward_points.push_back(SDL_Point{x, midscreen-line_height});
                    reverse_points.push_back(SDL_Point{x, midscreen+line_height});
                }
                
                last_heights.push_back(line_height);
            }
        }
        //TODO: is there some way not to draw the far right edge? technically it shouldn't join up
        else if ((line_height == 0) || (x == (height_factors.size()-1)))
        {
            //Either the wall ended or we've hit the end of the screen
            //Or we never started a polygon at all
            if (forward_points.size())
            {
                //Draw this polygon
                
                //If the line height just became 0, the shape should have ended on the previous X
                auto last_x = x;
                auto last_line_height = line_height;
                if (line_height == 0)
                {
                    last_x -= 1;
                    last_line_height = last_heights.back();
                }
                
                forward_points.push_back(SDL_Point{last_x, midscreen-last_line_height});
                reverse_points.push_back(SDL_Point{last_x, midscreen+last_line_height});
                
                //Add the fwd and reverse lists together with the reverse list reversed
                //As if we went from top left to top right, then bottom right to bottom left
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
            }
        }
    }
    
}

void SDLApp::draw_lines_from_points(std::vector<float> height_factors)
{
    clear();
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    auto midscreen = m_height/2;
    
    /*This can be further optimised. For each of the chunks we make we only
    the first two points on the left and then a point each time the trend of
     the line changes. If it was getting lower then starts going up, add a point.
    
    AND!!!!!!
    Each time we place a mid point because the line changed gradient, that MUST BE
    a corner of some kind! So we can draw a vertical line there to signify corners!!!!*/
    
    using LineChunk = std::pair<int, std::vector<int>>;
    std::vector<LineChunk> chunks;
    LineChunk current_chunk = std::make_pair(0, std::vector<int>());
    
    for (auto x=0; x != height_factors.size(); ++x)
    {
        int line_height = (m_height*height_factors[x])/2;
        
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

void SDLApp::draw_line_heights(std::vector<float> height_factors)
{
    clear();
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    auto midscreen = m_height/2;
    for (auto x=0; x != height_factors.size(); ++x)
    {
        // Divide by 2 because we draw above and below the middle
        int line_height = (m_height*height_factors[x])/2;
        
        // Fade out with distance
        auto alpha = std::max(float(10), 255*height_factors[x]);
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, alpha);
        
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




