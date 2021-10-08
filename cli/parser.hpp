#pragma once
#include <string>
#include <vector>

namespace Parser {
    class ModelParser {
        enum class out_type {
            VAR, // text parsed under brackets
            TEXT
        };
        
        struct out_elem {
            out_type type;
            std::string str; // id name or content
        };

        std::vector<out_elem> m_tokens;
        std::string m_fmt;
        bool m_parsed = false; // will be used to assert on ConvertTo

    public:
        explicit ModelParser(std::string fmt);
        void ParseModel();
        // TODO
        // std::string ConvertTo(const ModelParser& newfmt, const std::string& data);
    };
}
