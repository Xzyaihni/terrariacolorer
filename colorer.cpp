#include "colorer.h"

#include <iostream>

std::wstring colorer::color::hex_single(const float col)
{
    char color_str[2];
    sprintf(color_str, "%02x", static_cast<uint8_t>(col*255));

    wchar_t color_wstr[2];
    std::mbstowcs(color_wstr, color_str, 2);

    return std::wstring(color_wstr, 2);
}

std::wstring colorer::color::hex() const
{
    return hex_single(r) + hex_single(g) + hex_single(b);
}

bool colorer::whitespace(const wchar_t literal)
{
    return static_cast<unsigned>(literal)==32;
}

float colorer::divide_gradient(const float divisor, const int mult, const float value, const float fuzz)
{
    float lower_edge = divisor*(mult);
    if(lower_edge<0)
        ++lower_edge;

    float upper_edge = divisor*(mult+1);
    if(upper_edge>1)
        --upper_edge;

    float start_distance = value-lower_edge;
    start_distance = start_distance<0 ? 1+start_distance : start_distance;

    if(start_distance<1)
    {
        const float closest_distance = start_distance<0.5f ?
            start_distance : 1-start_distance;

        return std::clamp(closest_distance*(1/fuzz), 0.0f, 1.0f);
    } else
    {
        return 0;
    }
}

std::vector<colorer::color> colorer::generate_colors(const std::wstring text, const generate_options options)
{
    std::vector<color> output_colors;
    output_colors.reserve(text.length());

    int whitespace_count = 0;
    for(int i = 0; i < text.length(); ++i)
    {
        if(whitespace(text[i]))
            ++whitespace_count;
    }


    const float color_shift = 1.0f/(text.length()-whitespace_count);
    float c_color_offset = options.offset;

    const int colors_amount = 3;
    const float color_divisor = 1.0f/colors_amount;

    for(int i = 0; i < text.length(); ++i)
    {
        //if whitespace, dont advance color
        if(!whitespace(text[i]))
        {
            c_color_offset += color_shift;
            if(c_color_offset>=1)
                --c_color_offset;
        }

        const color c_color{divide_gradient(color_divisor, 0, c_color_offset, options.fuzz),
            divide_gradient(color_divisor, 1, c_color_offset, options.fuzz),
            divide_gradient(color_divisor, 2, c_color_offset, options.fuzz)};

        output_colors.emplace_back(c_color);
    }

    return output_colors;
}

std::wstring colorer::generate_codes(const std::wstring text, const std::vector<color> colors)
{
    std::wstring output_text;

    for(int i = 0; i < text.length(); ++i)
    {
        if(!whitespace(text[i]))
        {
            output_text += L"[c/";

            output_text += colors[i].hex();

            output_text += ':';

            output_text += text[i];

            output_text += ']';
        } else
        {
            output_text += text[i];
        }
    }

    return output_text;
}
