#include "node.h"

node::node()
{

}

node::node(int linea, int columna, QString t, QString v){
    this->linea=linea;
    this->columna=columna;
    this->valor=v;
    this->tipo=t;
    childs = QList<node>();
    this->tipo_=CommandType();
}

void node::add(node child){
    this->childs.append(child);
}

int node::CommandType(){
    if(QString::compare(this->tipo,"exec",Qt::CaseInsensitive)==0){
        return 0;
    }else if(QString::compare(this->tipo,"mkdisk",Qt::CaseInsensitive)==0){
        return 1;
    }else if(QString::compare(this->tipo,"rmdisk",Qt::CaseInsensitive)==0){
        return 2;
    }else if(QString::compare(this->tipo,"fdisk",Qt::CaseInsensitive)==0){
        return 3;
    }else if(QString::compare(this->tipo,"Mount",Qt::CaseInsensitive)==0){
        return 4;
    }else if(QString::compare(this->tipo,"unmount",Qt::CaseInsensitive)==0){
        return 5;
    }else if(QString::compare(this->tipo,"rep",Qt::CaseInsensitive)==0){
        return 6;
    }else if(QString::compare(this->tipo,"s",Qt::CaseInsensitive)==0){
        return 7;
    }else if(QString::compare(this->tipo,"f",Qt::CaseInsensitive)==0){
        return 8;
    }else if(QString::compare(this->tipo,"u",Qt::CaseInsensitive)==0){
        return 9;
    }else if(QString::compare(this->tipo,"path",Qt::CaseInsensitive)==0){
        return 10;
    }else if(QString::compare(this->tipo,"t",Qt::CaseInsensitive)==0){
        return 11;
    }else if(QString::compare(this->tipo,"delete",Qt::CaseInsensitive)==0){
        return 12;
    }else if(QString::compare(this->tipo,"name",Qt::CaseInsensitive)==0){
        return 13;
    }else if(QString::compare(this->tipo,"add",Qt::CaseInsensitive)==0){
        return 14;
    }else if(QString::compare(this->tipo,"id",Qt::CaseInsensitive)==0){
        return 15;
    }else if(QString::compare(this->tipo,"mkfs",Qt::CaseInsensitive)==0){
        return 16;
    }else if(QString::compare(this->tipo,"fs",Qt::CaseInsensitive)==0){
        return 17;
    }else if(QString::compare(this->tipo,"mkdir",Qt::CaseInsensitive)==0){
        return 18;
    }else if(QString::compare(this->tipo,"login",Qt::CaseInsensitive)==0){
        return 19;
    }else if(QString::compare(this->tipo,"usr",Qt::CaseInsensitive)==0){
        return 20;
    }else if(QString::compare(this->tipo,"pass",Qt::CaseInsensitive)==0){
        return 21;
    }else if(QString::compare(this->tipo,"logout",Qt::CaseInsensitive)==0){
        return 22;
    }else if(QString::compare(this->tipo,"r",Qt::CaseInsensitive)==0){
        return 23;
    }else if(QString::compare(this->tipo,"mkfile",Qt::CaseInsensitive)==0){
        return 24;
    }else if(QString::compare(this->tipo,"cont",Qt::CaseInsensitive)==0){
        return 25;
    }else if(QString::compare(this->tipo,"loss",Qt::CaseInsensitive)==0){
        return 26;
    }else if(QString::compare(this->tipo,"recovey",Qt::CaseInsensitive)==0){
        return 27;
    }else if(QString::compare(this->tipo,"cat",Qt::CaseInsensitive)==0){
        return 28;
    }else if(QString::compare(this->tipo,"fileN",Qt::CaseInsensitive)==0){
        return 29;
    }else if(QString::compare(this->tipo,"pause",Qt::CaseInsensitive)==0){
        return 30;
    }else if(QString::compare(this->tipo,"ruta",Qt::CaseInsensitive)==0){
        return 31;
    }else if(QString::compare(this->tipo,"mkgrp",Qt::CaseInsensitive)==0){
        return 32;
    }else if(QString::compare(this->tipo,"grp",Qt::CaseInsensitive)==0){
        return 33;
    }else if(QString::compare(this->tipo,"mkusr",Qt::CaseInsensitive)==0){
        return 34;
    }else{
        return -1;
    }
}
