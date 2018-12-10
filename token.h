#ifndef TOKEN_H
#define TOKEN_H

enum tokenID {PNCtk, IDtk, INTtk, EOFtk, BGNtk, ENDtk, ITERtk, VOIDtk, VARtk, RTNtk, READtk, PRNTtk, PROGtk, CONDtk, THENtk, LETtk, EQUtk, LSTtk, GRTtk, COLtk, PLUStk, MINtk, MULTtk, DIVtk, MODtk, DOTtk, LPtk, RPtk, COMtk, LCtk, RCtk, SEMItk, LBtk, RBtk};
const char tokenNames[][20] = {"OPtk", "IDtk", "INTtk", "EOFtk", "BEGINtk", "ENDtk", "ITERtk", "VOIDtk", "VARtk", "RETURNtk", "READtk", "PRINTtk", "PROGRAMtk", "CONDtk", "THENtk", "LETtk", "=tk", "<tk", ">tk", ":tk", "+tk", "-tk", "*tk", "/tk", "%tk", ".tk", "(tk", ")tk", ",tk", "{tk", "}tk", ";tk", "[tk", "]tk"};

typedef struct token
{
	tokenID type;
	char inst[9];
	int line;
} token_t;

#endif
