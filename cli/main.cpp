#include <iostream>
#include <filesystem>
#include <tclap/CmdLine.h>

int main(const int argc, const char* argv[])
{
    TCLAP::CmdLine cmd("Bunch Rename Tool");
    TCLAP::UnlabeledValueArg<std::string> srcdirArg("srcdir", "Source Directory containing files", true, ".", "string");
    cmd.add(srcdirArg);
    cmd.parse(argc, argv);

    const std::string srcdir = srcdirArg.getValue();
    for (const auto& entry : std::filesystem::directory_iterator(srcdir)) {
        std::cout << entry.path().filename() << '\n';
    }
}
