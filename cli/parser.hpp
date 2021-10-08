#pragma once
#include <string>
#include <unordered_map>
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

        std::string m_fmt;
        std::vector<out_elem> m_tokens;
        std::vector<int> m_var_indexes;
        std::vector<int> m_text_indexes;
        bool m_parsed = false; // will be used to assert on ConvertTo

    public:
        explicit ModelParser(std::string fmt);
        void ParseModel();
        std::unordered_map<std::string, std::string> ExtractData(const std::string& str);
        std::string ConvertTo(const ModelParser& newfmt, const std::string& data);
    };
}
