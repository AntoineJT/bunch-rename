#include <iostream>
#include <filesystem>
#include <functional>
#include <optional>
#include <tclap/CmdLine.h>

#include "parser.hpp"
#include "strutil.hpp"

auto GetEntries(const std::filesystem::path& path,
    const std::function<bool(const std::filesystem::directory_entry&)>& filter)
{
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (filter(entry)) {
            entries.push_back(entry);
        }
    }
    return entries;
}

// can't find a way to do it with templates
// as TCLAP::Arg type does not have getValue
#define OPT_ARG(x) x.isSet() ? std::optional(x.getValue()) : std::nullopt

int main(const int argc, const char* argv[])
{
    TCLAP::CmdLine cmd("Bunch Rename Tool");
    TCLAP::UnlabeledValueArg<std::string> srcdirArg("srcdir", "Source Directory containing files", true, ".", "string");
    TCLAP::UnlabeledValueArg<std::string> curfmtArg("curfmt", R"(Current file name format
        Example: curfmt = "{num} - {name}"
                 newfmt = "Song Artist - Album - {num}. {name}"
            for
                "11 - In Pieces"
            will result in
                "Song Artist - Album - 11. In Pieces"
)", true, "", "string");
    TCLAP::UnlabeledValueArg<std::string> newfmtArg("newfmt", "New file name format\nFor an example, see curfmt help.", true, "", "string");
    TCLAP::ValueArg<std::string> extArg("", "ext", "Keep only files with the specified extension", false, "", "string");
    TCLAP::SwitchArg confirmArg("", "y", "Don't prompt for confirmation");
    cmd.add(srcdirArg).add(curfmtArg).add(newfmtArg)
        .add(extArg).add(confirmArg);
    cmd.parse(argc, argv);

    const auto ext = OPT_ARG(extArg);
    const auto filter = [ext](const std::filesystem::directory_entry& entry) {
        if (ext && !StrUtil::EndsWith(entry.path().filename().string(), ext.value()))
            return false;
        return entry.is_regular_file();
    };

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

    const auto files = GetEntries(srcdir, filter);
    std::unordered_map<std::string, std::string> new_filenames;
    // for some reason decltype(files) does not work as I want
    std::vector<std::filesystem::directory_entry> filteredfiles;
    filteredfiles.reserve(files.size());

    for (const auto& file : files) {
        std::string filename = file.path().filename().string();
        // caution if no extension
        std::string fileext = filename.substr(filename.find_last_of('.'));
        std::string filenameNoExt = filename.substr(0, filename.length() - fileext.length());

        std::string newnameNoExt;
        try {
            newnameNoExt = oldFormatParser.ConvertTo(newFormatParser, filenameNoExt);
        }
        catch (const Parser::ParsingException&) {
            continue;
        }
        filteredfiles.push_back(file);

        std::string newname = newnameNoExt + fileext;
        std::cout << filename << "\n> " << newname << '\n';
        new_filenames.try_emplace(std::move(filename), std::move(newname));
    }
    std::cout << std::flush;

    if (filteredfiles.empty()) {
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

    for (const auto& file : filteredfiles) {
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
