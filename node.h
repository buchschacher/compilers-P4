#ifndef NODE_H
#define NODE_H

#include "token.h"
enum nodeID {programN, blockN, varsN, exprN, AN, MN, RN, statsN, mStatN, statN, inN, outN, ifN, loopN, assignN, RON};

const char labelName[][20] = {"program", "block", "vars", "expr", "A", "M", "R", "stats", "mStat", "stat", "in", "out", "if", "loop", "assign", "RO"};

// Node structure 
typedef struct node
{
	nodeID label;
	//char label[8];
	token_t token[2];
	struct node *child[4];
} node_t;

#endif
