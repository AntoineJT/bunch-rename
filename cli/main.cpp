#include <iostream>
#include <filesystem>
#include <functional>
#include <optional>
#include <tclap/CmdLine.h>

#include "parser.hpp"
#include "strutil.hpp"

// can't find a way to do it with templates
// as TCLAP::Arg type does not have getValue
// #define OPT_ARG(x) x.isSet() ? std::optional(x.getValue()) : std::nullopt

int main(const int argc, const char* argv[])
{
    TCLAP::CmdLine cmd("Bunch Rename Tool");
    TCLAP::UnlabeledValueArg<std::string> srcdirArg("srcdir", "Source Directory containing files", true, ".", "string");
    TCLAP::UnlabeledValueArg<std::string> curfmtArg("curfmt", R"(Current file name format
        Example: curfmt = "{num} - {name}.mp3"
                 newfmt = "Song Artist - Album - {num}. {name}.mp3"
            for
                "11 - In Pieces.mp3"
            will result in
                "Song Artist - Album - 11. In Pieces.mp3"
)", true, "", "string");
    TCLAP::UnlabeledValueArg<std::string> newfmtArg("newfmt", "New file name format\nFor an example, see curfmt help.", true, "", "string");
    TCLAP::SwitchArg confirmArg("", "y", "Don't prompt for confirmation");
    cmd.add(srcdirArg).add(curfmtArg).add(newfmtArg).add(confirmArg);
    cmd.parse(argc, argv);

    auto oldFormatParser = Parser::ModelParser(curfmtArg.getValue());
    auto newFormatParser = Parser::ModelParser(newfmtArg.getValue());
    try {
        oldFormatParser.ParseModel();
        newFormatParser.ParseModel();
    }
    catch (const Parser::ParsingException& ex) {
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    auto srcdir = std::filesystem::absolute(srcdirArg.getValue());
    if (!std::filesystem::exists(srcdir)) {
        std::cerr << "error: this folder does not exists!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!std::filesystem::is_directory(srcdir)) {
        std::cerr << "error: provided path is not a directory!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::unordered_map<std::string, std::string> new_filenames;
    // for some reason decltype(files) does not work as I want
    std::vector<std::filesystem::directory_entry> files;

    // filters files
    for (const auto& file : std::filesystem::directory_iterator(srcdir)) {
        if (!file.is_regular_file()) {
            continue;
        }

        std::string filename = file.path().filename().string();
        std::string newname;
        try {
            newname = oldFormatParser.ConvertTo(newFormatParser, filename);
        }
        catch (const Parser::ParsingException&) {
            continue;
        }
        files.push_back(file);

        std::cout << filename << "\n> " << newname << '\n';
        new_filenames.try_emplace(std::move(filename), std::move(newname));
    }
    std::cout << std::flush;

    if (files.empty()) {
        std::cerr << "No file matched your pattern! (curfmt)" << std::endl;
        return EXIT_FAILURE;
    }

    if (!confirmArg.getValue()) {
        std::cout << "Are you sure to rename all those files? [Y/N] ";
        char c;
        std::cin >> c;
        if (c != 'y' && c != 'Y') {
            return EXIT_SUCCESS;
        }
    }

    for (const auto& file : files) {
        const auto& path = file.path();
        const auto& newname = new_filenames[path.filename().string()];
        try {
            std::filesystem::rename(path, path.parent_path() / newname);
        }
        catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
}
