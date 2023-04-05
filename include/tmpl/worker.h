#pragma once

#include "context.h"
#include "options.h"

#include <iostream>

namespace tmpl {
namespace detail {
namespace {

template <typename F>
inline void TheSomeWork(F&& f)
{
    std::cout << "Very important work: " << std::endl;
    std::forward<F>(f)();
    std::cout << "Complete!" << std::endl;
}

template <typename T>
void SomeWork()
{
    constexpr auto opts = T::GetOptions();
    if constexpr (opts == Options::Opt1)
    {
        TheSomeWork(T::template GetField<ParameterTag::Foo>());
    }
}

}  // anonymous namespace

template <typename List>
struct Worker;

template <template <typename...> typename List, typename T, typename... U>
struct Worker<List<T, U...>>
{
    static void MakeWork() { (SomeWork<T>(), ..., SomeWork<U>()); }
};

}  // namespace detail

/**
 * @brief Compile-time struct that receive list of Contexts-s and provides ability to make work on them.
 *
 * @tparam List
 *           List of Contexts
 *
 * @details
 * Сontains the following public members:
 *     - @b MakeWork   - static method for making work @b
 *
 *
 * Usage:
 *     @code
 *     int Foo(int) { std::cout << "example" << std::endl; }
 *
 *     TMPL_LIST_BEGIN(List, Options::Opt1)
 *	        TMPL_ENTRY(FOO(Foo))
 *     TMPL_LIST_END()
 *
 *
 *     List::MakeWork();
 *     @endcode
 */
template <typename List>
struct Worker : detail::Worker<List>
{};

}  // namespace tmpl
