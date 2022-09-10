#ifndef COMMANDS_H
#define COMMANDS_H
#include "node.h"
#include "QString"
#include <iostream>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include "QStringList"
#include "structures.h"

using namespace std;
extern int singleVar;
extern mount repoMount[50];

class commands{
public:
    commands();
    bool secondScanner;

    void CommandTypes();
    int RandomNumber();
    bool FileExist(string path);

    QString MainCommandsRead(node *root);
    QString ReadFile(node *ext);
    QString QuotesClean(QString input);

    QString MkDiskExecute(node *mk);
    void CreateDisk(int singleSize,int units,char setting,string path);
    bool PathExist(QString path);

    QString RmDiskExecute(node *rm);
    void DeleteDisk(string path);

    void FDiskExecute(node *fd);
    int CreatePartition(int singleSize,int units,char setting,char part,QString path, QString name);
    bool PartitionsVerification(mbr disk,char typeSigle);
    bool SpaceVerification(mbr disk,int space);
    bool FirstPartition(mbr disk);
    int CreateInitialEBR(QString path,int initialPosition);

    mbr BestSetting(mbr disk,int singleSize,char setting,char typePart,char *name,QString path);
    mbr FirstSetting(mbr disk,int singleSize,char setting,char typePart,char *name,QString path);
    mbr WorstSetting(mbr disk,int singleSize,char setting,char typePart,char *name,QString path);

    int DeletePartition(int typeDelete,QString name,QString path);
    int AddPartition(int unity,int singleSize,QString name,QString path);
    mbr PartitionSort(mbr disk);
    int AddLogic(int unity,int singleSize,QString name,QString path,int startEXT,int sizeEXT);
    int DeleteLogic(int typeDelete,QString namePart, QString path,int startEXT,int sizeEXT);

    bool SameNamesEXT(QString path,int initialPos,QString namePart);
    bool SpaceEXT(QString path,int initialPos,int sizePart,int sizeLogic);

    int FirstSettingEXT(int sizePart,char settingPart,QString name,QString path,int initialEXT, int sizeEXT);
    int WorstSettingEXT(int sizePart,char settingPart,QString name,QString path,int initialEXT, int sizeEXT);
    int BestSettingEXT(int sizePart,char settingPart,QString name,QString path,int initialEXT, int sizeEXT);

    void ExecuteMount(node *mount);
    int DoMount(QString path,QString name);
    void ExecuteDismount(node *dismount);
    int DoDismount(QString partName);

    int bloque_con_espacio;
    int pos_bloque_con_espacio;
    int inodo_con_espacio;
    int pos_inodo_con_espacio;
    int padre;
    int bloque;
    int block_indirecto;
    int pos_block_indirecto;
    int tipo_op_archivo;
    //QString ruta_contenido;
    int indirecto_archivo_carpeta;

    int exclusivo_archivos;


    QString comando_ejec;

    int global_size;
    QString global_cont;


    QString contenidos_archivos;
    int tamanio_archivo;


    QString valor;
    superBloque global_sb;
    int posicion_inodo_archivo;
    //para indirectos


    QString Quitar_Comillas(QString v);

    void Leer_Principales(Nodo *raiz);
    int Ejecutar_MKFS(Nodo *mkfs);
    int Formatear_Particion_Ext2(QString id_parte,int tipo_system,int tipo_formateo);

    int Creacion_Carpeta_Raiz(superBloque S_b_temporal);
    int Creacion_Archivo_Usuario(superBloque s_b_temporal);

    int Calculo_de_N(int tipo_system);

    bool Buscar_Montada(QString id_montaje);
    bool Buscar_particion();

    int LLenar_Bitmap_Inodos(int inicio_inodo, int final_inodo, int inicio_bloque, int final_bloque);
    int LLenar_Journal(int inicio_jor,int fin_jor);

    int Ejecutar_Login(Nodo *log);
    int Iniciar_Sesion(QString user,QString pass, QString id_part);

    //cierre de sesion
    int Ejecutar_Logout();

    bool Sesion_Activa();

    //para crear archivos
    int Ejecutar_Mkdir(Nodo * dir);
    int Setear_Inicio(QString ruta, bool parametro_p, int tipo_operacion, QString ruta_count, int size, int size_o_cont);

    int Crear_Carpeta(QStringList rutas,bool parametro_p,superBloque sb,int pos_bmi,int pos);
    int Operar_Carpeta(superBloque sb, char *nom_carpeta, int enlace);


    inodo Crear_Inodo(int bloque_libre);
    carpeta Crear_Bloque_C(int padre, int actual);

    superBloque Cambiar_Libre_Inodo(superBloque sb);
    superBloque Cambiar_Libre_Bloque(superBloque sb);

    //MAnejo de archivos
    int Ejecutar_Mkfile(Nodo *arc);
    int Crear_Archivo(int pos_bmi, superBloque sb, QString nombre_ar, QString ruta_cont, int size, int tipo);
    int Operar_Archivo(superBloque sb, char *nom_carpeta, int tamanio, QString ruta);
    superBloque Crear_Bloque_A(int tamanio, inodo tem_i, superBloque sb, QString ruta_contenido, int pos_inodo);


    int Buscar_Indirectos(QStringList rutas, bool parametro_p, superBloque sb, int indirecto, int pos_ruta, int block, FILE *archivo);

    void Setear_Variables(int n1,int n2,int n3,int n4,int n5,int n6);
    void Reset_Variables();

    int Crear_Indirectos_Simples(int pos_inodo, int block, superBloque sb, char *nom_carpeta);

    int Crear_Indirectos_Dobles(int pos_inodo, int block, superBloque sb,char *nom_carpeta);

    int Crear_Indirectos_Triples(int pos_inodo, int block, superBloque sb, char *nom_carpeta);

    int Crear_Indrecto(superBloque sb, inodo inodo_padre, char * nom_carpeta, int inode_bm);

    int Ejecutar_Loos(Nodo *lo);
    int Comando_Loss(QString  id_part);
    int LLenar_Nulos(int inicio, int final, FILE *archivo, int tipo);

    int Ejecutar_Recovery(Nodo *rec);
    int Comando_Recovery(QString id_part);
    QString Leer_Jor(FILE *archivo);


    int Escribir_Jor();

    //----cat

    int Ejecutar_Cat(Nodo *cat);

    int Leer_Archivo(QString ruta);

    int Buscar_Carpeta_Archivo(int pos_bmi, superBloque sb, QStringList rutas, int pos);

    int Buscar_Indirectos_C_A(QStringList rutas, superBloque sb, int indirecto, int pos_ruta, int block, FILE *archivo);

    int Recorrer_Bloque_Archivos(int pos_inodo,superBloque sb);

    int Ejecutar_Mkgrp(Nodo *grupo);

    int Agregar_Grupos(QString nombre_grupo);
    int Recorrer_Grupos(QString con_archivo, QString nombre, QString nombre_usuario, int iniciar);

    int Agregar_Eliminar_Contenido_Archivos(QString contenido_nuevo, int pos_inodo, superBloque sb);

    //crear usuarios
    int Ejecutar_Mkuser(Nodo *usuario);
    int Agregar_Usuario(QString nombre_usuario, QString password, QString nombre_grupo);


    int Usuario_Sesion(QString nombre_usuario, QString pass, QString contenido, QString id_part);

    int Buscar_Usuario(QString id_particion, QString nombre_usuario, QString pass);

};

#endif // COMMANDS_H
