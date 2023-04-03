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
    Worker<TestList1>::MakeWork();
    return 0;
}
