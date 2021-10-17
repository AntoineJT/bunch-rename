#include <iostream>
#include "lib/parser.hpp"

void t_assert(const bool cond)
{
    if (!cond) {
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

int main()
{
    // VAR - VAR
    t_assert(ConvertTo("{num} - {name}", "{num}. {name}", 
        "11 - In Pieces.mp3") == "11. In Pieces.mp3");
    t_assert(ConvertToFails("{num} - {name}", "{num}. {name}", 
        "11. In Pieces.mp3"));

    // VAR - TEXT
    t_assert(ConvertTo("{year} - {name}.txt", "{year}. {name}.log", 
        "2021 - test.txt") == "2021. test.log");
    t_assert(ConvertToFails("{year} - {name}.txt", "{year}. {name}.log", 
        "2021. test.txt"));

    // TEXT - TEXT
    t_assert(ConvertTo("Linkin Park - Minutes to Midnight - {num}. {name}.mp3", "{num} - {name}.mp3",
        "Linkin Park - Minutes to Midnight - 11. In Pieces.mp3") == "11 - In Pieces.mp3");
    t_assert(ConvertToFails("Linkin Park - Minutes to Midnight - {num}. {name}.mp3", "{num} - {name}.mp3",
        "Linkin Park - Minutes to Midnight - 11 - In Pieces.mp3"));
    return EXIT_SUCCESS;
}
