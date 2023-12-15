set warning_flags=	^
-Wall			^
-Wextra			^
-Wwrite-strings		^
-Wstrict-prototypes	^
-Wmissing-prototypes	^
-Wmissing-declarations	^
-Wsign-compare		^
-Wlogical-op		^
-Wtype-limits		^
-Wsuggest-attribute=pure	^
-Wsuggest-attribute=const	^
-Wsuggest-attribute=noreturn	^
-Wsuggest-attribute=format	^
-Wformat-nonliteral

set flags=-ansi -pedantic -O0 -g -Werror 

gcc.exe %warning_flags% %flags% src\ruby_flakes.c 
