#include <QCoreApplication>

#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include "Grammar/parser.h"
#include "Grammar/scanner.h"

#include <stdlib.h>
#include "QString"
#include "QFile"
#include "commands.h"
#include "structures.h"
#include <QTextStream>
#include <fstream>

using namespace std;

int singleVar;
mount repoMount[50];
login mainUser;

mount temporal;
singlePartition tem_part;

extern int yyparse();
extern node *raiz;
extern int linea;
extern int columna;
extern int yylineno;

int globalValue = 1;
char globalInput[500];

void otherExecution(QString entrada){
    QFile entryFile(entrada);
    QString textData = "";
    if (entryFile.open(QIODevice::ReadOnly|QIODevice::Text)){
       textData = QString::fromUtf8(entryFile.readAll());
       QFile file("temp.txt");
       if (file.open(file.WriteOnly)){
            QTextStream stream1(&file);
            stream1 << textData;
       }
       const char* x = "temp.txt";
       FILE* input = fopen(x, "r");
       yyrestart(input);
       if(yyparse() == 0){
            commands *execute = new commands();
            execute->MainCommandsRead(raiz);
            delete execute;
        }else{
            cout<<"Error: Hay errores en el comando solicitado"<<endl;
        }
    }else{
        cout<<"Error: No se puede ejecutar el comando exec porque la ruta no existe"<<endl;
    }
}

void startExecution(){
    ofstream singleFile;
    singleFile.open("tem.txt",ios::out);

    if(singleFile.fail()!=true){
        singleFile << globalInput;
        singleFile.close();
        const char *x = "tem.txt";
        FILE *inputFile = fopen(x,"r");
        yyrestart(inputFile);

        if(yyparse() == 0){
            commands *execCommand = new commands();
            QString outText = execCommand->MainCommandsRead(raiz);
            if(QString::compare(outText,"",Qt::CaseInsensitive)!=0){
                otherExecution(outText);
            }
            delete execCommand;
        }else{
            cout<<"Error: Hay errores en el comando solicitado"<<endl;
        }
    }
}

int main(int argc, char *argv[]){
    singleVar=0;
    mainUser.session='0';
    cout<<endl<<endl;
    cout<<"*********************************************************************"<<endl;
    cout<<"                    SISTEMA DE ARCHIVOS EXT2/EXT3                   *"<<endl;
    cout<<"*********************************************************************"<<endl;
    cout<<"*                                                                   *"<<endl;
    cout<<"*             Estudiante: Marvin Alexis Estrada Florian             *"<<endl;
    cout<<"*                        Carnet: 201800476                          *"<<endl;
    cout<<"*                                                                   *"<<endl;
    cout<<"*********************************************************************"<<endl<<endl;
    while(globalValue!=0){
        cout<<">> Escriba un comando:"<<endl;
        cin.getline(globalInput,sizeof(globalInput));
        startExecution();
    }
    return 0;
}
