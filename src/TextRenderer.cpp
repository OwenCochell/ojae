#include <algorithm>
#include <SDL2/SDL.h>

#include "TextRenderer.h"
#include "Debug.h"

TextRenderer::TextRenderer() 
{
    font = 0;
    cursor_pos = {0, 0};
    start_x = 0;
    end_x = 0;
    start_y = 0;
    end_y = 0;
    smallest_y = 0;
    largest_y = 0;
    display_start = 0;
    time_limit = 0;
}

TextRenderer::TextRenderer(int start_x, int end_x, int start_y, int end_y,
    int time_limit) 
{
    available_fonts.push_back(16);

    font = 16;
    path = "assets/characters_font16.png";
    texture = TextureHandler::load_texture(path.c_str());
    cursor_pos = {0, 0};

    this->start_x = start_x;
    this->end_x = end_x;
    this->start_y = start_y;
    this->end_y = end_y;
    smallest_y = 0;
    largest_y = 0;
    display_start = 0;
    this->time_limit = time_limit;
}

TextRenderer::~TextRenderer() {}

int TextRenderer::get_font() { return font; }

int TextRenderer::get_largest_y() { return largest_y; }

bool TextRenderer::check_font(int font_size)
{
    /*
    Checks if a given font is available to use

    :PARAM font_size: Font to check
    */

    return std::find(available_fonts.begin(), available_fonts.end(),
        font_size) != available_fonts.end();
}

void TextRenderer::update()
{
    /*
    Called once per frame. If the time limit for the contents of this 
    text renderer does not equal -1, this function will loop through all 
    the contents and delete the members that have exceed their time stamp limit
    */

    if(time_limit == -1)
    {
        return;
    }

    Uint64 current_ticks = SDL_GetTicks64();

    if(contents.size() > 0)
    {
        smallest_y = contents.at(0).first.second.second;
    }

    else
    {
        smallest_y = 0;
    }

    // Loop through the contents and remove elements that have exceeded their
    // time stamp, and find the smallest y value
    
    for(int i = 0; i < contents.size(); i++)
    {
        if(contents.at(i).second <= current_ticks)
        {
            contents.erase(contents.begin() + i);
            i -= 1;
            continue;
        }

        if(contents.at(i).first.second.second < smallest_y)
        {
            smallest_y = contents.at(i).first.second.second;
        }
    }

    int move_up_amount = 0;

    if(display_start < smallest_y)
    {
        display_start = smallest_y;
        move_up_amount = smallest_y - display_start;
        cursor_pos.second -= move_up_amount;
    }

    for(int i = 0; i < contents.size(); i++)
    {
        contents.at(i).first.second.second -= move_up_amount;
    }

}

void TextRenderer::add(std::string new_content)
{
    /*
    Adds a string to the contents at the character position of the passed 
    coordinates 

    :PARAM new_content: Content to add
    */

    for(char c : new_content)
    {
        add(c);
    }
}

void TextRenderer::add(char new_content)    
{
    if(new_content == '\n')
    {
        // Put the cursor on the next line
        set_cursor_pos(0, cursor_pos.second += 1);
        goto CALCULATE_LARGEST_Y;
    }

    if(time_limit == -1)
    {
        contents.push_back({{new_content, std::pair<int,int>
        {
        start_x + (cursor_pos.first * font), 
        start_y + (cursor_pos.second * font * 1.5)
        }}, -1});
    }

    else
    {
        contents.push_back({{new_content, std::pair<int,int>
        {
        start_x + (cursor_pos.first * font), 
        start_y + (cursor_pos.second * font * 1.5)
        }}, SDL_GetTicks64() + time_limit});
    }

    // Increment the cursor's character position by 1
    cursor_pos.first++;
    

    // If the next character would print out of bounds
    if(start_x + (cursor_pos.first * font) > (end_x - font))
    {
        // Put the cursor on the next line
        set_cursor_pos(0, cursor_pos.second += 1);
    }

    CALCULATE_LARGEST_Y:
    if((cursor_pos.second * font * 1.5) >= largest_y)
    {
        largest_y = (cursor_pos.second * font * 1.5);

        if(largest_y - (end_y - start_y) >= 0)
        {
            display_start = largest_y - (end_y - start_y);
        }
    }

}

void TextRenderer::clear()
{
    /*
    Clears the contents and resets the cursor position to the top left
    */

    contents.clear();
    largest_y = 0;
    set_cursor_pos(0, 0);
}

void TextRenderer::set_font(int new_font)
{
    /*
    Sets the font to the new font if it is supported 

    :PARAM new_font: New font to be set
    */

    if(check_font(new_font))
    {
        font = new_font;
        path = "assets/characters_font" + std::to_string(font) + ".png";
        SDL_DestroyTexture(texture);
        texture = TextureHandler::load_texture(path.c_str());
    }
}

void TextRenderer::set_cursor_pos(int x, int y)
{
    /*
    Places the cursor in a new spot in the window if it is within bounds

    :PARAM x: X Coordinate
    :PARAM y: Y Coordinate 
    */
   
    if
    (
        x >= 0 &&
        x <= end_x - font && 
        y >= 0 &&
        y <= end_x - font
    )
    {
        cursor_pos.first = x;
        cursor_pos.second = y;
    }
}

void TextRenderer::draw_all()
{
    /*
    Loops through contents and displays the text to the screen
    */

    for(std::pair<std::pair<char, std::pair<int, int>>, int> element : contents)
    {
        char c = element.first.first;

        int x_pos = element.first.second.first;
        int y_pos = element.first.second.second;

        // If the character it wants to render is outside of the current 
        // amount of text we are displaying
        if(y_pos < display_start ||
            y_pos > display_start + end_y - start_y)
        {
            continue;
        }

        src.x = (((c - 33) % 8) * font) - 1; // Character ascii value - 65 which would make '!' be 0
        src.y = (((c - 33) / 8) * font) - 1; // Divide this by 8 so that if the ascii value exceeds 
            // a division of 8 it increments y by 1, to correspond with the png image.
        src.w = font;
        src.h = font;

        dest.x = x_pos;
        dest.y = y_pos - display_start;
        dest.w = font;
        dest.h = font;

        TextureHandler::draw(texture, src, dest); // Draw the character

    }  
}

