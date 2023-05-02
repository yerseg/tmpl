#pragma once

#include "context.h"
#include "options.h"
#include "string.h"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>

#include <concepts>
#include <type_traits>

namespace tmpl {

namespace detail {

enum class ParameterTag
{
    Generic,
    String,
    Type,
    Foo,
    FooPtr,
    Id,
    Callback,
    Ptr
};

template <
    typename Parameter,
    ParameterTag ParamTag = ParameterTag::Generic,
    bool RuntimeFieldEnable_ = false,
    typename GTag = TMPL_MP_STRING(
        "__default_tag__eb7ed569-14be-47e1-bda6-94a86cee8222"                 // tag + random GUID
        "23e693c4981b550f700e74fd9ab88ad8df4bec28854c09228dd17589c56b7efb")>  // SHA256("__default_tag__eb7ed569-14be-47e1-bda6-94a86cee8222")
    requires same_as_mp_string<GTag>
struct ParameterHolder
{
    using Tag = std::integral_constant<ParameterTag, ParamTag>;
    using GenericTag = GTag;
    using Type = Parameter;
    using RuntimeFieldEnable = std::bool_constant<RuntimeFieldEnable_>;
};

template <ParameterTag Tag>
struct IsParameterTagMatch
{
    template <typename U>
    using fn = std::bool_constant<U::Tag::value == Tag>;
};

template <typename GenericTag>
    requires same_as_mp_string<GenericTag>
struct IsGenericTagMatch
{
    template <typename U>
    using fn = std::is_same<typename U::GenericTag, GenericTag>;
};

template <ParameterTag Tag, typename... Parameters>
struct ParametersGetterImpl
{
    using ParametersList = boost::mp11::mp_list<Parameters...>;
    using Index = boost::mp11::mp_find_if_q<ParametersList, IsParameterTagMatch<Tag>>;
    using IsNotExist = std::is_same<Index, boost::mp11::mp_size<ParametersList>>;
};

template <ParameterTag Tag, typename... Parameters>
struct ParametersGetter
{
    using Impl = ParametersGetterImpl<Tag, Parameters...>;

    static_assert(!Impl::IsNotExist::value, "Assertion error: there is no value with Tag in the parameter pack!");

    using holder = boost::mp11::mp_at<typename Impl::ParametersList, typename Impl::Index>;
    using type = typename holder::Type;
};

template <typename T>
struct ContextField
{
    static inline T field{};
};

template <>
struct ContextField<void>
{};

template <typename Param>
struct FieldsProvider
    : public ContextField<
          std::conditional_t<std::is_same_v<typename Param::RuntimeFieldEnable, std::true_type>, typename Param::Type, void>>
{};

template <Options Opts, typename... Parameters>
struct ContextBase : public FieldsProvider<Parameters>...
{
    // static_assert(
    //     std::is_same_v<
    //         typename ParametersGetter<ParameterTag::Type, Parameters...>::type,
    //         typename ParametersGetter<ParameterTag::Cb, Parameters...>::type::value_type>,
    //     "Assertion error");

    template <ParameterTag Tag>
    static constexpr auto Get()
    {
        return ParametersGetter<Tag, Parameters...>::type::value;
    }

    template <ParameterTag Tag>
    static constexpr auto& GetField()
    {
        return FieldsProvider<typename ParametersGetter<Tag, Parameters...>::holder>::field;
    }

    static constexpr auto GetOptions() { return Opts; }
};

template <Options Opts, typename... Parameters>
struct ContextImpl : ContextBase<Opts, Parameters...>
{};

template <typename... Parameters>
struct ContextImpl<Options::Opt7, Parameters...> : ContextBase<Options::Opt7, Parameters...>
{
    static inline void** some_field_2 = nullptr;
};

template <Options Opts>
struct ToContextImpl
{
    template <typename>
    struct Ctxt;

    template <template <typename...> typename List, typename... Params>
    struct Ctxt<List<Params...>>
    {
        using type = ContextImpl<Opts, Params...>;
    };
};

/**
 * @brief Compile-time struct that receive wrapped parameters of the entry and
 * hold them. Must be used together with the macros provided below. Not intended
 * for use in user code.
 *
 * @tparam Otions
 *           Opts
 *
 * @details
 * Сontains the following public static methods:
 *     - @b GetField
 *     - @b GetOptions
 *
 * Сontains the following public static fields:
 *     - @b some_field                - pointer holds original function.
 *     - @b some_field_2              - user-defined pointer holds target.
 *
 * Each public static method of the structure may be unavailable (will result in
 * a compilation error) in case when the corresponding parameter has not been
 * passed to the context.
 *
 */
template <Options Opts, typename... Parameters>
using Context = detail::ContextImpl<Opts, Parameters...>;

template <Options Opts>
struct ToContext : detail::ToContextImpl<Opts>
{
    template <typename ParamsList>
    using fn = typename detail::ToContextImpl<Opts>::template Ctxt<ParamsList>::type;
};

}  // namespace detail
}  // namespace tmpl

#define TMPL_STR(name) ::tmpl::detail::ParameterHolder<TMPL_MP_STRING(#name), ::tmpl::detail::ParameterTag::String>

#define TMPL_TYPE(type) ::tmpl::detail::ParameterHolder<type, ::tmpl::detail::ParameterTag::Type>

#define TMPL_FIELD(type) ::tmpl::detail::ParameterHolder<type, ::tmpl::detail::ParameterTag::Type, true>

#define TMPL_FOO(foo) ::tmpl::detail::ParameterHolder<std::integral_constant<decltype(&foo), foo>, ::tmpl::detail::ParameterTag::Foo>

#define TMPL_FPTR(name)                                                                                                                    \
    ::tmpl::detail::ParameterHolder<std::integral_constant<decltype(&::##name), ::##name>, ::tmpl::detail::ParameterTag::FooPtr>

#define TMPL_ROUTINE(id) ::tmpl::detail::ParameterHolder<std::integral_constant<uint8_t, id>, ::tmpl::detail::ParameterTag::Id>

#define TMPL_CB(cb) ::tmpl::detail::ParameterHolder<std::integral_constant<decltype(&cb), cb>, ::tmpl::detail::ParameterTag::Callback>

#define TMPL_COMMA_SEP(r, token, i, e) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(token, e)
#define TMPL_CAT(token, ...) BOOST_PP_SEQ_FOR_EACH_I(TMPL_COMMA_SEP, token, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define TMPL_ENTRY(...) ::boost::mp11::mp_list<TMPL_CAT(TMPL_, __VA_ARGS__)>,

#define TMPL_LIST_BEGIN(Name, Type)                                                                                                        \
    using Name = ::boost::mp11::mp_transform_q < ::tmpl::detail::ToContext<Type>, ::boost::mp11::mp_pop_back < ::boost::mp11::mp_list <

#define TMPL_LIST_END() ::boost::mp11::mp_list<> >>> ;
