%{
#include <stdio.h>
#include <string>
#include "nnet.h"

extern FILE* yyin;
extern char* yytext;
extern int yylex();
extern int yylineno;

int yyerror(const char* s);
static std::string curIndent = "";
static std::string curAttriBlock = "";
static int curAttriLevel;

NNet* pnnet = NULL;

%}

%union{
      char*     ptr;
}

%start ROOT

%token <ptr>    IDENT
%token          QUOT COLON BRACE_L BRACE_R 

%%


ROOT:
       globalattribs layers {
       }

layers: 
       layer {
       } | layers layer {
       }

layer: 
       IDENT {
            pnnet->beginLayer();
       } BRACE_L {
            curAttriLevel = 1;
       } attribs {
            free($1);
       } BRACE_R {
            pnnet->endLayer();
       }


globalattribs: 
       globalattrib {
       } | globalattribs globalattrib {
       }

globalattrib: 
       IDENT COLON IDENT {
            pnnet->setAttri(0, NULL, $1, $3);
            free($1);
            free($3);
       } | IDENT COLON QUOT idents QUOT {
            pnnet->setAttri(0, NULL, $1, curIndent.c_str());
            curIndent = "";
            free($1);
       }

attribs: 
       attrib {
       } | attribs attrib {
       }

attrib: 
       IDENT COLON IDENT {
            pnnet->setAttri(curAttriLevel, curAttriBlock.c_str(), $1, $3);
            free($1);
            free($3);
       } | IDENT COLON QUOT idents QUOT {
            pnnet->setAttri(curAttriLevel, curAttriBlock.c_str(), $1, curIndent.c_str());
            curIndent = "";
            free($1);
       } | IDENT COLON BRACE_L {
            ++curAttriLevel;
            curAttriBlock = $1;
            free($1);
       } attribs {
       } BRACE_R {
            --curAttriLevel;
            curAttriBlock = "";
       } | IDENT BRACE_L {
            ++curAttriLevel;
            curAttriBlock = $1;
            free($1);
       } attribs {
       } BRACE_R {
            --curAttriLevel;
            curAttriBlock = "";
       }

idents: 
       IDENT {
            curIndent = curIndent + " " + $1;
            free($1);
       } | idents IDENT {
            curIndent = curIndent + " " + $2;
            free($2);
       }

%%

int yyerror(const char* s){
    printf("%s\nlineno: %d", s, yylineno);
}

int parse(const char* filename, NNet* pNNetOut){
    yyin = fopen(filename,"r");
    pnnet = pNNetOut;
    if(yyin == NULL){
         printf("error in open file: %s\n",filename);
         return -1;
    }

    yyparse();

    fclose(yyin);
    yyin = NULL;
    return 0;
}

