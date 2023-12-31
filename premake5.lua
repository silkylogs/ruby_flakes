workspace "ruby_flakes"
	  configurations { "Debug" }
	  architecture("x86_64")

project "ruby_flakes"
	kind "WindowedApp"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"
	warnings "Everything"
	includedirs { "external_includes" }
	libdirs { "external_libs" }
	links { "SDL2" }
	linkoptions { "-lsdl2" }

	files { "src/ruby_flakes.c" }
	-- Note: Check ansi c compliance locally through the provided batch file

	filter "configurations:Debug"
	       defines { "DEBUG" }
	       symbols "On"
