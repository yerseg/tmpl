#pragma once

#include <type_traits>
#include <utility>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>

namespace tmpl {
namespace detail {
template <typename Char, Char... C>
struct mp_string_impl
{};

template <typename Char, std::size_t Size>
constexpr Char char_at(const Char (&str)[Size], const std::size_t i)
{
    return str[i];
}

template <typename Char, typename IdxSequence, typename F, F f>
struct mp_string_from_impl;

template <typename Char, typename F, F f, typename T, T... Idxs>
struct mp_string_from_impl<Char, std::index_sequence<Idxs...>, F, f>
{
    template <typename I>
    using char_at_idx = std::integral_constant<Char, f(I::value)>;

    template <typename L>
    struct to_mp_string;

    template <typename... Indexes>
    struct to_mp_string<boost::mp11::mp_list<Indexes...>>
    {
        using type = mp_string_impl<Char, Indexes::value...>;
    };

    using type = typename to_mp_string<
        boost::mp11::mp_transform<char_at_idx, boost::mp11::mp_list<std::integral_constant<decltype(Idxs), Idxs>...>>>::type;
};

template <typename Char, size_t Size, typename F, F f>
struct mp_string_from
{
    using type = typename mp_string_from_impl<Char, std::make_index_sequence<Size>, F, f>::type;
};

template <typename F, typename Char, size_t Size>
constexpr auto mp_string_from_f(Char (&str)[Size], F f)
{
    return typename mp_string_from<Char, Size, F, f>::type{};
};

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

#define TMPL_MP_STRING(literal)                                                                                                            \
    decltype(::tmpl::detail::mp_string_from_f(                                                                                             \
        (literal), [](const std::size_t i) constexpr { return ::tmpl::detail::char_at((literal), i); }))
