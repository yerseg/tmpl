#pragma once

#include "context.h"
#include "options.h"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>

#include <type_traits>

namespace tmpl {

namespace detail {

enum class ParameterTag
{
    String,
    Type,
    Foo,
    FooPtr,
    Id,
    Callback,
    Ptr
};

template <ParameterTag ParamTag, typename Parameter, bool IsHold = false>
struct ParameterHolder
{
    using Tag = std::integral_constant<ParameterTag, ParamTag>;
    using Type = Parameter;
    using Hold = std::bool_constant<IsHold>;
};

template <ParameterTag Tag>
struct IsParameterTagMatch
{
    template <typename U>
    using fn = std::bool_constant<U::Tag::value == Tag>;
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
    : public ContextField<std::conditional_t<std::is_same_v<typename Param::Hold, std::true_type>, typename Param::Type, void>>
{};

template <Options Opts, typename _Tag, typename... Parameters>
struct ContextBase : public FieldsProvider<Parameters>...
{
    // static_assert(
    //     std::is_same_v<
    //         typename ParametersGetter<ParameterTag::Type, Parameters...>::type,
    //         typename ParametersGetter<ParameterTag::Cb, Parameters...>::type::value_type>,
    //     "Assertion error");

    // field can be string, functor, everything
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

    // static inline typename ParametersGetter<ParameterTag::Type, Parameters...>::type some_field = nullptr;
};

template <Options Opts, typename Tag, typename... Parameters>
struct ContextImpl : ContextBase<Opts, Tag, Parameters...>
{};

template <typename Tag, typename... Parameters>
struct ContextImpl<Options::Opt7, Tag, Parameters...> : ContextBase<Options::Opt7, Parameters...>
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
        using type = ContextImpl<Opts, List<Params...>, Params...>;
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

#define TMPL_STR(name) ::tmpl::detail::ParameterHolder<::tmpl::detail::ParameterTag::String, TMPL_MP_STRING(#name)>

#define TMPL_TYPE(type) ::tmpl::detail::ParameterHolder<::tmpl::detail::ParameterTag::Type, type>

#define TMPL_FIELD(type) ::tmpl::detail::ParameterHolder<::tmpl::detail::ParameterTag::Type, type, true>

#define TMPL_FOO(foo) ::tmpl::detail::ParameterHolder<::tmpl::detail::ParameterTag::Foo, std::integral_constant<decltype(&foo), foo>>

#define TMPL_FPTR(name)                                                                                                                    \
    ::tmpl::detail::ParameterHolder<::tmpl::detail::ParameterTag::FooPtr, std::integral_constant<decltype(&::##name), ::##name>>

#define TMPL_ROUTINE(id) ::tmpl::detail::ParameterHolder<::tmpl::detail::ParameterTag::Id, std::integral_constant<uint8_t, id>>

#define TMPL_CB(cb) ::tmpl::detail::ParameterHolder<::tmpl::detail::ParameterTag::Callback, std::integral_constant<decltype(&cb), cb>>

#define TMPL_COMMA_SEP(r, token, i, e) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(token, e)
#define TMPL_CAT(token, ...) BOOST_PP_SEQ_FOR_EACH_I(TMPL_COMMA_SEP, token, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define TMPL_ENTRY(...) ::boost::mp11::mp_list<TMPL_CAT(TMPL_, __VA_ARGS__)>,

#define TMPL_LIST_BEGIN(Name, Type)                                                                                                        \
    using Name = ::boost::mp11::mp_transform_q < ::tmpl::detail::ToContext<Type>, ::boost::mp11::mp_pop_back < ::boost::mp11::mp_list <

#define TMPL_LIST_END() ::boost::mp11::mp_list<> >>> ;
