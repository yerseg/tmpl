#pragma once

#include <cstdint>
#include <type_traits>

namespace tmpl {

enum class Options : uint32_t
{
    Opt1 = 0x1,
    Opt2 = 0x2,
    Opt3 = 0x4,
    Opt4 = 0x8,
    Opt5 = 0x10,
    Opt6 = 0x20,
    Opt7 = 0x40
};

constexpr Options operator|(Options a, Options b)
{
    return static_cast<Options>(static_cast<std::underlying_type_t<decltype(a)>>(a) | static_cast<std::underlying_type_t<decltype(a)>>(b));
}

}  // namespace tmpl
