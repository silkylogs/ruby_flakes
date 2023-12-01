# Ruby Flakes

This is a pure C89 project to create a logic gate simulator (alongside an environment) intended
to be used as an experimental scripting/live programming apparatus for video games.

## Building instructions
At this stage in development, only libc is a prerequisite to build this project.
Either run the built-in `build.bat` or consider modifying the contents of the build script:
```bat
gcc.exe -Wall -Werror -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wsign-compare -Wlogical-op -Wtype-limits -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=format -Wformat-nonliteral -ansi -pedantic -O0 -Wextra ruby_flakes.c -o ruby_flakes.exe
```