solution "Bytecode"
    configurations {
        "Release",
        "Debug"
    }

    platforms {
        "x64",
        "x32",
        "Native" -- for targets where bitness is not specified
    }

    language "C++"

    location "../"
    objdir "../build/"

PROJECT_DIR = "../"
SOURCE_DIR = path.join(PROJECT_DIR, "source/")


project "Bytecode"
    kind "ConsoleApp"

    configuration "Debug"
        targetdir "../bin/debug/"
        objdir "obj/debug"
        defines { "_DEBUG" }
        flags { "Symbols" }
		debugargs { "cRun", "./Programs/Functions/Functions.bca" }
    configuration "Release"
        targetdir "../bin/release/"
        objdir "obj/debug"
	flags {"OptimizeSpeed", "No64BitChecks"}

    configuration "vs*"
        flags { "Unicode", "Winmain"}
        defines { "WIN32", "PLATFORM_Win" }
		links { }
    configuration { "linux", "gmake"}
--	flags {"-pedantic"}--
	defines { "PLATFORM_Linux", "__linux__" }
	includedirs { "/usr/include" }

	buildoptions_cpp
	{
		"-std=c++14",
	}

    configuration {}

    defines { "_CONSOLE" }

	flags {"ExtraWarnings", "FatalWarnings"}

    files {
        path.join(SOURCE_DIR, "*.cpp"),
        path.join(SOURCE_DIR, "*.h"),
    }

    excludes {
    }
    links{
    }

    -- optional. This is purely cosmetically.
    vpaths {
    }
