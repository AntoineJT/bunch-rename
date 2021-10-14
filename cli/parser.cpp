#include "parser.hpp"
#include <cassert>
#include <iostream>
#include "strutil.hpp"

Parser::ModelParser::ModelParser(std::string fmt)
    : m_fmt(std::move(fmt)) {}

// if it throws m_parsed is still false
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
                throw ParsingException("error: invalid format");
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
                throw ParsingException("error: unknown parsing state");
            }
        }
        else if (c == '}') {
            if (state == PARSE_TEXT) {
                throw ParsingException("error: invalid format");
            }
            else if (state == PARSE_ID) {
                m_tokens.push_back(out_elem{ out_type::VAR, std::move(buffer) });
                m_var_indexes.push_back(index);
                ++index;
                buffer = "";
                state = PARSE_TEXT;
            }
            else {
                throw ParsingException("error: unknown parsing state");
            }
        }
        else {
            buffer += c;
        }
    }
    m_parsed = true;
}

std::unordered_map<std::string, std::string> Parser::ModelParser::ExtractData(std::string_view strv)
{
    size_t tok_index = 0;
    out_type type = m_tokens[tok_index].type;
    std::unordered_map<std::string, std::string> map;
    std::string buffer;

    while (!strv.empty()) {
        if (type == out_type::VAR) {
            if (tok_index + 1 == m_tokens.size()) {
                map.try_emplace(m_tokens[tok_index].str, strv);
                break;
            }
            // relies on type alternance (var to
            // text and the opposite) to work
            if (!StrUtil::StartsWith(strv, m_tokens[tok_index + 1].str)) {
                buffer += strv.front();
                strv.remove_prefix(1);
            }
            else {
                map.try_emplace(m_tokens[tok_index].str, std::move(buffer));
                ++tok_index;
                type = m_tokens[tok_index].type; // always TEXT for now
                buffer = "";
            }
        }
        else if (type == out_type::TEXT) {
            const auto& curstr = m_tokens[tok_index].str;
            if (!StrUtil::StartsWith(strv, curstr))
                throw ParsingException("error: provided file name do not match the format");

            strv.remove_prefix(curstr.length());
            ++tok_index;
            type = m_tokens[tok_index].type;
        }
    }
    return map;
}

bool Parser::ModelParser::Matches(std::string_view strv)
{
    // less effective than duplicating the
    // ExtractData code here while removing
    // unneeded operations as using a map
    // but much more maintainable
    try {
        ExtractData(strv);
    }
    catch (const ParsingException&) {
        return false;
    }
    return true;
}

std::string Parser::ModelParser::ConvertTo(const ModelParser& newfmt, std::string_view data)
{
    assert(m_parsed && newfmt.m_parsed);

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
            throw ParsingException("error: unknown parsing state");
        }
    }
    return str;
}
