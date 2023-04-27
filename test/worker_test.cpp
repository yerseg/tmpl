#include <tmpl/worker.h>

#include <gtest/gtest.h>

#define __STDC_LIB_EXT1__
#include <string.h>

using namespace boost::mp11;
using namespace tmpl;

static std::string g_test_str = {};
static std::string g_test_str_2 = {};

void Foo(char const*& ptr)
{
    g_test_str = "Hello, World! from Foo";
    ptr = new char[10];
    std::memcpy((void*)ptr, (void*)g_test_str.data(), 10);
}

void Bar(char const*& ptr)
{
    g_test_str_2 = "Hello, World! from Bar";
    ptr = new char[20];
    std::memcpy((void*)ptr, (void*)g_test_str_2.data(), 20);
}

TEST(WorkerTest, Basic)
{
    TMPL_LIST_BEGIN(TestList, Options::Opt1)
    TMPL_ENTRY(FOO(Foo), FIELD(char const*))
    TMPL_ENTRY(FOO(Bar), FIELD(char const*))
    TMPL_LIST_END()

    Worker<TestList>::MakeWork();

    EXPECT_EQ("Hello, World! from Foo", g_test_str);
}
