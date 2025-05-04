-- Copyright (C) 2025  ilobilo

set_project("img2ascii")
set_version("v0.1")

set_license("EUPL-1.2")

add_rules("mode.debug", "mode.release")
set_policy("run.autobuild", true)
add_rules("plugin.compile_commands.autoupdate", { outputdir = "build" })

target("img2ascii")
    set_kind("binary")

    add_files("source/**.cpp")

    set_languages("c++23")

    set_warnings("all", "error")
    set_optimize("fastest")

    set_rundir("$(projectdir)")
