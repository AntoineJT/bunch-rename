set_project("bunch_rename")
set_version("0.0.0")

set_defaultmode("debug")

add_rules("mode.debug", "mode.release")
set_languages("cxx17")
set_symbols("debug", "edit")

add_requires("tclap 1.4.0-rc1") -- latest version at the time

if is_mode("release") then
    set_optimize("fastest")
end

target("cli")
    set_kind("binary")
    set_basename("brn")

    add_files("cli/**.cpp")
    add_headerfiles("cli/**.hpp")

    add_packages("tclap")
