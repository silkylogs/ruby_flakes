-- premake5.lua
workspace "ruby_flakes"
   configurations { "Debug" }
   architecture("x86_64")

project "ruby_flakes"
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files { "src/ruby_flakes.c" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
