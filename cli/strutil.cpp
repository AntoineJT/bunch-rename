#include "strutil.hpp"

bool StrUtil::StartsWith(const std::string_view str, const std::string& begin)
{
    const size_t count = begin.length();

    for (size_t i = 0; i < count; ++i) {
        if (str[i] != begin[i]) {
            return false;
        }
    }
    return true;
}

bool StrUtil::EndsWith(const std::string_view str, const std::string& endstr)
{
    const size_t count = endstr.length();
    const size_t start = str.length() - count;

    for (size_t i = count - 1; i > 0; --i) {
        if (str[start + i] != endstr[i]) {
            return false;
        }
    }
    return true;
}
