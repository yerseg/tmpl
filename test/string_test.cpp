#include <tmpl/string.h>

#include <gtest/gtest.h>

using namespace tmpl;

TEST(MpStringTest, Basic)
{
    EXPECT_EQ("Hello!", std::string_view(mp_c_str_v<TMPL_MP_STRING("Hello!")>));
    EXPECT_EQ(L"Hello!", std::wstring_view(mp_c_str_v<TMPL_MP_STRING(L"Hello!")>));
    EXPECT_EQ(u8"Hello!", std::u8string_view(mp_c_str_v<TMPL_MP_STRING(u8"Hello!")>));
    EXPECT_EQ(u"Hello!", std::u16string_view(mp_c_str_v<TMPL_MP_STRING(u"Hello!")>));
    EXPECT_EQ(U"Hello!", std::u32string_view(mp_c_str_v<TMPL_MP_STRING(U"Hello!")>));
}
