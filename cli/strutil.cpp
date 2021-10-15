#include "strutil.hpp"

bool StrUtil::StartsWith(const std::string_view str, const std::string_view begin)
{
    const size_t count = begin.length();

    for (size_t i = 0; i < count; ++i) {
        if (str[i] != begin[i]) {
            return false;
        }
    }
    return true;
}

#if 0
bool StrUtil::EndsWith(const std::string_view str, const std::string_view endstr)
{
    const size_t count = endstr.length();
    const size_t start = str.length() - count;

    // TODO Test it
    if (start <= 0) {
        return false;
    }

    for (size_t i = count - 1; i > 0; --i) {
        if (str[start + i] != endstr[i]) {
            return false;
        }
    }
    return true;
}
#endif
