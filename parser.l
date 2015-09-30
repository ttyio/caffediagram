%{
#include "parser_y.hpp"
%}
%option noyywrap
%%

"\""         {return QUOT;}
":"          {return COLON;}
"{"          {return BRACE_L;}
"}"          {return BRACE_R;}
([a-zA-Z0-9]|"-"|"\."|"_"|"~"|"/"|"\\")* {yylval.ptr = strdup(yytext); return IDENT;}

