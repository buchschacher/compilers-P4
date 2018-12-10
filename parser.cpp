#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "token.h"
#include "node.h"
#include "scanner.h"

node_t* program();
node_t* block();
node_t* vars();
node_t* expr();
node_t* a();
node_t* m();
node_t* r();
node_t* stats();
node_t* mstat();
node_t* stat();
node_t* in();
node_t* out();
node_t* iff();
node_t* loop();
node_t* assign();
node_t* ro();
void error();
token_t token;

node_t* parser()
{
	node_t *root = NULL;
	token = scanner();
	root = program();

	// check for end of file token
	if (token.type == EOFtk)
	{
		printf("Parse was OK\n");
		return root;
	}	
	error();
}

/* <program> -> void <vars> <block> */
node_t* program()
{
	node_t *node = new node_t;
	node->label = programN;

	// check for matching void
	if (token.type == VOIDtk)
	{
		token = scanner();
		node->child[0] = vars();
		node->child[1] = block();
		return node;
	}
	error();
}

/* <block> -> begin <vars> <stats> end */
node_t* block()
{
	node_t *node = new node_t;
	node->label = blockN;

	// check for matching begin
	if (token.type == BGNtk)
	{
		token = scanner();
		node->child[0] = vars();
		node->child[1] = stats();
		
		// check for matching end
		if (token.type == ENDtk)
		{
			token = scanner();
			return node;
		}
	}
	error();
}

/* <vars> -> empty | let id = int <vars> */
node_t* vars()
{
	// predict  <vars> -> let id = int <vars> production
	if (token.type == LETtk)
	{
		node_t *node = new node_t;
		node->label = varsN;
		token = scanner();

		// check for matching id
		if (token.type == IDtk)
		{
			node->token[0] = token;
			token = scanner();

			// check for matching =
			if (token.type == EQUtk)
			{
				token = scanner();

				// check for matching int
				if (token.type == INTtk)
				{
					node->token[1] = token;
					token = scanner();
					node->child[0] = vars();
					return node;
				}
			}
		}
		error();
	}

	// empty production
	return NULL;
}

/* <expr> -> <A> / <expr> | <A> * <expr> | <A> */
node_t* expr()
{
	node_t *node = new node_t;
	node->label = exprN;
	node->child[0] = a();

	// predict <expr> -> <A> / <expr> or <expr> -> <A> * <expr> production
	if ((token.type == DIVtk) || (token.type == MULTtk))
	{
		node->token[0] = token;
		token = scanner();
		node->child[1] = expr();
		return node;
	}

	// <expr> -> <A> production
	return node;
}

/* <A> -> <M> + <A> | <M> - <A> | <M> */
node_t* a()
{
	node_t *node = new node_t;
	node->label = AN;
	node->child[0] = m();

	// predict <A> -> <M> + <A> or <A> -> <M> - <A>
	if ((token.type == PLUStk) || (token.type == MINtk))
	{
		node->token[0] = token;
		token = scanner();
		node->child[1] = a();
		return node;
	}

	// <A> -> <M> production
	return node;
}

/* <M> -> - <M> | <R> */
node_t* m()
{
	node_t *node = new node_t;
	node->label = MN;

	// predict <M> -> - <M>
	if (token.type == MINtk)
	{
		node->token[0] = token;
		token = scanner();
		node->child[0] = m();
		return node;
	}

	// predict <M> -> <R>
	else 
	{
		node->child[0] = r();
		return node;
	}
}

/* <R> -> ( <expr> ) | id | int */
node_t* r()
{
	node_t *node = new node_t;
	node->label = RN;

	// predict <R> -> ( <expr> ) production
	if (token.type == LPtk)
	{
		token = scanner();
		node->child[0] = expr();

		// check for matching )
		if (token.type == RPtk)
		{
			token = scanner();
			return node;
		}
		error();
	}

	// predict <R> -> id production
	if (token.type == IDtk)
	{
		node->token[0] = token;
		token = scanner();
		return node;
	}

	// predict <R> -> int production
	if (token.type == INTtk)
	{
		node->token[0] = token;
		token = scanner();
		return node;
	}

	error();
}

/* <stats> -> <stat> <mStat> */
node_t* stats()
{
	node_t *node = new node_t;
	node->label = statsN;
	node->child[0] = stat();		// process <stats>
	node->child[1] = mstat();		// process <mStat>
	return node;
}

/* <mStat> -> empty | <stat> <mStat> */
node_t* mstat()
{
	// predict <mStat> -> <stat> <mStat> production
	if ((token.type == READtk) || (token.type == BGNtk) || (token.type == ITERtk) || (token.type == PRNTtk) || (token.type == IDtk) || (token.type == CONDtk))
	{

		node_t *node = new node_t;
		node->label = mStatN;

		node->child[0] = stat();	// process <stat>
		node->child[1] = mstat();	// process <mStat>
		return node;
	}

	// empty production
	return NULL;
}

/* <stat> -> <in> | <out> | <block> | <if> | <loop> | <assign> */
node_t* stat()
{
	node_t *node = new node_t;
	node->label = statN;

	// <stat> -> <in> production
	if (token.type == READtk)
	{
		// process <in>
		node->child[0] = in();
		return node;
	}

	// <stat> -> <out> production
	if (token.type == PRNTtk)
	{
		// process <out>
		node->child[0] = out();
		return node;
	}

	// <stat> -> <block> production
	if (token.type == BGNtk)
	{
		// process <block>
		node->child[0] = block();
		return node;
	}

	// <stat> -> <if> production
	if (token.type == CONDtk)
	{
		// process <if>
		node->child[0] = iff();
		return node;
	}

	// <stat> -> <loop> production
	if (token.type == ITERtk)
	{
		// process <loop>
		node->child[0] = loop();
		return node;
	}

	// <stat> -> <assign> production
	if (token.type == IDtk)
	{
		node->child[0] = assign();
		return node;
	}

	error();
}

/* <in> -> read ( id ) : */
node_t* in()
{
	node_t *node = new node_t;
	node->label = inN;

	// check for matching read
	if (token.type == READtk)
	{
		token = scanner();

		// check for matching (
		if (token.type == LPtk)
		{
			token = scanner();

			// check for matching id
			if (token.type == IDtk)
			{
				node->token[0] = token;
				token = scanner();

				// check for matching )
				if (token.type == RPtk)
				{
					token = scanner();

					// check for matching :
					if (token.type == COLtk)
					{
						token = scanner();
						return node;
					}
				}
			}
		}
	}
	error();
}

/* <out> -> print ( <expr> ) : */
node_t* out()
{
	node_t *node = new node_t;
	node->label = outN;

	// check for matching print
	if (token.type == PRNTtk)
	{
		token = scanner();

		// check for matching (
		if (token.type == LPtk)
		{
			token = scanner();
			node->child[0] = expr();

			// check for matching )
			if (token.type == RPtk)
			{
				token = scanner();

				// check for matching :
				if (token.type == COLtk)
				{
					token = scanner();
					return node;
				}
			}
		}
	}
	error();
}

/* <if> -> cond ( <expr> <RO> <expr> ) <stat> */
node_t* iff()
{
	node_t *node = new node_t;
	node->label = ifN;

	// check for matching cond
	if (token.type == CONDtk)
	{
		token = scanner();

		// check for matching (
		if (token.type == LPtk)
		{
			token = scanner();
			node->child[0] = expr();
			node->child[1] = ro();
			node->child[2] = expr();

			// check for matching )
			if (token.type == RPtk)
			{
				token = scanner();
				node->child[3] = stat();
				return node;
			}
		}
	}
	error();
}

/* <loop> -> iter ( <expr> <RO> <expr> ) <stat> */
node_t* loop()
{
	node_t *node = new node_t;
	node->label = loopN;

	// check for matching inter
	if (token.type == ITERtk)
	{
		token = scanner();

		// check for matching (
		if (token.type == LPtk)
		{
			token = scanner();
			node->child[0] = expr();
			node->child[1] = ro();
			node->child[2] = expr();

			// check for matching )
			if (token.type == RPtk)
			{
				token = scanner();
				node->child[3] = stat();
				return node;
			}
		}
	}
	error();
}

/* <assign> -> id = <expr> : */
node_t* assign()
{
	node_t *node = new node_t;
	node->label = assignN;

	// check for matching id
	if (token.type == IDtk)
	{
		node->token[0] = token;
		token = scanner();

		// check for matching =
		if (token.type == EQUtk)
		{
			token = scanner();
			node->child[0] = expr();

			// check for matching :
			if (token.type == COLtk)
			{
				token = scanner();
				return node;
			}
		}
	}
	error();
}

/* <RO> -> < | < = | > | > = | = = | = */
node_t* ro()
{
	node_t *node = new node_t;
	node->label = RON;

	// predic <RO> -> < or <RO> -> < = productions
	if (token.type == LSTtk)
	{
		node->token[0] = token;
		token = scanner();
		if (token.type == EQUtk)
		{
			node->token[1] = token;
			token = scanner();
		}
		return node;
	}

	// predic <RO> -> > or <RO> -> > = productions
	if (token.type == GRTtk)
	{
		node->token[0] = token;
		token = scanner();
		if (token.type == EQUtk)
		{
			node->token[1] = token;
			token = scanner();
		}
		return node;
	}

	// predic <RO> -> = or <RO> -> = = productions
	if (token.type == EQUtk)
	{
		node->token[0] = token;
		token = scanner();
		if (token.type == EQUtk)
		{
			node->token[1] = token;
			token = scanner();
		}
		return node;
	}

	error();
}

void error()
{
	printf("Parser Error: Unexpected %s token \"%s\" on line %d\n", tokenNames[token.type], token.inst, token.line);
	exit(1);
}
