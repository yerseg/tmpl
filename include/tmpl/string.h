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

template <typename F, F f, typename Char, size_t Size>
struct mp_string_from
{
    using type = typename mp_string_from_impl<Char, std::make_index_sequence<Size>, F, f>::type;
};

template <typename Char, size_t Size>
struct literal_char_type
{
    using type = Char;

    constexpr literal_char_type(const Char (&)[Size]);
};

}  // namespace detail


template <char... C>
using mp_string = detail::mp_string_impl<char, C...>;

#if defined(__cpp_char8_t)
template <char8_t... C>
using mp_string8 = detail::mp_string_impl<char8_t, C...>;
#else
template <char... C>
using mp_string8 = detail::mp_string_impl<char, C...>;
#endif

template <char16_t... C>
using mp_string16 = detail::mp_string_impl<char16_t, C...>;

template <char32_t... C>
using mp_string32 = detail::mp_string_impl<char32_t, C...>;

template <wchar_t... C>
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
    ::tmpl::detail::mp_string_from<                                                                                                        \
        decltype(::tmpl::detail::literal_char_type(literal))::type (*)(const std::size_t),                                                 \
        [](const std::size_t i) constexpr { return (literal)[i]; },                                                                        \
        decltype(::tmpl::detail::literal_char_type(literal))::type,                                                                        \
        std::size(literal)>::type
