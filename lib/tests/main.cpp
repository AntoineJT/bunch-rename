#include <iostream>
#include "lib/parser.hpp"
#include "lib/strutil.hpp"

#define T_ASSERT(cond) t_assert((cond), #cond, __LINE__)

void t_assert(const bool b_cond, const char* cond, const int line)
{
    if (!b_cond) {
        std::cerr << "assertion failed at line " << line << ": "
            << "\n\t" << cond << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string ConvertTo(const std::string& fmt, const std::string& newfmt, std::string_view data)
{
    auto parser = Parser::ModelParser(fmt);
    parser.ParseModel();
    auto parser2 = Parser::ModelParser(newfmt);
    parser2.ParseModel();
    return parser.ConvertTo(parser2, data);
}

bool ConvertToFails(const std::string& fmt, const std::string& newfmt, std::string_view data)
{
    try {
        ConvertTo(fmt, newfmt, data);
        return false;
    }
    catch (const Parser::ParsingException&) {
        return true;
    }
}

inline void TestParser()
{
    // VAR - VAR
    T_ASSERT(ConvertTo("{num} - {name}", "{num}. {name}",
        "11 - In Pieces.mp3") == "11. In Pieces.mp3");
    T_ASSERT(ConvertToFails("{num} - {name}", "{num}. {name}",
        "11. In Pieces.mp3"));

    // VAR - TEXT
    T_ASSERT(ConvertTo("{year} - {name}.txt", "{year}. {name}.log",
        "2021 - test.txt") == "2021. test.log");
    T_ASSERT(ConvertToFails("{year} - {name}.txt", "{year}. {name}.log",
        "2021. test.txt"));

    // TEXT - VAR
    T_ASSERT(ConvertTo("Linkin Park - Minutes to Midnight - {num}. {name}", "{num} - {name}",
        "Linkin Park - Minutes to Midnight - 11. In Pieces.mp3") == "11 - In Pieces.mp3");
    T_ASSERT(ConvertToFails("Linkin Park - Minutes to Midnight - {num}. {name}", "{num} - {name}",
        "Linkin Park - Minutes to Midnight - 11 - In Pieces.mp3"));

    // TEXT - TEXT
    T_ASSERT(ConvertTo("Linkin Park - Minutes to Midnight - {num}. {name}.mp3", "{num} - {name}.mp3",
        "Linkin Park - Minutes to Midnight - 11. In Pieces.mp3") == "11 - In Pieces.mp3");
    T_ASSERT(ConvertToFails("Linkin Park - Minutes to Midnight - {num}. {name}.mp3", "{num} - {name}.mp3",
        "Linkin Park - Minutes to Midnight - 11 - In Pieces.mp3"));
}

inline void TestStrUtil()
{
    // StartsWith
    T_ASSERT(StrUtil::StartsWith("This is a unit test!", "This"));
    T_ASSERT(!StrUtil::StartsWith("This is a unit test!", "this"));
    T_ASSERT(StrUtil::StartsWith("", ""));
    T_ASSERT(!StrUtil::StartsWith("", "test"));
    T_ASSERT(StrUtil::StartsWith("test", ""));
}

int main()
{
    TestParser();
    TestStrUtil();
    return EXIT_SUCCESS;
}
