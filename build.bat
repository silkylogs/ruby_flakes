set flags=-Wall -Werror -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wsign-compare -Wlogical-op -Wtype-limits -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=format -Wformat-nonliteral -ansi -pedantic -O0 -Wextra -g

gcc.exe %flags% ruby_flakes.c -o ruby_flakes.exe
