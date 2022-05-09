#ifndef YAN_COLORER_H
#define YAN_COLORER_H

#include <string>
#include <vector>

namespace colorer
{
    enum class gradient {rainbow};

    struct generate_options
    {
        gradient colors;
        float fuzz = 0;
        float offset = 0;
    };

    struct color
    {
        float r;
        float g;
        float b;

        static std::wstring hex_single(const float col);
        std::wstring hex() const;
    };

    bool whitespace(const wchar_t literal);

    float divide_gradient(const float divisor, const int mult, const float value, const float fuzz);

    std::vector<color> generate_colors(const std::wstring text, const generate_options options);
    std::wstring generate_codes(const std::wstring text, const std::vector<color> colors);
};

#endif
