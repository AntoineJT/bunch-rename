#include <iostream>
#include <filesystem>
#include <tclap/CmdLine.h>

template<bool filesOnly>
auto GetEntries(const std::string_view path)
    -> std::vector<std::filesystem::directory_entry>
{
    namespace fs = std::filesystem;

    std::vector<fs::directory_entry> entries;
    if constexpr (filesOnly) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                entries.push_back(entry);
            }
        }
    }
    else {
        for (const auto& entry : fs::directory_iterator(path)) {
            entries.push_back(entry);
        }
    }
    return entries;
}

void PrintFiles(const std::string_view path)
{
    for (const auto& entry : GetEntries<true>(path)) {
        std::cout << entry.path().filename() << '\n';
    }
}

int main(const int argc, const char* argv[])
{
    TCLAP::CmdLine cmd("Bunch Rename Tool");
    TCLAP::UnlabeledValueArg<std::string> srcdirArg("srcdir", "Source Directory containing files", true, ".", "string");
    TCLAP::ValueArg<std::string> extArg("", "ext", "Keep only files with the specified extension", false, "", "string");
    cmd.add(srcdirArg);
    cmd.add(extArg);
    cmd.parse(argc, argv);

    const std::string srcdir = srcdirArg.getValue();
    PrintFiles(srcdir);
}
