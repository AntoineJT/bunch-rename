#include "parser.hpp"

Parser::ModelParser::ModelParser(std::string fmt)
    : m_fmt(std::move(fmt)) {}

void Parser::ModelParser::ParseModel()
{
    using namespace std::string_literals;
    enum {
        PARSE_ID, // text under brackets
        PARSE_TEXT
    };

    auto state = PARSE_TEXT;
    std::string buffer;
    for (char c : m_fmt) {
        if (c == '{') {
            if (state == PARSE_ID) {
                throw "error: invalid format"s;
            } 
            else if (state == PARSE_TEXT) {
                m_tokens.emplace_back(out_type::TEXT, std::move(buffer));
                buffer = "";
                state = PARSE_ID;
            }
            else {
                throw "error: unknown parsing state"s;
            }
        }
        else if (c == '}') {
            if (state == PARSE_TEXT) {
                throw "error: invalid format"s;
            }
            else if (state == PARSE_ID) {
                m_tokens.emplace_back(out_type::VAR, std::move(buffer));
                buffer = "";
                state = PARSE_TEXT;
            }
        }
        else {
            buffer += c;
        }
    }
}
