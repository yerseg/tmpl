#include <tmpl/string.h>

#include <gtest/gtest.h>

using namespace tmpl;

TEST(MpStringTest, Basic) { EXPECT_EQ("Hello!", std::string_view(mp_c_str_v<TMPL_MP_STRING("Hello!")>)); }