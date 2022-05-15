#ifndef YAN_COLORER_H
#define YAN_COLORER_H

#include <string>
#include <vector>

#include <lua.hpp>

class lua_exception : public std::exception
{
public:
    lua_exception(const std::string message);

    const char* what() const noexcept override;
private:
    std::string _message;
};

class colorer
{
public:
    struct generate_options
    {
        float fuzz = 0;
        float offset = 0;
    };

    struct color
    {
        float r;
        float g;
        float b;

        static std::wstring hex_single(const float col) noexcept;
        std::wstring hex() const noexcept;
    };

    colorer();
    colorer(const colorer&) = default;
    colorer(colorer&&) noexcept = default;
    colorer& operator=(const colorer&) = default;
    colorer& operator=(colorer&&) noexcept = default;

    ~colorer();

    void update_lua(const std::string filename);

    std::vector<color> generate_colors(const std::wstring text, const generate_options options) const;
    static std::wstring generate_codes(const std::wstring text, const std::vector<color> colors) noexcept;

    bool updated = false;

private:
    color divide_gradient(const float value, const float fuzz) const;
    static bool whitespace(const wchar_t literal) noexcept;

    lua_State* _lua_state = nullptr;
};

#endif
