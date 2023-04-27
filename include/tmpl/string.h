#pragma once

namespace tmpl {
namespace detail {
template <typename Char, Char... C>
struct mp_string_impl
{};
}  // namespace detail


template <char... C>
using mp_string = detail::mp_string_impl<char, C...>;

template <char8_t... C>
using mp_string8 = detail::mp_string_impl<char8_t, C...>;

template <char8_t... C>
using mp_string16 = detail::mp_string_impl<char16_t, C...>;

template <char8_t... C>
using mp_string32 = detail::mp_string_impl<char32_t, C...>;

template <char8_t... C>
using mp_wstring = detail::mp_string_impl<wchar_t, C...>;

template <typename T>
struct mp_c_str;

template <typename Char, Char... C>
struct mp_c_str<detail::mp_string_impl<Char, C...>>
{
    static inline constexpr Char value[sizeof...(C) + 1] = {C..., Char{'\0'}};
};

template <typename T>
inline constexpr auto mp_c_str_v = mp_c_str<T>::value;

}  // namespace tmpl

#define TMPL_MP_STRING(literal)
