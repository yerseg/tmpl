#include <tmpl/string.h>

#include <gtest/gtest.h>

using namespace tmpl;

TEST(MpStringTest, Basic)
{
#if defined(_MSC_VER)
    EXPECT_EQ("Hello!", std::string_view(mp_c_str_v<mp_string<'H', 'e', 'l', 'l', 'o', '!'>>));
#else
    EXPECT_EQ("Hello!", std::string_view(mp_c_str_v<TMPL_MP_STRING("Hello!")>));
#endif
}