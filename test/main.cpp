#include <tmpl/string.h>
#include <tmpl/utils.h>

#include <iostream>

using namespace boost::mp11;
using namespace tmpl;

void Foo() { std::cout << "Hello, World! from Foo" << std::endl; }

TMPL_LIST_BEGIN(TestList1, Options::Opt1)
TMPL_ENTRY(FOO(Foo))
TMPL_LIST_END()

int main()
{
    using str = tmpl::mp_string<'H', 'e', 'l', 'l', 'o', '!'>;

    Worker<TestList1>::MakeWork();

    std::cout << tmpl::mp_c_str_v<str> << std::endl;

    return 0;
}
