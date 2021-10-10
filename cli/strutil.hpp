#pragma once
#include <string>
#include <string_view>

namespace StrUtil {
    bool StartsWith(const std::string_view str, const std::string_view begin);
    bool EndsWith(const std::string_view str, const std::string_view endstr);
}
