#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "token.h"
#include "node.h"
#include "parser.h"
#include "statsem.h"
#include "codegen.h"

FILE *fp = NULL;
FILE *fout = NULL;


int main(const int argc, char *argv[])
{
	node_t *root = NULL;

	// Choose file pointer based on number of arguments
	if (argc == 1)
	{
		fp = stdin;
		fout = fopen("out.asm", "w");
	}
	else if (argc == 2)
	{
		// Build filename from arguments
		int inNameLen = strlen(argv[1]) + 6;
		char inName[inNameLen];
		char outName[inNameLen];
		sprintf(inName, "%s.fs18", argv[1]);
		sprintf(outName, "%s.asm", argv[1]);
		//strcpy(inName, argv[1]);
		//strcat(inName, ".fs18");
		
		// Open file and call test scanner with file pointer
		fp = fopen(inName, "r");
		fout = fopen(outName, "w");
		if (fp == NULL)
		{
			// Error if file cannot be opened
			printf("Main error: Could not open file \"%s\"\n", inName);
			return 1;
		}
		fout = fopen(outName, "w");
		if (fout == NULL)
		{
			// Error if file cannot be opened
			printf("Main error: Could not open file \"%s\"\n", outName);
			return 1;
		}
	}
	else
	{
		// Error message for more than two arguments
		printf("Main error: invalid number of arguments\n");
		return 1;
	}

	// Create tree from input
	root = parser();

	// Close file
	if (argc == 2)
		fclose(fp);

	// Verify static semantics
	statSem(root);

	// Generate virtmach code
	codeGen(root);

	return 0;
}
