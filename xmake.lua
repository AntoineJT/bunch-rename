set_xmakever("2.5.6")

set_project("bunch_rename")
set_version("0.0.0")

set_defaultmode("debug")

add_rules("mode.debug", "mode.release")
set_languages("cxx17")
set_symbols("debug", "edit")

add_requires("tclap 1.4.0-rc1") -- latest version at the time

if is_plat("linux") or is_plat("mingw") then
    add_links("stdc++fs")
end

if is_mode("release") then
    set_optimize("fastest")
end

target("cli")
    set_kind("binary")
    set_basename("brn")

    add_files("cli/**.cpp")
    -- add_headerfiles("cli/**.hpp")

    add_packages("tclap")
    add_deps("lib")

target("lib")
    set_kind("static")
    
    add_files("lib/**.cpp")
    add_headerfiles("lib/**.hpp")
    add_includedirs("lib/include/", {public = true})

--[[
target("cli_tests")
    set_kind("binary")
]]
