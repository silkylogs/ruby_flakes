workspace "ruby_flakes"
	  configurations { "Debug" }
	  architecture("x86_64")

project "ruby_flakes"
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"

	files { "src/ruby_flakes.c" }
	-- Note: Check ansi c compliance locally
	--prebuildcommands { 'check_ansi_c_compliance.bat' }

	filter "configurations:Debug"
	       defines { "DEBUG" }
	       	symbols "On"
