#include <tmpl/worker.h>

#include <gtest/gtest.h>

#include <cstring>

namespace {

using namespace boost::mp11;
using namespace tmpl;

std::string test_str = {};
std::string test_str_2 = {};

int* int_ptr = nullptr;
int* int_ptr_2 = nullptr;

void Foo(int& i)
{
    test_str = "Hello, World! from Foo";
    int_ptr = &i;
}

void Bar(int& i)
{
    test_str_2 = "Hello, World! from Bar";
    int_ptr_2 = &i;
}

TEST(WorkerTest, Basic)
{
    TMPL_LIST_BEGIN(TestList, Options::Opt1)
    TMPL_ENTRY(FOO(Foo), FIELD(int))
    TMPL_ENTRY(FOO(Bar), FIELD(int))
    TMPL_LIST_END()

    Worker<TestList>::MakeWork();

    EXPECT_EQ("Hello, World! from Foo", test_str);
    EXPECT_EQ("Hello, World! from Bar", test_str_2);

    EXPECT_NE(int_ptr, int_ptr_2);
}

}  // namespace
