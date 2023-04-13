#include <tmpl/worker.h>

#include <gtest/gtest.h>

using namespace boost::mp11;
using namespace tmpl;

static std::string g_test_str = {};
void Foo() { g_test_str = "Hello, World! from Foo"; }

TEST(WorkerTest, Basic)
{
    TMPL_LIST_BEGIN(TestList, Options::Opt1)
    TMPL_ENTRY(FOO(Foo))
    TMPL_LIST_END()

    Worker<TestList>::MakeWork();

    EXPECT_EQ("Hello, World! from Foo", g_test_str);
}
