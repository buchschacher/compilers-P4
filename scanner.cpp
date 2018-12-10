#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <string>
#include <map>

#include "main.h"
#include "token.h"

using namespace std;

int getNextState(int, char);
tokenID termType(int);
tokenID keywordLookup(char*);
tokenID punctLookup(char*);
int path(char);
void error(int);

static int line = 1;

/* Return next token from the file */
token_t scanner()
{
	static char nextChar = fgetc(fp);
	int state = 0;

	// FSA diagram represented as a table
	const int states = 5;
	const int paths = 8;
	const int fsa[states][paths] = {
		{  0,   1,  -2,   3,   2,   4, 100,  -1},
		{101,   1,   1,   1, 101, 101, 101,  -1},
		{102, 102, 102, 102, 102, 102, 102,  -1},
		{103, 103, 103,   3, 103, 103, 103,  -1},
		{  4,   4,   4,   4,   4,   0,   0,   4}
	};

	// Initialize token
	token_t token;
	token.inst[0] = '\0';
	token.line = line;

	// Repeat until terminal state
	while ((state >= 0) && (state <= 4))
	{
		int nextState = fsa[state][path(nextChar)];
		if (nextState < 0)
			error(nextState);
		else if (nextState > 4)
		{
			// Check for reserved words
			if ((token.type = termType(nextState)) == IDtk)
				token.type = keywordLookup(token.inst);
			// Determine punctuation token
			else if ((token.type = termType(nextState)) == PNCtk)
				token.type = punctLookup(token.inst);
			//printf("%s\n", tokenNames[token.type]);
			return token;
		}
		else
		{
			// Track line number
			if (nextChar == '\n')
				token.line = ++line;

			// Add nextChar to string
			if ((nextState != 0) && (nextState != 4))
			{
				token.inst[strlen(token.inst) + 1] = '\0';
				token.inst[strlen(token.inst)] = nextChar;
			}

			// Update state and get next character
			state = nextState;
			nextChar = fgetc(fp);
		}
	}
}


/* Take a character as input and return the column number of the the corresponding path based on the
 * FSA graph */
int path(char nextChar)
{
	if (isspace(nextChar))
		return 0;
	if (islower(nextChar))
		return 1;
	if (isupper(nextChar))
		return 2;
	if (isdigit(nextChar))
		return 3;
	if (ispunct(nextChar) && nextChar != '#')
		return 4;
	if (nextChar == '#')
		return 5;
	if (nextChar == EOF)
		return 6;
	return 7;
}

/* Output error message with line number */
void error(int errorNum)
{
	if (errorNum == -1)
		printf("Scanner Error: Invalid character on line %d\n", line);
	else if (errorNum == -2)
		printf("Scanner Error: Invalid token on line %d\n", line);
	else if (errorNum == -3)
		printf("Scanner Error: Invalid state reached\n");
	exit(1);
}

/* Pass the terminal state of the FSA and return the token type or error message */
tokenID termType(int state)
{
	if (state == 100)
		return EOFtk;
	if (state == 101)
		return IDtk;
	if (state == 102)
		return PNCtk;
	if (state == 103)
		return INTtk;
	error(-3);
}

/* Pass the instance from a token and determine if it is a reserve word and return the corrisponding
 * token type for the keyword or identifier */
tokenID keywordLookup(char *inst)
{
	// Initialize keyword list
	string sInst = inst;
	static map<string, tokenID> keywordList;
	if (keywordList.empty())
	{
		keywordList.insert(pair<string, tokenID>("begin", BGNtk));
		keywordList.insert(pair<string, tokenID>("end", ENDtk));
		keywordList.insert(pair<string, tokenID>("iter", ITERtk));
		keywordList.insert(pair<string, tokenID>("void", VOIDtk));
		keywordList.insert(pair<string, tokenID>("var", VARtk));
		keywordList.insert(pair<string, tokenID>("return", RTNtk));
		keywordList.insert(pair<string, tokenID>("read", READtk));
		keywordList.insert(pair<string, tokenID>("print", PRNTtk));
		keywordList.insert(pair<string, tokenID>("program", PROGtk));
		keywordList.insert(pair<string, tokenID>("cond", CONDtk));
		keywordList.insert(pair<string, tokenID>("then", THENtk));
		keywordList.insert(pair<string, tokenID>("let", LETtk));
	}
	map<string, tokenID>::iterator it;

	// Check if identifier is in the keyword list
	it = keywordList.find(sInst);
	if (it != keywordList.end())
		return it->second;
	return IDtk;
}

/* Pass the instance from a token and determine if it is a valid punctuation mark and return the
 * corrisponding token or error */
tokenID punctLookup(char *inst)
{
	// Initialize operators and delimiters list
	string sInst = inst;
	static map<string, tokenID> punctList;
	if (punctList.empty())
	{
		punctList.insert(pair<string, tokenID>("=", EQUtk));
		punctList.insert(pair<string, tokenID>("<", LSTtk));
		punctList.insert(pair<string, tokenID>(">", GRTtk));
		punctList.insert(pair<string, tokenID>(":", COLtk));
		punctList.insert(pair<string, tokenID>("+", PLUStk));
		punctList.insert(pair<string, tokenID>("-", MINtk));
		punctList.insert(pair<string, tokenID>("*", MULTtk));
		punctList.insert(pair<string, tokenID>("/", DIVtk));
		punctList.insert(pair<string, tokenID>("%", MODtk));
		punctList.insert(pair<string, tokenID>(".", DOTtk));
		punctList.insert(pair<string, tokenID>("(", LPtk));
		punctList.insert(pair<string, tokenID>(")", RPtk));
		punctList.insert(pair<string, tokenID>(",", COMtk));
		punctList.insert(pair<string, tokenID>("{", LCtk));
		punctList.insert(pair<string, tokenID>("}", RCtk));
		punctList.insert(pair<string, tokenID>(";", SEMItk));
		punctList.insert(pair<string, tokenID>("[", LBtk));
		punctList.insert(pair<string, tokenID>("]", RBtk));

	}
	map<string, tokenID>::iterator it;

	// Check if identifier is in the punct list
	it = punctList.find(sInst);
	if (it != punctList.end())
		return it->second;
	error(-1);
}
