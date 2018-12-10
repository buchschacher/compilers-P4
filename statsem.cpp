#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <deque>
#include "token.h"
#include "node.h"

using namespace std;

void push(string);
void pop();
int find(string);
void printStack();

static int varCount = 0;
deque<string> varStack;

/* perform left to right traversal, and perform different actions depending on subtree and node visited */
void statSem(node_t *node)
{
	if (node->label == programN)
	{
		// traverse program identifiers
		if (node->child[0] != NULL)
			statSem(node->child[0]);

		// traverse program statements
		statSem(node->child[1]);

		// OK message
		printf("Static semantics OK\n");
		return;	
	}
	else if (node->label == blockN)
	{
		// store previous number of variables
		int tempVarCount = varCount;
		varCount = 0;

		// traverse block identifiers
		if (node->child[0] != NULL)
			statSem(node->child[0]);

		// traverse block statements
		statSem(node->child[1]);

		// remove local variables before leaving scope
		while (varCount > 0)
			pop();

		// return to tracking previous scope identifiers
		varCount = tempVarCount;
		return;	
	}
	else if (node->label == varsN)
	{
		// check identifier hasn't already been defined in the scope
		int loc = find(node->token[0].inst);
		if ((loc >= 0) && (loc < varCount))
		{
			printf("Line %d: Identifier \'%s\' already defined in scope\n", node->token[0].line, node->token[0].inst);
			exit(1);
		}
		push(node->token[0].inst);

		// traverse <vars> nodes
		if (node->child[0] != NULL)
			statSem(node->child[0]);
		return;
	}
	else if (((node->label == RN) || (node->label == inN) || (node->label == assignN)) && (node->token[0].type == IDtk))
	{
		// check if identifier is in the stack
		if (find(node->token[0].inst) == -1)
		{
			// error message with line
			printf("Line %d: Identifier %s not defined\n", node->token[0].line, node->token[0].inst);
			exit(1);
		}

		// traverse child nodes
		if (node->child[0] != NULL)
			statSem(node->child[0]);
		return;
	}
	else
	{
		// traverse child nodes
		for (int i = 0; i < 4; i++)
			if (node->child[i] != NULL)
				statSem(node->child[i]);
		return;
	}
}

/* add string to front of stack */
void push(string id)
{
	varStack.push_front(id);
	varCount++;
	return;
}

/* remove string from front of stack */
void pop()
{
	varStack.pop_front();
	varCount--;
	return;
}

/* traverse stack and return location of identifier or -1 if not in stack */
int find(string id)
{
	for (int i = 0; i < varStack.size(); i++)
		if (varStack.at(i) == id)
			return i;
	return -1;
}

void printStack()
{
	for (int i = 0; i < varStack.size(); i++)
		printf("%s ", varStack.at(i).c_str());
	printf("\n");
}
