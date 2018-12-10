all: main.cpp codegen.cpp statsem.cpp parser.cpp scanner.cpp codegen.h token.h statsem.h parser.h scanner.h
	g++ -o comp main.cpp statsem.cpp parser.cpp scanner.cpp codegen.cpp
