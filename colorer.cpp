#include <iostream>
#include <array>
#include <cstring>

#include "colorer.h"


lua_exception::lua_exception(const std::string message)
: _message(message)
{
}

const char* lua_exception::what() const noexcept
{
    return _message.c_str();
}

std::wstring colorer::color::hex_single(const float col) noexcept
{
    char color_str[3];
    sprintf(color_str, "%02x", static_cast<uint8_t>(col*255));

    wchar_t color_wstr[2];
    std::mbstowcs(color_wstr, color_str, 2);

    return std::wstring(color_wstr, 2);
}

std::wstring colorer::color::hex() const noexcept
{
    return hex_single(r) + hex_single(g) + hex_single(b);
}

colorer::colorer()
{
}

colorer::~colorer()
{
    if(_lua_state!=nullptr)
        lua_close(_lua_state);
}

void colorer::update_lua(const std::string filename)
{
    if(_lua_state!=nullptr)
        lua_close(_lua_state);

    updated = false;


    _lua_state = luaL_newstate();

    luaL_openlibs(_lua_state);

    const int error_state =
        luaL_loadfile(_lua_state, filename.data())
        || lua_pcall(_lua_state, 0, 0, 0);

    if(error_state)
    {
        const std::string error_string(lua_tostring(_lua_state, -1));
        lua_pop(_lua_state, 1);

        throw lua_exception(error_string);
    }
}

std::vector<colorer::color> colorer::generate_colors(const std::wstring text, const generate_options options) const
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

    for(int i = 0; i < text.length(); ++i)
    {
        //if whitespace, dont advance color
        if(!whitespace(text[i]))
        {
            c_color_offset += color_shift;
            if(c_color_offset>=1)
                --c_color_offset;
        }

        output_colors.emplace_back(divide_gradient(c_color_offset, options.fuzz));
    }

    return output_colors;
}

std::wstring colorer::generate_codes(const std::wstring text, const std::vector<color> colors) noexcept
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

colorer::color colorer::divide_gradient(const float value, const float fuzz) const
{
    if(_lua_state==nullptr)
        return {0, 0, 0};

    lua_getglobal(_lua_state, "color_func");
    lua_pushnumber(_lua_state, value);
    lua_pushnumber(_lua_state, fuzz);

    if(lua_pcall(_lua_state, 2, 1, 0) || !lua_istable(_lua_state, -1))
       throw lua_exception(lua_tostring(_lua_state, -1));

    auto table_val = [this](const char* key)
    {
        lua_pushstring(_lua_state, key);
        lua_gettable(_lua_state, -2);

        const float table_num = static_cast<float>(lua_tonumber(_lua_state, -1));

        lua_pop(_lua_state, 1);

        return table_num;
    };

    const color c_color{table_val("r"), table_val("g"), table_val("b")};

    lua_pop(_lua_state, 1);

    return c_color;
}

bool colorer::whitespace(const wchar_t literal) noexcept
{
    return static_cast<unsigned>(literal)==32;
}
