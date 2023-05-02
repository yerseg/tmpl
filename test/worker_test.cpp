#include <tmpl/worker.h>

#include <gtest/gtest.h>

#include <cstring>

using namespace boost::mp11;
using namespace tmpl;

static std::string g_test_str = {};
static std::string g_test_str_2 = {};

static int* char_ptr = nullptr;
static int* char_ptr_2 = nullptr;

void Foo(int& i)
{
    g_test_str = "Hello, World! from Foo";
    char_ptr = &i;
}

void Bar(int& i)
{
    g_test_str_2 = "Hello, World! from Bar";
    char_ptr_2 = &i;
}

TEST(WorkerTest, Basic)
{
    TMPL_LIST_BEGIN(TestList, Options::Opt1)
    TMPL_ENTRY(FOO(Foo), FIELD(int))
    TMPL_ENTRY(FOO(Bar), FIELD(int))
    TMPL_LIST_END()

    Worker<TestList>::MakeWork();

    EXPECT_EQ("Hello, World! from Foo", g_test_str);
    EXPECT_EQ("Hello, World! from Bar", g_test_str_2);

    EXPECT_NE(char_ptr, char_ptr_2);
    // EXPECT_EQ("Hello, World! from Foo", char_ptr);
    // EXPECT_EQ("Hello, World! from Bar", char_ptr_2);

    // delete char_ptr;
    // delete char_ptr_2;
}
