#include "parser.hpp"
#include <iostream>
#include "strutil.hpp"

using namespace std::string_literals;

Parser::ModelParser::ModelParser(std::string fmt)
    : m_fmt(std::move(fmt)) {}

void Parser::ModelParser::ParseModel()
{
    enum {
        PARSE_ID, // text under brackets
        PARSE_TEXT
    };

    auto state = PARSE_TEXT;
    std::string buffer;
    int index = 0;
    for (char c : m_fmt) {
        if (c == '{') {
            if (state == PARSE_ID) {
                throw "error: invalid format"s;
            }
            else if (state == PARSE_TEXT) {
                if (buffer != "") {
                    m_tokens.push_back(out_elem{ out_type::TEXT, std::move(buffer) });
                    m_text_indexes.push_back(index);
                    ++index;
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
                m_tokens.push_back(out_elem{ out_type::VAR, std::move(buffer) });
                m_var_indexes.push_back(index);
                ++index;
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

std::unordered_map<std::string, std::string> Parser::ModelParser::ExtractData(const std::string& str)
{
    // m_tokens
    size_t tok_index = 0;
    out_type type = m_tokens[tok_index].type;
    std::unordered_map<std::string, std::string> map;
    std::string_view strv = str;
    std::string_view prev_strv = strv;
    std::string buffer;

    while (!strv.empty()) {
        if (type == out_type::VAR) {
            if (!StrUtil::StartsWith(strv, m_tokens[tok_index + 1].str)) {
                buffer += strv.front();
                strv.remove_prefix(1);
            }
            else {
                map.try_emplace(m_tokens[tok_index].str, std::move(buffer));
                ++tok_index;
                type = m_tokens[tok_index].type;
                buffer = "";
            }
        }
        else if (type == out_type::TEXT) {
            const auto& curstr = m_tokens[tok_index].str;
            if (!StrUtil::StartsWith(strv, curstr))
                throw "error: provided file name do not match the format"s;

            strv.remove_prefix(curstr.length());
            ++tok_index;
            type = m_tokens[tok_index].type;
            prev_strv = strv;
        }
    }
    if (type == out_type::VAR) {
        map.try_emplace(m_tokens[tok_index].str, prev_strv);
    }
    return map;
}

std::string Parser::ModelParser::ConvertTo(const ModelParser& newfmt, const std::string& data)
{
    auto map = ExtractData(data);
    std::string str;
    for (const auto& token : newfmt.m_tokens) {
        if (token.type == out_type::TEXT) {
            str += token.str;
        }
        else if (token.type == out_type::VAR) {
            str += map[token.str];
        }
        else {
            throw "error: unknown parsing state"s;
        }
    }
    return str;
}
