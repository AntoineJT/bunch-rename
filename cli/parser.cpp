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
                if (buffer != "") {
                    m_tokens.push_back(out_elem{ out_type::TEXT, std::move(buffer) });
                    buffer = "";
                }
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
                m_tokens.push_back(out_elem{out_type::VAR, std::move(buffer)});
                buffer = "";
                state = PARSE_TEXT;
            }
            else {
                throw "error: unknown parsing state"s;
            }
        }
        else {
            buffer += c;
        }
    }
    m_parsed = true;
}
