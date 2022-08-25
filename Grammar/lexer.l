%option case-insensitive 
%option yylineno
%option header-file="scanner.h"
%option outfile="scanner.cpp"
%option noyywrap
%option nounput

%{
    #include "parser.h"
    #include <cstdlib>
    #include <string>
    #include <iostream>
    #include "node.h"
    #include <cstdlib>
    #include <stdio.h>

    int yylex();
    void yyerror(const char *s);
    int linea=1;
    int columna=1;
    int anterior=0;

    node *raiz;

    void consolePrint (const char *message,const char *tkn){
       // std::cout<<message<<" : "<<tkn<<std::endl;
    }

%}

numero [0-9]+
numeroN "-"{numero}
iden [a-zñA-ZÑ]+
cadena "\""[^"\""]*"\""
cadena2 ([a-zA-Z]\:)?[a-zA-Z0-9\/s_\/.\/-\/(\/):]+
otros \t|\r|\f

%x COM

%%
INITIAL
{
    "#"             {BEGIN COM;}
    {otros}         {/*igore white espace*/}
    
    {iden}        {consolePrint ("iden",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return iden;}
  
    {numero}      {consolePrint ("numero",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return numero;}

    {cadena}     {consolePrint("cadena",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return cadena;}

    {cadena2}       {consolePrint("cadena2",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return cadena2;}

    {numeroN}      {consolePrint ("numeroN",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return numeroN;}

     "->"         {consolePrint("flecha",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return Tflecha;}

     "-"              {consolePrint("guion",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return Tguion;}

      " "             {columna=columna+anterior; anterior=strlen(yytext);  strcpy(yylval.TEXT,yytext); }

     "\n"            {linea++; columna=0;anterior=0;}

}

<COM>"\n"      {linea++; columna=0; BEGIN INITIAL;}
<COM>.;      
%%
