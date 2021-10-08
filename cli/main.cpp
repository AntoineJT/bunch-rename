#include <iostream>
#include <filesystem>
#include <functional>
#include <optional>
#include <tclap/CmdLine.h>

auto GetEntries(const std::string_view path,
    const std::function<bool(std::filesystem::directory_entry)>& filter)
-> std::vector<std::filesystem::directory_entry>
{
    namespace fs = std::filesystem;

    std::vector<fs::directory_entry> entries;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (filter(entry)) {
            entries.push_back(entry);
        }
    }
    return entries;
}

bool EndsWith(const std::string& str, const std::string& endstr)
{
    const size_t count = endstr.length();
    const size_t start = str.length() - count;

    for (size_t i = count - 1; i > 0; --i) {
        if (str[start + i] != endstr[i]) {
            return false;
        }
    }
    return true;
}

// can't find a way to do it with templates
// as TCLAP::Arg type does not have getValue
#define OPT_ARG(x) x.isSet() ? std::optional(x.getValue()) : std::nullopt

int main(const int argc, const char* argv[])
{
    TCLAP::CmdLine cmd("Bunch Rename Tool");
    TCLAP::UnlabeledValueArg<std::string> srcdirArg("srcdir", "Source Directory containing files", true, ".", "string");
    TCLAP::ValueArg<std::string> curfmtArg("", "curfmt", R"(Current file name format
        Example: curfmt = "{num} - {name}"
                 newfmt = "Song Artist - Album - {num}. {name}"
            for
                "11 - In Pieces"
            will result in
                "Song Artist - Album - 11. In Pieces"
)", true, "", "string");
    TCLAP::ValueArg<std::string> newfmtArg("", "newfmt", "New file name format\nFor an example, see curfmt help.", true, "", "string");
    TCLAP::ValueArg<std::string> extArg("", "ext", "Keep only files with the specified extension", false, "", "string");
    TCLAP::SwitchArg dryrunArg("", "dry-run", "List selected files only");
    cmd.add(srcdirArg).add(curfmtArg).add(newfmtArg)
        .add(extArg).add(dryrunArg);
    cmd.parse(argc, argv);

    const std::string& srcdir = srcdirArg.getValue();
    const std::string& curfmt = curfmtArg.getValue();
    const std::string& newfmt = newfmtArg.getValue();
    const auto ext = OPT_ARG(extArg);
    const bool dryrun = dryrunArg.getValue();

    const auto filter = [ext](const std::filesystem::directory_entry& entry) {
        if (ext && !EndsWith(entry.path().filename().string(), ext.value()))
            return false;
        return entry.is_regular_file();
    };
    auto files = GetEntries(srcdir, filter);

    if (dryrun) {
        for (const auto& file : files) {
            std::cout << file.path().filename() << '\n';
        }
    }
}
