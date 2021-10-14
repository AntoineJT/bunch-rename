#pragma once
#include <exception>
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
        std::unordered_map<std::string, std::string> ExtractData(std::string_view str);
        std::string ConvertTo(const ModelParser& newfmt, std::string_view data);
    };

    struct ParsingException : private std::exception {
        explicit ParsingException(std::string reason = "error: error while parsing") : m_reason(std::move(reason)) {}
        ~ParsingException() noexcept override = default;
        inline const char* what() const noexcept override { return m_reason.c_str(); }
    private:
        std::string m_reason;
    };
}
