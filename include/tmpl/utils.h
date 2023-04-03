#pragma once

#include "context.h"
#include "options.h"
#include "worker.h"

#include <boost/mp11/mpl_list.hpp>

namespace tmpl {
namespace detail {

template <typename T>
struct EqualImpl;

template <template <Options, typename...> typename Context, Options Opts, typename... Parameters>
struct EqualImpl<Context<Opts, Parameters...>>
{
    template <typename Parameter>
    using type = std::is_same<typename ParametersGetter<Parameter::Tag::value, Parameters...>::type, typename Parameter::Type>;
};

template <typename... Parameters>
struct Equal
{
    template <typename U>
    using type = boost::mp11::mp_all_of<boost::mp11::mp_list<Parameters...>, EqualImpl<U>::template value>;
};

template <typename... Parameters>
struct NotEqual
{
    template <typename U>
    using type = boost::mp11::mp_bool<!boost::mp11::mp_all_of<boost::mp11::mp_list<Parameters...>, EqualImpl<U>::template type>::value>;
};

template <typename List, typename... Parameters>
struct Unique
{
    template <typename T>
    using type = std::is_same<boost::mp11::mp_count_if<List, Equal<Parameters...>::template type>, boost::mp11::mp_size_t<1>>;
};

template <typename List, typename... Parameters>
struct GetContextsThatMatchedWithParameters
{
    using type = boost::mp11::mp_remove_if<List, NotEqual<Parameters...>::template type>;
};

template <typename List, typename... Parameters>
struct GetContextFromList
{
    using MatchedContexts = typename GetContextsThatMatchedWithParameters<List, Parameters...>::type;

    static_assert(boost::mp11::mp_size<MatchedContexts>::value != 0, "Assertion error: type not found!");
    static_assert(
        boost::mp11::mp_size<MatchedContexts>::value == 1,
        "Assertion error: the passed list of parameters corresponds to "
        "several Contexts!");

    using type = boost::mp11::mp_front<MatchedContexts>;
};

template <typename FirstContext, typename... Context>
inline auto GetValidSomeFieldPtrImpl()
{
    static_assert(
        (std::is_same_v<decltype(FirstContext::some_field), decltype(Context::some_field)> && ...),
        "Assertion error: Call types of some_field must be same "
        "for all contexts in pack.");

    decltype(FirstContext::some_field) validPtr = nullptr;

    const bool found =
        ((FirstContext::some_field != nullptr ? validPtr = FirstContext::some_field, true : false) || ... ||
         (Context::some_field != nullptr ? validPtr = Context::some_field, true : false));

    assert(found && validPtr);

    return validPtr;
}

template <typename List>
struct ValidSomeFieldPtrGetter;

template <template <typename...> typename List, typename... Entries>
struct ValidSomeFieldPtrGetter<List<Entries...>>
{
    static auto Get()
    {
        static const auto validPtr = GetValidSomeFieldPtrImpl<Entries...>();
        return validPtr;
    }
};

}  // namespace detail
}  // namespace tmpl

#define GET_SOME_FIELD_PTR(ListName, ...) ::tmpl::detail::GetContextFromList<ListName, TMPL_CAT(TMPL_, __VA_ARGS__)>::type::some_field

#define GET_VALID_SOME_FIELD_PTR(ListName, ...)                                                                                            \
    ::tmpl::detail::ValidSomeFieldPtrGetter<                                                                                               \
        typename ::tmpl::detail::GetContextsThatMatchedWithParameters<ListName, TMPL_CAT(TMPL_, __VA_ARGS__)>::type>::Get()

#define GET_SOME_FIELD_2_PTR(ListName, ...) ::tmpl::detail::GetContextFromList<ListName, TMPL_CAT(TMPL_, __VA_ARGS__)>::type::some_field_2
