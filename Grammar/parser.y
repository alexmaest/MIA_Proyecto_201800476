%{

#include "scanner.h"
#include "node.h"
#include <iostream>

extern int yylineno;
extern int columna;
extern int linea;
extern char *yytext;
extern node *raiz;

int yyerror(const char* mens)
{
    std::cout << mens <<" "<<yytext<<" Linea: "<<linea<<" Columna: "<<columna<< std::endl;
    return 0;
}

%}

%defines "parser.h"
%output "parser.cpp"
//%error-verbose
%locations

%union{
char TEXT [256];
class node *nodo;
}

//Terminales
%token<TEXT> numero;
%token<TEXT> numeroN;
%token<TEXT> cadena;
%token<TEXT> cadena2;
%token<TEXT> montar;
%token<TEXT> Tflecha;
%token<TEXT> Tguion;
%token<TEXT> iden;

//No terminales
%type<nodo> init;
%type<nodo> Lista_Comandos;
%type<nodo> Comando;
%type<nodo> Lista_Ejecutar;
%type<nodo> Parametro;

%start init
%%

init: Lista_Comandos{
        raiz=$1;
    };  

Lista_Comandos: Lista_Comandos Comando{
                    $$=$1;
                    $$->add(*$2);
                }
               |Comando{
                   $$=new node(@1.first_line, @1.first_column,"Lista_Comandos","Lista_Comandos");
                   
                   $$->add(*$1);
               };

Comando: iden Lista_Ejecutar{
            $$=new node(@1.first_line, @1.first_column,$1,$1);
            
            $$->add(*$2);
        }
        |iden{
             $$=new node(@1.first_line, @1.first_column,$1,$1);
             node *nodito=new node(@1.first_line, @1.first_column,$1,$1);
             $$->add(*nodito);
        };


Lista_Ejecutar: Lista_Ejecutar Parametro{
                    $$=$1;
                    $$->add(*$2);
                }
                |Parametro{
                    $$=new node(@1.first_line, @1.first_column,"Lista_Ejecutar","Lista_Ejecutar");
                    $$->add(*$1);
                };

Parametro: Tguion iden Tflecha iden{
            $$=new node(@1.first_line, @1.first_column,$2,$4);
            }
           |Tguion iden Tflecha cadena{
               $$=new node(@1.first_line, @1.first_column,$2,$4);
           }
           |Tguion iden Tflecha cadena2{
               $$=new node(@1.first_line, @1.first_column,$2,$4);
           }
           |Tguion iden Tflecha numero{
               $$=new node(@1.first_line, @1.first_column,$2,$4);
           }
           |
           Tguion iden Tflecha numeroN{
            $$=new node(@1.first_line, @1.first_column,$2,$4);
           }
           |Tguion iden{
               $$=new node(@1.first_line, @1.first_column,$2,$2);
           }
           ;