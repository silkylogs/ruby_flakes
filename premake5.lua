workspace "ruby_flakes"
	  configurations { "Debug" }
	  architecture("x86_64")

project "ruby_flakes"
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"
	warnings "Everything"

	files { "src/ruby_flakes.c" }
	-- Note: Check ansi c compliance locally through the provided batch file

	filter "configurations:Debug"
	       defines { "DEBUG" }
	       symbols "On"
