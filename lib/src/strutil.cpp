#include "lib/strutil.hpp"

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
