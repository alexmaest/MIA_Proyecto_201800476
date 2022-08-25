#ifndef NODE_H
#define NODE_H
#include "QString"
#include "QList"
#include <list>
using namespace std;

class node
{
public:
    node();

    int linea;
    int columna;
    QString tipo;
    QString valor;

    node(int linea,int columna,QString t, QString v);

    void add(node child);

    QList<node> childs;

    int tipo_;

    int CommandType();

};

#endif // NODE_H
