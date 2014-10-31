#!/bin/bash
#bison -d -o parser.cpp parser.y
#lex -o tokens.cpp tokens.l

	#-lstdc++ -lm -ltinfo -ldl -lpthread \
clang *.cpp \
	-g -std=c++11 -Wno-deprecated-register \
	-lstdc++ -lm -ldl -lpthread \
	`llvm-config-3.5 --cppflags` \
	-I/usr/include/llvm-3.5 -I/usr/include/llvm-c-3.5 \
	`llvm-config-3.5 --libs core jit native --ldflags`
