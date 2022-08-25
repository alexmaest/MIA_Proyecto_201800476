#include "commands.h"
#include <QtDebug>

enum Estados{

    EXEC,
    MKDISK,
    RMDISK,
    FDISK,
    MOUNT,
    UNMOUNT,
    REP,

    singleSize,
    fit,
    unit,
    path,
    type,
    DELETE,
    name,
    add,
    id,

    MKFS,
    fs,
    MKDIR,
    Login,
    usr,
    pwd,
    Logout,
    PP,
    MKFILE,
    Cont,
    LOSS,
    RECOVERY,
    CAT,
    File,
    PAUSE,
    Ruta
};

commands::commands()
{
    secondScanner=false;
}

QString commands::MainCommandsRead(node *root){
    for(int y=0;y<root->childs.size();y++){
        node tem = root->childs.at(y).childs.at(0);
        int typeSwitch = root->childs.at(y).tipo_;
        switch (typeSwitch) {
            case EXEC:
                return ReadFile(&tem);
             case MKDISK:
                MkDiskExecute(&tem);
                break;
            case RMDISK:
                RmDiskExecute(&tem);
                break;
            case FDISK:
                FDiskExecute(&tem);
                break;
            case MOUNT:
                ExecuteMount(&tem);
                break;
            case UNMOUNT:
                ExecuteDismount(&tem);
                break;
            case REP:
                //Case para reportes
                break;
            default:
                break;
        }
    }
    return "";
}

QString commands::ReadFile(node *ext){
    for(int y=0;y<ext->childs.size();y++){
        int tipo_=ext->childs.at(y).tipo_;
        switch (tipo_) {
            case path:
                return QuotesClean(ext->childs.at(y).valor);
                break;
            default:
                //Error no permite ese parametro
                break;
            }
    }
    return "";
}

bool commands::FileExist(string file){
    FILE * archivo;
    if (archivo = fopen(file.c_str(), "r")){
        fclose(archivo);
        return true;
    }else{
        return false;
    }
}

int commands::RandomNumber(){
    srand(time(NULL));
    int test = rand() % 1000;
    return test;
}

QString commands::QuotesClean(QString v){
    if(v.startsWith("\"")){
        if(v.endsWith("\"")){
            QString output;
            for(int a=1;a<=v.size()-2;a++){
                output.append(v[a]);
            }
            v = output;
         }
    }
    return v;
}

bool commands::PathExist(QString ruta){
    QStringList folderList = ruta.split("/");
    QString root="/home";
    for(int i=2;i<folderList.size();i++){
        root = root+"/"+folderList.at(i);
        DIR *path;
        if(path = opendir(root.toStdString().c_str())){
            closedir(path);
        }else{
            if(mkdir(root.toStdString().c_str(),0777)==0){
                cout<<"Carpeta creada"<<endl;
            }else{
                return false;
            }
        }
    }
    return true;
}

QString commands::MkDiskExecute(node *mk){
    bool is_size=false;
    bool is_path=false;
    int tamanio=0;
    int unidades=1024*1024;
    QString ruta="";
    char c='F';
    QString salida_consola="Mkdisk ";
    bool error_parametros=false;
    for(int i=0;i<mk->childs.size();i++){
        node tem=mk->childs.at(i);
        int tipo_=tem.tipo_;
        switch (tipo_) {
        case singleSize:
            try {
                tamanio=tem.valor.toInt();
            } catch (...) {

            }
            salida_consola=salida_consola+"-size="+tem.valor+" ";
            if(tamanio<=0){
                is_size=false;
            }else{
                 is_size=true;
            }
            break;
        case path:
            ruta=QuotesClean(tem.valor);
            is_path=true;
            salida_consola=salida_consola+"-path="+tem.valor+" ";
            break;
        case fit:
            if(QString::compare(tem.valor,"bf",Qt::CaseInsensitive)==0){
                c='B';
            }else if(QString::compare(tem.valor,"ff",Qt::CaseInsensitive)==0){
                c='F';
            }else if(QString::compare(tem.valor,"wf",Qt::CaseInsensitive)==0){
                c='W';
            }else{
                cout<<"Ajuste no valido"<<endl;
                error_parametros=true;
            }
            salida_consola=salida_consola+"-fit="+tem.valor+" ";
            break;
        case unit:
            if(QString::compare(tem.valor,"k",Qt::CaseInsensitive)==0){
                unidades=1024;
            }else if(QString::compare(tem.valor,"m",Qt::CaseInsensitive)==0){
                unidades=1024*1024;
            }else{
                cout<<"Unidades no validas, ingrese k o m"<<endl;
                error_parametros=true;
            }
            salida_consola=salida_consola+"-unit="+tem.valor+" ";
            break;
        default:
            break;
        }
    }
    std::cout<<salida_consola.toStdString()<<endl;
    if(is_size==true && is_path==true  ){
        if(error_parametros==false){
            QStringList lista=ruta.split("/");
            QString value="/home";
            for(int i=2;i<lista.size()-1;i++){
                value=value+"/"+lista.at(i);
            }
            if(PathExist(value)==true){
                CreateDisk(tamanio,unidades,c,ruta.toStdString());
                QString nombre_ext=lista.at(lista.size()-1);
                QStringList solo_nombre=nombre_ext.split(".");
                try {
                    QString ruta_raid=value+"/"+solo_nombre.at(0)+"_ra1.disk";
                    CreateDisk(tamanio,unidades,c,ruta_raid.toStdString());
                } catch (...) {
                    cout<<"Error: no se puede crear el raid "<<nombre_ext.toStdString()<<endl;
                }
            }else{
                cout<<"Direccion no valida o no fue posible la creacion del directorio"<<endl;
            }
        }else{
            //Los errores ya se imprimieron
        }
    }else{
        cout<<"Falto comando ruta o tamanio que debe ser mayor a 0"<<endl;
    }
    return "";
}

QString commands::RmDiskExecute(node *rm){
    bool is_path=false;
    QString ruta="";
    QString salida_consola="rmdisk ";
    for(int i=0;i<rm->childs.size();i++){
        node tem=rm->childs.at(i);
        int tipo_=tem.tipo_;
        switch (tipo_) {
        case path:
            ruta=QuotesClean(tem.valor);
            salida_consola=salida_consola+"-path="+tem.valor;
            is_path=true;
            break;
        default:
            cout<<"Comando no valido para rmdisk"<<endl;
            break;
        }
    }
    cout<<salida_consola.toStdString()<<endl;
    if(is_path==true){
        DeleteDisk(ruta.toStdString());
    }else{
        cout<<"No se especifico el comando path en rmdisk por lo que no se puede operar"<<endl;
    }

    return "";
}

void commands::FDiskExecute(node *fd){
    bool is_size=false;
    bool is_path=false;
    bool is_name=false;
    bool errores_general=false;
    int tamanio=0;
    int unidades=1024;
    int unidades2=-1;
    QString ruta="";
    QString nombre="";
    char tipo_par='P';
    char ajuste='W';
    int tipo_eliminacion=0;
    bool is_delete=false;
    bool is_add=false;

    for(int i=0;i<fd->childs.size();i++){
        node tem=fd->childs.at(i);
        int tipo_=tem.tipo_;
        switch (tipo_) {
        case singleSize:
            try {
                tamanio=tem.valor.toInt();
                is_size=true;
            } catch (...) {
                is_size=false;
            }
            if(tamanio<=0){
                cout<<"El tamanio de la particion debe ser mayor a 0"<<endl;
                errores_general=true;
            }
            break;
        case path:
            ruta=QuotesClean(tem.valor);
            is_path=true;
            break;
        case name:
             nombre=tem.valor;
             is_name=true;
            break;
        case unit:
                if(QString::compare(tem.valor,"b",Qt::CaseInsensitive)==0){
                    unidades=1;
                    unidades2=1;
                }else if(QString::compare(tem.valor,"k",Qt::CaseInsensitive)==0){
                    unidades=1024;
                    unidades2=1024;
                }else if(QString::compare(tem.valor,"m",Qt::CaseInsensitive)==0){
                    unidades=1024*1024;
                    unidades2=unidades;
                }else{
                    cout<<"La unidad especificada es invalida, debe ser b,k,m"<<endl;
                    errores_general=true;
                }
            break;
        case type:
                if(QString::compare(tem.valor,"p",Qt::CaseInsensitive)==0){
                    tipo_par='P';
                }else if(QString::compare(tem.valor,"e",Qt::CaseInsensitive)==0){
                    tipo_par='E';
                }else if(QString::compare(tem.valor,"L",Qt::CaseInsensitive)==0){
                    tipo_par='L';
                }else{
                    cout<<"El tipo de particion es invalida, debe ser p,e,l"<<endl;
                    errores_general=true;
                }
            break;
        case fit:
            if(QString::compare(tem.valor,"bf",Qt::CaseInsensitive)==0){
                ajuste='B';
            }else if(QString::compare(tem.valor,"ff",Qt::CaseInsensitive)==0){
                ajuste='F';
            }else if(QString::compare(tem.valor,"wf",Qt::CaseInsensitive)==0){
                ajuste='W';
            }else{
                cout<<"El ajuste especificado es invalido,debe ser bf,ff,wf"<<endl;
                errores_general=true;
            }
            break;
        case DELETE:
                is_delete=true;
                if(QString::compare(tem.valor,"full",Qt::CaseInsensitive)==0){
                    tipo_eliminacion=2;
                }else{
                    cout<<"El comando de delete no es valido"<<endl;
                    is_delete=false;
                }
            break;
        case add:
            try {
                tamanio=tem.valor.toInt();
                is_add=true;
            } catch (...) {
                is_add=false;
            }
            break;
        default:
            break;
        }
    }
    if(is_name && is_path && is_size){
        if(!errores_general){
            CreatePartition(tamanio,unidades,ajuste,tipo_par,ruta,nombre);
            QStringList lista=ruta.split("/");
            QString value="/home";
            for(int i=2;i<lista.size()-1;i++){
                value=value+"/"+lista.at(i);
            }
            QString nombre_ext=lista.at(lista.size()-1);
            try {
                QStringList solo_nombre=nombre_ext.split(".");
                QString ruta_raid=value+"/"+solo_nombre.at(0)+"_ra1.disk";
                CreatePartition(tamanio,unidades,ajuste,tipo_par,ruta_raid,nombre);
            } catch (...) {
                cout<<"Error: no se encontro el raid"<<nombre_ext.toStdString()<< endl;
            }
        }else{
            //Los errores ya se imprimieron
        }
    }else if(is_delete){
        if(is_name && is_path){
            DeletePartition(tipo_eliminacion,nombre,ruta);
            QStringList lista=ruta.split("/");
            QString value="/home";
            for(int i=2;i<lista.size()-1;i++){
                value=value+"/"+lista.at(i);
            }

            QString nombre_ext=lista.at(lista.size()-1);
            try {
                QStringList solo_nombre=nombre_ext.split(".");
                QString ruta_raid=value+"/"+solo_nombre.at(0)+"_ra1.disk";
                DeletePartition(tipo_eliminacion,nombre,ruta_raid);
            } catch (...) {
                cout<<"Error: no se encontro el raid"<<nombre_ext.toStdString()<< endl;
            }
        }else{
            cout<<"Hacen falta parametros de name o path en delete"<<endl;
        }
    }else if(is_add){
        if(unidades2>-1){
            if(is_name && is_path){
                AddPartition(unidades,tamanio,nombre,ruta);
                QStringList lista=ruta.split("/");
                QString value="/home";
                for(int i=2;i<lista.size()-1;i++){
                    value=value+"/"+lista.at(i);
                }
                QString nombre_ext=lista.at(lista.size()-1);
                try {
                    QStringList solo_nombre=nombre_ext.split(".");
                    QString ruta_raid=value+"/"+solo_nombre.at(0)+"_ra1.disk";
                    AddPartition(unidades,tamanio,nombre,ruta_raid);
                } catch (...) {
                    cout<<"Error: no se puede agregar"<<nombre_ext.toStdString()<< endl;
                }
            }else{
                cout<<"Hacen falta parametro de name o path en comando add"<<endl;
            }
        }else{
            cout<<"El comando en units es invalido"<<endl;
        }
    }
    else{
        cout<<"Hacen falta parametros obligatorios, name, path o size"<<endl;
    }
}

void commands::CreateDisk(int tamanio, int unidades, char ajuste, string ruta){
    unidades=tamanio*unidades;
    mbr nuevo_disco;

    nuevo_disco.singleSize=unidades;
    nuevo_disco.fit=ajuste;
    nuevo_disco.signature=RandomNumber();
    nuevo_disco.singleDate=time(0);

    for(int i=0;i<4;i++){
        nuevo_disco.part[i].singleSize=0;
        nuevo_disco.part[i].state='0';
        nuevo_disco.part[i].type='0';
        nuevo_disco.part[i].fit='0';
        nuevo_disco.part[i].start=unidades;
    }

    char val[]={'0'};
    FILE *archivo_mkdisk;
    const char *path=ruta.c_str();
    archivo_mkdisk=fopen(path,"wb+");
    fseek(archivo_mkdisk,0,SEEK_SET);
    fwrite(val,sizeof(char),1,archivo_mkdisk);

    fseek(archivo_mkdisk,unidades-1,SEEK_SET);
    fwrite(val,sizeof(char),1,archivo_mkdisk);

    fseek(archivo_mkdisk,0,SEEK_SET);
    fwrite(&nuevo_disco,sizeof(mbr),1,archivo_mkdisk);

    fclose(archivo_mkdisk);
    cout<<"Disco creado con exito"<<endl;
}

void commands::DeleteDisk(string ruta){
    if(FileExist(ruta)){
        remove(ruta.c_str());
        cout<<"Archivo eliminando con exito"<<endl;
    }else{
        cout<<"El archivo no existe"<<ruta<<endl;
    }
}

bool commands::FirstPartition(mbr disco){
    int contador=0;
    for(int i=0;i<4;i++){
        if(disco.part[i].state=='0'){
            contador++;
        }
    }
    if(contador==4){
        return true;
    }
    return false;
}

int commands::CreatePartition(int tamanio, int unidades, char ajuste, char tipo_part, QString ruta, QString nombre){

    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    int size_particion=tamanio*unidades;
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    mbr disco;
    fseek(archivo,0,SEEK_SET);
    fread(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);
    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();
    if(tipo_part=='p' || tipo_part=='P' ||tipo_part=='E' || tipo_part=='e' ){
        if(!SpaceVerification(disco,size_particion)){
            return 0;
        }
    }

    if(!PartitionsVerification(disco,tipo_part)){
        return 0;
    }


    for(int i=0;i<4;i++){
        if(strcasecmp(nom,disco.part[i].name)==0){
            cout<<"Error: ya existe el nombre en una particion"<<endl;
            return 0;
        }
    }

    char ajuste_disco=disco.fit;
    if(tipo_part=='P' || tipo_part=='E'){
        if(ajuste_disco=='B'|| ajuste_disco=='b' ){
             disco=BestSetting(disco,size_particion,ajuste,tipo_part,nom,ruta);
        }else if(ajuste_disco=='F' || ajuste_disco=='f'){
            disco=FirstSetting(disco,size_particion,ajuste,tipo_part,nom,ruta);
        }else if(ajuste_disco=='W' || ajuste_disco=='w'){
            disco=WorstSetting(disco,size_particion,ajuste,tipo_part,nom,ruta);
        }
    }else{
        int pos_extendida=-1;
        for(int i=0;i<4;i++){
            if(disco.part[i].type=='E' || disco.part[i].type=='e'){
                pos_extendida=i;
            }
        }
        if(pos_extendida==-1){
            cout<<"INFO:: No se pueden crear particiones logicas debido a que no existe una particion extendida"<<endl;
            return 0;
        }
        if(SameNamesEXT(ruta,disco.part[pos_extendida].start,nombre)){
            cout<<"INFO:: No se pueden crear a particion "<<nombre.toStdString()<<" debido a que ya existe una con el mismo nombre"<<endl;
            return 0;
        }

        //Aca verifico si existe espacio en la extendida
        if(SpaceEXT(ruta,disco.part[pos_extendida].start,disco.part[pos_extendida].singleSize,size_particion)){
            cout<<"INFO:: No se puede crear la particion "<<nombre.toStdString()<<" por falta de espacio"<<endl;
            return 0;
        }

        if(disco.part[pos_extendida].fit=='B' || disco.part[pos_extendida].fit=='b'){
            BestSettingEXT(size_particion,ajuste,nombre,ruta,disco.part[pos_extendida].start,disco.part[pos_extendida].singleSize);
        }else if(disco.part[pos_extendida].fit=='F'|| disco.part[pos_extendida].fit=='f'){
            WorstSettingEXT(size_particion,ajuste,nombre,ruta,disco.part[pos_extendida].start,disco.part[pos_extendida].singleSize);
        }else{
            FirstSettingEXT(size_particion,ajuste,nombre,ruta,disco.part[pos_extendida].start,disco.part[pos_extendida].singleSize);
        }
    }
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }

    fseek(archivo,0,SEEK_SET);
    fwrite(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);
    return 0;
}

int commands::AddPartition(int unidad, int tamanio, QString nombre, QString ruta){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    mbr disco;
    fseek(archivo,0,SEEK_SET);
    fread(&disco,sizeof(mbr),1,archivo);

    fclose(archivo);
    int tamanio_modif=unidad*tamanio;
    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();
    int posicion=-1;
    for(int i=0;i<4;i++){
        if(strcasecmp(disco.part[i].name,nom)==0){
            posicion=i;
        }
    }
    if(posicion==-1){
        for(int i=0;i<4;i++){
            if(disco.part[i].type=='E' || disco.part[i].type=='e'){
                posicion=i;
            }
        }
        if(posicion!=-1){
            AddLogic(unidad,tamanio,nombre,ruta,disco.part[posicion].start,disco.part[posicion].singleSize);
        }else{
             cout<<"La particion no existe en el disco"<<endl;
        }
    }else{
        if(tamanio_modif>0){
            if(posicion==3){
                int final_particion=disco.part[posicion].start+disco.part[posicion].singleSize;
                int espacio_disponible=disco.singleSize-final_particion;
                if(tamanio_modif<=espacio_disponible){
                    disco.part[posicion].singleSize=disco.part[posicion].singleSize+tamanio_modif;
                    cout<<"Se agrego espacio a la particion"<<endl;
                }else{
                    cout<<"No existe espacio suficiente para agregar"<<endl;
                }
            }else{
                int final_particion=disco.part[posicion].start+disco.part[posicion].singleSize;
                int espacio_disponible=disco.part[posicion+1].start-final_particion;
                if(tamanio_modif<=espacio_disponible){
                    disco.part[posicion].singleSize=disco.part[posicion].singleSize+tamanio_modif;
                    cout<<"Se agrego espacio a la particion"<<endl;
                }else{
                    cout<<"No existe espacio suficiente para agregar"<<endl;
                }
            }
        }else if(tamanio_modif<0){
            tamanio_modif=tamanio_modif*-1;
            if(tamanio_modif<disco.part[posicion].singleSize){
                disco.part[posicion].singleSize=disco.part[posicion].singleSize-tamanio_modif;
                cout<<"Se quito espacio a la particion"<<endl;
            }else{
                cout<<"No se puede quitar espacio debido a que no queda espacio"<<endl;
            }
        }else{
        }
    }
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    fseek(archivo,0,SEEK_SET);
    fwrite(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);
    return 0;
}

int commands::DeletePartition(int tipo_eliminacion, QString nombre, QString ruta){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    mbr disco;
    fseek(archivo,0,SEEK_SET);
    fread(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);
    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    int posicion=-1;
    for(int i=0;i<4;i++){
        if(strcasecmp(disco.part[i].name,nom)==0){
            posicion=i;
        }
    }

    if(posicion==-1){
        for(int i=0;i<4;i++){
            if(disco.part[i].type=='E' || disco.part[i].type=='e'){
                posicion=i;
            }
        }
        if(posicion!=-1){
            DeleteLogic(tipo_eliminacion,nombre,ruta,disco.part[posicion].start,disco.part[posicion].singleSize);
        }else{
            cout<<"INFO:: La particion no existe"<<endl;
            return 0;
        }
    }else{
        if(tipo_eliminacion==1){
            disco.part[posicion].fit='0';
            disco.part[posicion].state='0';
            disco.part[posicion].type='0';
            disco.part[posicion].singleSize=0;
            disco.part[posicion].start=disco.singleSize;
            strcpy(disco.part[posicion].name,"");
        }else{
            int inicio=disco.part[posicion].start;
            int tamanio=disco.part[posicion].singleSize;
            char val[]={'\0'};
            fseek(archivo,inicio,SEEK_SET);
            fwrite(&val,sizeof(char),1,archivo);
            fseek(archivo,tamanio-1,SEEK_SET);
            fwrite(&val,sizeof(char),1,archivo);

            disco.part[posicion].fit='0';
            disco.part[posicion].state='0';
            disco.part[posicion].type='0';
            disco.part[posicion].singleSize=0;
            disco.part[posicion].start=disco.singleSize;
            strcpy(disco.part[posicion].name,"");
        }
        cout<<"Particion Elimina con exito"<<endl;
        disco=PartitionSort(disco);
    }
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    fseek(archivo,0,SEEK_SET);
    fwrite(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);
    return 0;
}

bool commands::PartitionsVerification(mbr disco, char tipo_part){
    int contador_primarias=0;
    int contador_exten=0;
    if(tipo_part=='P'){
        for(int i=0;i<4;i++){
            if(disco.part[i].type=='P'){
                contador_primarias++;
            }
        }
        if(contador_primarias==3){
            cout<<"Error: ya existen 3 particiones primarias,no se pueden crear mas"<<endl;
            return false;
        }else{
            return true;
        }
    }else if(tipo_part=='E'){
        for(int i=0;i<4;i++){
            if(disco.part[i].type=='E'){
                contador_exten++;
            }
        }
        if(contador_exten==1){
            cout<<"Error: ya existe 1 particiones extendida, no se pueden crear mas"<<endl;
            return false;
        }else{
            return true;
        }
    }
    return true;
}

bool commands::SpaceVerification(mbr disco, int espacio){
    int ocupado=0;
    for(int i=0;i<4;i++){
        if(disco.part[i].state=='1' ){
            ocupado=ocupado+disco.part[i].singleSize;
        }
    }
    int libre=disco.singleSize-ocupado;
    if(libre>=espacio){

        return true;
    }
    else{
        cout<<"No se puede crear la particion debido a que no queda espacio disponible"<<endl;
        return false;
    }
}

mbr commands::PartitionSort(mbr disco){
    int i;
    int j;
    singlePartition part_tem;
    for(i=0;i<4;i++){
        for(j=i+1;j<4;j++){
            if(disco.part[j].start<disco.part[i].start){
                part_tem=disco.part[j];
                disco.part[j]=disco.part[i];
                disco.part[i]=disco.part[j];
                disco.part[i]=part_tem;
            }
        }
    }
    return disco;
}

int commands::AddLogic(int unidad, int tamanio, QString nombre, QString ruta, int inicio_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem;
    fseek(archivo,inicio_ext,SEEK_SET);
    fread(&tem,sizeof(ebr),1,archivo);
    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();
    int agrega=unidad*tamanio;
    int pos_siguiente=0;
    int disponible=0;
    bool encontre=0;
    while(pos_siguiente!=-1){
        if(tem.status=='1'){
            if(strcasecmp(tem.name,nom)==0){
                pos_siguiente=tem.next;
                if(pos_siguiente!=-1){
                    ebr siguiente;
                    fseek(archivo,pos_siguiente,SEEK_SET);
                    fread(&siguiente,sizeof(ebr),1,archivo);
                    disponible=siguiente.start-(tem.start+tem.singleSize);
                    pos_siguiente=-1;
                }else{
                    disponible=(inicio_ext+size_ext)-(tem.start+tem.singleSize);
                }
                encontre=true;
                break;
            }
        }
        pos_siguiente=tem.next;
        if(pos_siguiente!=-1){
            fseek(archivo,pos_siguiente,SEEK_SET);
            fread(&tem,sizeof(ebr),1,archivo);
        }
    }
   if(encontre==true){
       if(agrega>0){
           if(agrega<disponible){
               tem.singleSize=tem.singleSize+agrega;
               fseek(archivo,tem.start,SEEK_SET);
               fwrite(&tem,sizeof(ebr),1,archivo);
               cout<<"Espacio agrega con exito"<<endl;
           }else{
               cout<<"No se puede agregar espacio debido a que no es suficiente"<<endl;
               return 0;
           }
       }else if(agrega<0){
           agrega=agrega*-1;
           if(agrega<tem.singleSize){
               tem.singleSize=tem.singleSize-agrega;
               fseek(archivo,tem.start,SEEK_SET);
               fwrite(&tem,sizeof(ebr),1,archivo);
               cout<<"Espacio quitado con exito"<<endl;
           }else{
               cout<<"No se puede quitar el espacio debido a que la particion queda con espacio negativo"<<endl;
               return 0;
           }
       }else{
           return 0;
       }

   }else{
       cout<<"La particion especificada para agregar o quitar espacio no existe en el disco"<<endl;
       return 0;
   }
   archivo=fopen(ruta.toStdString().c_str(),"rb+");
   if(archivo==NULL){
       cout<<"Error: no existe el disco especificado"<<endl;
       return 0;
   }
   fseek(archivo,tem.start,SEEK_SET);
   fwrite(&tem,sizeof(ebr),1,archivo);
   fclose(archivo);
   return 0;

}

int commands::DeleteLogic(int tipo_eliminacion, QString nombre_part, QString ruta, int inicio_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem;
    ebr tem_anterior;
    fseek(archivo,inicio_ext,SEEK_SET);
    fread(&tem,sizeof(ebr),1,archivo);

    if(tem.status=='0' && tem.singleSize==-1){
        cout<<"No existe la particion con el nombre "<<nombre_part.toStdString()<<endl;
        return 0;
    }

    int pos_siguiente=0;
    bool existe_particion=false;
    int next_siguiente=0;

    QByteArray array = nombre_part.toLocal8Bit();
    char* nom = array.data();
    int contador=0;
    while(pos_siguiente!=-1){
        if(tem.status=='1'){
            if(strcasecmp(tem.name,nom)==0){
                if(contador==0){
                    tem.status='0';
                    tem.fit='0';
                    strcpy(tem.name,"");
                    tem.singleSize=0;
                    tem.start=inicio_ext;
                    fseek(archivo,inicio_ext,SEEK_SET);
                    fwrite(&tem,sizeof(ebr),1,archivo);
                    cout<<"Se elimino la particion correctamente"<<endl;
                }else{
                    next_siguiente=tem.next;
                    pos_siguiente=tem.next;
                    if(next_siguiente==-1){
                        tem_anterior.next=next_siguiente;
                    }else{
                        tem_anterior.next=next_siguiente;
                        pos_siguiente=-1;
                    }
                    fseek(archivo,tem_anterior.start,SEEK_SET);
                    fwrite(&tem_anterior,sizeof(ebr),1,archivo);
                    cout<<"Se Elimino la particion corrrectamente"<<endl;
                }
                existe_particion=true;
            }else{
                tem_anterior=tem;
                pos_siguiente=tem.next;
                if(pos_siguiente!=-1){
                    fseek(archivo,pos_siguiente,SEEK_SET);
                    fread(&tem,sizeof(ebr),1,archivo);
                }
            }
            contador++;
        }else{
            tem_anterior=tem;
            fseek(archivo,tem.next,SEEK_SET);
            fread(&tem,sizeof(ebr),1,archivo);
            contador++;
        }
    }
    fclose(archivo);
    if(!existe_particion){
        cout<<"La particion no se encuentra en el disco"<<endl;
        return 0;
    }
    return 0;
}

bool commands::SameNamesEXT(QString ruta, int pos_inicial, QString nombre_part){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return true;
    }
    ebr tem_ebr;
    fseek(archivo,pos_inicial,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);

    QByteArray array = nombre_part.toLocal8Bit();
    char* nom = array.data();
    int pos_siguiente=0;
    while(pos_siguiente!=-1){
        if(strcasecmp(tem_ebr.name,nom)==0){
            fclose(archivo);
            return true;
        }
        pos_siguiente=tem_ebr.next;
        if(pos_siguiente!=-1){

            fseek(archivo,pos_siguiente,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }else{
            fclose(archivo);
            return false;
        }
    }
    fclose(archivo);
    return false;
}

bool commands::SpaceEXT(QString ruta, int pos_inicial, int size_part,int size_logica){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return false;
    }
    ebr tem_ebr;
    fseek(archivo,pos_inicial,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);

    int pos_siguiente=0;
    int ocupado=0;
    while(pos_siguiente!=-1){
        ocupado=ocupado+tem_ebr.singleSize;
        pos_siguiente=tem_ebr.next;
        if(pos_siguiente!=-1){
            fseek(archivo,pos_siguiente,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }
    }
    int libre=size_part-ocupado;
    if(libre>=size_logica){
        fclose(archivo);
        return false;
    }
    fclose(archivo);
    return true;
}

int commands::CreateInitialEBR(QString ruta, int pos_inicial){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr inicial_ebr;
    inicial_ebr.fit='0';
    strcpy(inicial_ebr.name,"");
    inicial_ebr.next=-1;
    inicial_ebr.singleSize=0;
    inicial_ebr.start=pos_inicial;
    inicial_ebr.status='0';
    cout<<"INFO:: La particion extendida fue creada con exito"<<endl;
    fseek(archivo,pos_inicial,SEEK_SET);
    fwrite(&inicial_ebr,sizeof(ebr),1,archivo);
    fclose(archivo);
    return 0;
}

mbr commands::FirstSetting(mbr disco, int size_particion, char ajuste_part, char tipo_part, char *nombre,QString ruta){
    if(FirstPartition(disco)){
        disco.part[0].fit=ajuste_part;
        disco.part[0].singleSize=size_particion;
        disco.part[0].start=sizeof(mbr)+1;
        disco.part[0].state='1';
        disco.part[0].type=tipo_part;
        strcpy(disco.part[0].name,nombre);

        if(tipo_part=='E' || tipo_part=='e'){
            CreateInitialEBR(ruta,sizeof(mbr)+1);
        }else{
            cout<<"INFO:: se ha creado la particion primaria con exito"<<endl;
        }
        return disco;
    }else{
        int inicio1=sizeof(mbr)+1;
        int inicio2=0;
        int espacio_disponible=0;
        int posicion_disponible=0;
        for(int i=0;i<4;i++){
            if(disco.part[i].state=='1'){
                inicio2=disco.part[i].start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    for(int j=i;j<4;j++){
                        if(disco.part[j].state=='0'){
                            posicion_disponible=j;
                            break;
                        }
                    }
                    break;
                }else{
                    inicio1=disco.part[i].start+disco.part[i].singleSize;
                }
            }else if(disco.part[i].state=='0'){
                inicio2=disco.part[i].start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    posicion_disponible=i;
                    break;
                }else{
                    cout<<"No se puede crear la particion por falta de tamanio"<<endl;
                    return disco;
                }
            }
        }
        disco.part[posicion_disponible].fit=ajuste_part;
        disco.part[posicion_disponible].state='1';
        disco.part[posicion_disponible].type=tipo_part;
        strcpy(disco.part[posicion_disponible].name,nombre);
        disco.part[posicion_disponible].start=inicio1;
        disco.part[posicion_disponible].singleSize=size_particion;
        if(tipo_part=='E' || tipo_part=='e'){
            CreateInitialEBR(ruta,inicio1);
        }
        cout<<"Particion creada con exito"<<endl;
        disco=PartitionSort(disco);
        return disco;
    }
}

mbr commands::BestSetting(mbr disco, int size_particion, char ajuste_part, char tipo_part, char *nombre, QString ruta){
    int espacio_anterior=0;
    int inicio=0;
    bool encontro_espacio=false;
    if(FirstPartition(disco)){
        disco.part[0].fit=ajuste_part;
        disco.part[0].singleSize=size_particion;
        disco.part[0].start=sizeof(mbr)+1;
        disco.part[0].state='1';
        disco.part[0].type=tipo_part;
        strcpy(disco.part[0].name,nombre);
        if(tipo_part=='E' || tipo_part=='e'){
            CreateInitialEBR(ruta,sizeof(mbr)+1);
        }
        return disco;
    }else{
        int inicio1=sizeof(mbr)+1;
        int inicio2=0;
        int espacio_disponible=0;
        int posicion_disponible=0;
        for(int i=0;i<4;i++){
            if(disco.part[i].state=='1'){
                inicio2=disco.part[i].start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        inicio=inicio1;
                        encontro_espacio=true;
                    }
                    else{
                        if(espacio_anterior<=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            espacio_anterior=espacio_disponible;
                            inicio=inicio1;
                            encontro_espacio=true;
                        }
                    }
                }
                inicio1=disco.part[i].start+disco.part[i].singleSize;
            }else if(disco.part[i].state=='0'){
                inicio2=disco.part[i].start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        inicio=inicio1;
                        encontro_espacio=true;
                    }else{
                        if(espacio_anterior<=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            espacio_anterior=espacio_disponible;
                            inicio=inicio1;
                            encontro_espacio=true;
                        }
                    }
                }else{
                }
                posicion_disponible=i;
                break;
            }
        }
        if(!encontro_espacio){
            cout<<"No se puede crear la particion debido a que no se encontro un espacio disponible"<<endl;
            return disco;
        }

        disco.part[posicion_disponible].fit=ajuste_part;
        disco.part[posicion_disponible].state='1';
        disco.part[posicion_disponible].type=tipo_part;
        strcpy(disco.part[posicion_disponible].name,nombre);
        disco.part[posicion_disponible].start=inicio;
        disco.part[posicion_disponible].singleSize=size_particion;
        if(tipo_part=='E' || tipo_part=='e'){
            CreateInitialEBR(ruta,inicio);
        }
        cout<<"Particion creada con exito"<<endl;
        disco=PartitionSort(disco);
        return disco;
    }
}

mbr commands::WorstSetting(mbr disco, int size_particion, char ajuste_parte, char tipo_part, char *nombre, QString ruta){
    int espacio_anterior=0;
    int inicio=0;
    bool encontro_espacio=false;
    if(FirstPartition(disco)){
        disco.part[0].fit=ajuste_parte;
        disco.part[0].singleSize=size_particion;
        disco.part[0].start=sizeof(mbr)+1;
        disco.part[0].state='1';
        disco.part[0].type=tipo_part;
        strcpy(disco.part[0].name,nombre);
        if(tipo_part=='E' || tipo_part=='e'){
            CreateInitialEBR(ruta,sizeof(mbr)+1);
        }
        return disco;
    }else{
        int inicio1=sizeof(mbr)+1;
        int inicio2=0;
        int espacio_disponible=0;
        int posicion_disponible=0;
        for(int i=0;i<4;i++){
            if(disco.part[i].state=='1'){
                inicio2=disco.part[i].start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        inicio=inicio1;
                        encontro_espacio=true;
                    }else{
                        if(espacio_anterior>=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            espacio_anterior=espacio_disponible;
                            inicio=inicio1;
                            encontro_espacio=true;
                        }
                    }
                }
                 inicio1=disco.part[i].start+disco.part[i].singleSize;
            }else if(disco.part[i].state=='0'){
                inicio2=disco.part[i].start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        inicio=inicio1;
                        encontro_espacio=true;
                    }else{
                        if(espacio_anterior>=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            espacio_anterior=espacio_disponible;
                            inicio=inicio1;
                            encontro_espacio=true;
                        }
                    }
                }else{
                }
                posicion_disponible=i;
                break;
            }
        }
        if(!encontro_espacio){
            cout<<"No se puede crear la particion debido a que no se encontro un espacio disponible"<<endl;
            return disco;
        }

        disco.part[posicion_disponible].fit=ajuste_parte;
        disco.part[posicion_disponible].state='1';
        disco.part[posicion_disponible].type=tipo_part;
        strcpy(disco.part[posicion_disponible].name,nombre);
        disco.part[posicion_disponible].start=inicio;
        disco.part[posicion_disponible].singleSize=size_particion;
        if(tipo_part=='E' || tipo_part=='e'){
            CreateInitialEBR(ruta,inicio);
        }
        cout<<"Particion creada con exito"<<endl;
        disco=PartitionSort(disco);
        return disco;
    }
}

int commands::FirstSettingEXT(int size_part, char ajuste_part, QString nombre_part, QString ruta, int inicial_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem_ebr;
    ebr tem_ebr_anterior;
    ebr nuevo;
    fseek(archivo,inicial_ext,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);

    QByteArray array = nombre_part.toLocal8Bit();
    char* nom = array.data();

    if(tem_ebr.status=='0' && tem_ebr.next==-1){
        tem_ebr.fit=ajuste_part;
        strcpy(tem_ebr.name,nom);
        tem_ebr.next=-1;
        tem_ebr.singleSize=size_part;
        tem_ebr.start=inicial_ext;
        tem_ebr.status='1';
        fseek(archivo,inicial_ext,SEEK_SET);
        fwrite(&tem_ebr,sizeof(ebr),1,archivo);
        fclose(archivo);
        return 0;
    }

    int inicio1=inicial_ext;
    int inicio2=0;
    int pos_siguiente=0;
    int espacio_disponible=0;
    bool encontro_espacio=false;
    int contador=0;
    int caso=0;
    while(pos_siguiente!=-1){
        if(tem_ebr.status=='1'){
           inicio2=tem_ebr.start;
           espacio_disponible=inicio2-inicio1;
           if(espacio_disponible>=size_part){
               caso=1;
               nuevo.fit=ajuste_part;
               strcpy(nuevo.name,nom);
               nuevo.next=inicio2;
               nuevo.singleSize=size_part;
               nuevo.start=inicio1;
               nuevo.status='1';
               if(contador==0){
               }else{
                    tem_ebr_anterior.next=nuevo.start;
               }
               encontro_espacio=true;
           }
           contador++;
           inicio1=tem_ebr.start+tem_ebr.singleSize;
           pos_siguiente=tem_ebr.next;
           if(pos_siguiente!=-1){
               tem_ebr_anterior=tem_ebr;
               fseek(archivo,pos_siguiente,SEEK_SET);
               fread(&tem_ebr,sizeof(ebr),1,archivo);
            }else{
               espacio_disponible=inicio1-(size_ext+inicial_ext);
               if(espacio_disponible>=size_part){
                   caso=2;
                   nuevo.fit=ajuste_part;
                   strcpy(nuevo.name,nom);
                   nuevo.next=-1;
                   nuevo.singleSize=size_part;
                   nuevo.start=inicio1;
                   nuevo.status='1';
                   tem_ebr.next=nuevo.start;
                   encontro_espacio=true;
                   }
               }
        }else{
            fseek(archivo,tem_ebr.next,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }
    }
    if(encontro_espacio){
        if(caso==1){
            if(contador<=0){
                ebr tem1;
                fseek(archivo,inicial_ext,SEEK_SET);
                fread(&tem1,sizeof(ebr),1,archivo);
                tem1.fit=nuevo.fit;
                strcpy(tem1.name,nuevo.name);
                tem1.next=nuevo.next;
                tem1.singleSize=nuevo.singleSize;
                tem1.start=nuevo.start;
                tem1.status=nuevo.status;
                fseek(archivo,inicial_ext,SEEK_SET);
                fwrite(&tem1,sizeof(ebr),1,archivo);
            }else{
                fseek(archivo,tem_ebr_anterior.start,SEEK_SET);
                fwrite(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                fseek(archivo,nuevo.start,SEEK_SET);
                fwrite(&nuevo,sizeof(ebr),1,archivo);
                fseek(archivo,tem_ebr.start,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }
        }else if(caso==2){
            fseek(archivo,tem_ebr.start,SEEK_SET);
            fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            fseek(archivo,nuevo.start,SEEK_SET);
            fwrite(&nuevo,sizeof(ebr),1,archivo);
        }
        cout<<"Particion Logica creada con exito"<<endl;
    }else{
        cout<<"INFO:: la particion no fue creado por falta de espacio"<<endl;
    }
    fclose(archivo);
    return 0;
}

int commands::BestSettingEXT(int size_part, char ajuste_part, QString nombre, QString ruta, int inicial_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem_ebr;
    ebr tem_ebr_anterior;
    fseek(archivo,inicial_ext,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);
    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    if(tem_ebr.status=='0' && tem_ebr.next==-1){
        tem_ebr.fit=ajuste_part;
        strcpy(tem_ebr.name,nom);
        tem_ebr.next=-1;
        tem_ebr.singleSize=size_part;
        tem_ebr.start=inicial_ext;
        tem_ebr.status='1';
        fseek(archivo,inicial_ext,SEEK_SET);
        fwrite(&tem_ebr,sizeof(ebr),1,archivo);
        fclose(archivo);
        return 0;
    }

    int inicio1=inicial_ext;
    int inicio2=0;
    int nuevo_inicio=0;
    int nuevo_siguiente=0;
    int inicio_anterior=0;
    int espacio_anterior=0;
    int pos_siguiente=0;
    int espacio_disponible=0;
    int contador=0;
    int caso=0;
    bool encontro_espacio=false;
    while(pos_siguiente!=-1){
        if(tem_ebr.status=='1'){
            inicio2=tem_ebr.start;
            espacio_disponible=inicio2-inicio1;
            if(espacio_disponible>=size_part){
                if(espacio_anterior==0){
                    espacio_anterior=espacio_disponible;
                    caso=1;
                    nuevo_inicio=inicio1;
                    nuevo_siguiente=inicio2;
                    encontro_espacio=true;
                    if(contador==0){
                        inicio_anterior=inicio1;
                    }else {
                        inicio_anterior=tem_ebr_anterior.start;
                    }
                }else{
                    if(espacio_anterior<=espacio_disponible){
                        encontro_espacio=true;
                    }else{
                        espacio_anterior=espacio_disponible;
                        caso=1;
                        nuevo_inicio=inicio1;
                        nuevo_siguiente=inicio2;
                        if(contador==0){
                            inicio_anterior=inicio1;
                        }else {
                            inicio_anterior=tem_ebr_anterior.start;
                        }
                        encontro_espacio=true;
                    }
                }
            }
            contador++;
            inicio1=tem_ebr.start+tem_ebr.singleSize;
            pos_siguiente=tem_ebr.next;
            if(pos_siguiente!=-1){
                tem_ebr_anterior=tem_ebr;
                fseek(archivo,pos_siguiente,SEEK_SET);
                fread(&tem_ebr,sizeof(ebr),1,archivo);
                }else{
                    int pos_final=size_ext+inicial_ext;
                    espacio_disponible=pos_final-inicio1;
                    if(espacio_disponible>=size_part){
                        if(espacio_anterior==0){
                            caso=2;
                            espacio_anterior=espacio_disponible;
                            nuevo_inicio=inicio1;
                            nuevo_siguiente=-1;
                            inicio_anterior=tem_ebr.start;
                            encontro_espacio=true;
                        }else{
                            if(espacio_anterior<=espacio_disponible){
                                encontro_espacio=true;
                            }else{
                                encontro_espacio=true;
                                caso=2;
                                espacio_anterior=espacio_disponible;
                                nuevo_inicio=inicio1;
                                nuevo_siguiente=-1;
                               inicio_anterior=tem_ebr.start;
                                encontro_espacio=true;
                            }
                        }
                    }
                }

            \
        }else{
            fseek(archivo,tem_ebr.next,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }
    }
    if(encontro_espacio){
        if(caso==1){
            if(contador<=1){
                fseek(archivo,inicial_ext,SEEK_SET);
                fread(&tem_ebr,sizeof(ebr),1,archivo);//el primero
                tem_ebr.fit=ajuste_part;
                strcpy(tem_ebr.name,nom);
                tem_ebr.next=nuevo_siguiente;
                tem_ebr.singleSize=size_part;
                tem_ebr.start=nuevo_inicio;
                tem_ebr.status='1';
                fseek(archivo,nuevo_inicio,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }else{
                fseek(archivo,inicio_anterior,SEEK_SET);
                fread(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                tem_ebr_anterior.next=nuevo_inicio;
                fseek(archivo,inicio_anterior,SEEK_SET);
                fwrite(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                ebr nuevo;
                nuevo.fit=ajuste_part;
                strcpy(nuevo.name,nom);
                nuevo.next=nuevo_siguiente;
                nuevo.singleSize=size_part;
                nuevo.start=nuevo_inicio;
                nuevo.status='1';
                fseek(archivo,nuevo_inicio,SEEK_SET);
                fwrite(&nuevo,sizeof(ebr),1,archivo);
                fseek(archivo,nuevo_siguiente,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }
        }else if(caso==2){
            ebr nuevo;
            nuevo.fit=ajuste_part;
            strcpy(nuevo.name,nom);
            nuevo.next=-1;
            nuevo.singleSize=size_part;
            nuevo.start=nuevo_inicio;
            nuevo.status='1';
            tem_ebr.next=nuevo_inicio;
            fseek(archivo,tem_ebr.start,SEEK_SET);
            fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            fseek(archivo,nuevo_inicio,SEEK_SET);
            fwrite(&nuevo,sizeof(ebr),1,archivo);
        }
        cout<<"Particion Logica creada con exito"<<endl;
    }else{
        cout<<"INFO:: No se encontro espacio para la particion"<<endl;

    }
    fclose(archivo);
    return 0;
}

int commands::WorstSettingEXT(int size_part, char ajuste_part, QString nombre, QString ruta, int inicial_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem_ebr;
    ebr tem_ebr_anterior;
    fseek(archivo,inicial_ext,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);
    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    if(tem_ebr.status=='0' && tem_ebr.next==-1){
        tem_ebr.fit=ajuste_part;
        strcpy(tem_ebr.name,nom);
        tem_ebr.next=-1;
        tem_ebr.singleSize=size_part;
        tem_ebr.start=inicial_ext;
        tem_ebr.status='1';
        fseek(archivo,inicial_ext,SEEK_SET);
        fwrite(&tem_ebr,sizeof(ebr),1,archivo);
        fclose(archivo);
        return 0;
    }

    int inicio1=inicial_ext;
    int inicio2=0;
    int nuevo_inicio=0;
    int nuevo_siguiente=0;
    int inicio_anterior=0;
    int espacio_anterior=0;
    int pos_siguiente=0;
    int espacio_disponible=0;
    int contador=0;
    int caso=0;
    bool encontro_espacio=false;
    while(pos_siguiente!=-1){
        if(tem_ebr.status=='1'){
            inicio2=tem_ebr.start;
            espacio_disponible=inicio2-inicio1;
            if(espacio_disponible>=size_part){
                if(espacio_anterior==0){
                    espacio_anterior=espacio_disponible;
                    caso=1;
                    nuevo_inicio=inicio1;
                    nuevo_siguiente=inicio2;
                    encontro_espacio=true;
                    if(contador==0){
                        inicio_anterior=inicio1;
                    }else {
                        inicio_anterior=tem_ebr_anterior.start;
                    }
                }else{
                    if(espacio_anterior>=espacio_disponible){
                        encontro_espacio=true;
                    }else{
                        espacio_anterior=espacio_disponible;
                        caso=1;
                        nuevo_inicio=inicio1;
                        nuevo_siguiente=inicio2;
                        if(contador==0){
                            inicio_anterior=inicio1;
                        }else {
                            inicio_anterior=tem_ebr_anterior.start;
                        }
                        encontro_espacio=true;
                    }
                }
            }
            contador++;
            inicio1=tem_ebr.start+tem_ebr.singleSize;
            pos_siguiente=tem_ebr.next;
            if(pos_siguiente!=-1){
                tem_ebr_anterior=tem_ebr;
                fseek(archivo,pos_siguiente,SEEK_SET);
                fread(&tem_ebr,sizeof(ebr),1,archivo);
            }else{
                int pos_final=size_ext+inicial_ext;
                espacio_disponible=pos_final-inicio1;
                if(espacio_disponible>=size_part){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        nuevo_inicio=inicio1;
                        nuevo_siguiente=-1;
                        encontro_espacio=true;
                        caso=2;
                    }else{
                        if(espacio_anterior>=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            encontro_espacio=true;
                            espacio_anterior=espacio_disponible;
                            nuevo_inicio=inicio1;
                            nuevo_siguiente=-1;
                            inicio_anterior=tem_ebr.start;
                            caso=2;
                            }
                        }
                    }
                }
        }else{
            fseek(archivo,tem_ebr.next,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }
    }
    if(encontro_espacio){
        if(caso==1){
            if(contador<=1){
                fseek(archivo,inicial_ext,SEEK_SET);
                fread(&tem_ebr,sizeof(ebr),1,archivo);
                tem_ebr_anterior.fit=ajuste_part;
                strcpy(tem_ebr.name,nom);
                tem_ebr.next=nuevo_siguiente;
                tem_ebr.singleSize=size_part;
                tem_ebr.start=nuevo_inicio;
                tem_ebr.status='1';
                fseek(archivo,nuevo_inicio,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }else{
                fseek(archivo,inicio_anterior,SEEK_SET);
                fread(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                tem_ebr_anterior.next=nuevo_inicio;
                fseek(archivo,inicio_anterior,SEEK_SET);
                fwrite(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                ebr nuevo;
                nuevo.fit=ajuste_part;
                strcpy(nuevo.name,nom);
                nuevo.next=nuevo_siguiente;
                nuevo.singleSize=size_part;
                nuevo.start=nuevo_inicio;
                nuevo.status='1';
                fseek(archivo,nuevo_inicio,SEEK_SET);
                fwrite(&nuevo,sizeof(ebr),1,archivo);
                fseek(archivo,nuevo_siguiente,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }
        }else if(caso==2){
            ebr nuevo;
            nuevo.fit=ajuste_part;
            strcpy(nuevo.name,nom);
            nuevo.next=-1;
            nuevo.singleSize=size_part;
            nuevo.start=nuevo_inicio;
            nuevo.status='1';
            tem_ebr.next=nuevo_inicio;
            fseek(archivo,tem_ebr.start,SEEK_SET);
            fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            fseek(archivo,nuevo_inicio,SEEK_SET);
            fwrite(&nuevo,sizeof(ebr),1,archivo);
        }
        cout<<"Particion Logica creada con exito"<<endl;
    }else{
        cout<<"INFO:: No se encontro espacio para la particion"<<endl;

    }
    fclose(archivo);
    return 0;
}

void commands::ExecuteMount(node *montaje){
    bool is_error=false;
    bool is_path=false;
    bool is_name=false;
    QString ruta="";
    QString name_part="";
    for(int i=0;i<montaje->childs.size();i++){
        node tem=montaje->childs.at(i);
        int tipo_=tem.tipo_;
        switch (tipo_) {
            case path:
                ruta=QuotesClean(tem.valor);
                is_path=true;
                break;
            case name:
                name_part=tem.valor;
                is_name=true;
                break;
            default:
                cout<<"Error:Comando invalido en mount"<<endl;
                is_error=true;
        }
    }
    if(is_name && is_path){
        DoMount(ruta,name_part);
    }else if(is_error){
        //Ya están impresos los errores
    }
    else{
        cout<<"Error: hacen falta parametros obligatorios en mount"<<endl;
    }
}

int commands::DoMount(QString ruta, QString nombre){
    QStringList lista=ruta.split("/");
    int total_lista=lista.size();
    QString nombre_disco=lista.at(total_lista-1);
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco"<<endl;
        return 0;
    }
    fseek(archivo,0,SEEK_SET);
    mbr disco;
    int part_exte=-1;

    fread(&disco,sizeof(mbr),1,archivo);
    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();
    bool encontro=false;
    for(int i=0;i<4;i++){
        if(disco.part[i].state=='1'){
            if(strcasecmp(disco.part[i].name,nom)==0){
                encontro=true;
            }
            if(disco.part[i].type=='E' || disco.part[i].type=='e'){
                part_exte=i;
            }
        }
    }
    if(encontro==false && part_exte!=-1){
        //Buscar el nombre
    }
    if(encontro==false){
        cout<<"Error: no se encontro la particion especifica en el disco"<<endl;
        return 0;
    }

    for(int i=0;i<50;i++){
        if(repoMount[i].state==1){
            if(QString::compare(repoMount[i].diskName,nombre_disco)==0 && QString::compare(repoMount[i].partName,nombre)==0){
                cout<<"Alerta:: La particion ya fue montada"<<endl;
                fclose(archivo);
                return 0;
            }
        }
    }
    int valor;
    int posicion=-1;
    int contador=0;
    bool disco_usado=false;
    for(int i=0;i<50;i++){
        if(repoMount[i].state==1){
            if(QString::compare(repoMount[i].diskName,nombre_disco)==0){
                disco_usado=true;//se esta usando el disco
                contador++;
                valor=repoMount[i].letterNumber;
            }
        }
    }

    if(disco_usado==true){
        for(int i=0;i<50;i++){
            if(repoMount[i].state==0){
                posicion=i;
            }
        }

        int numero_monta=contador+1;
        repoMount[posicion].diskPath=ruta;
        repoMount[posicion].state=1;
        //repoMount[posicion].id_mont=vd+QString::number(numero_monta);
        repoMount[posicion].diskName=nombre_disco;
        repoMount[posicion].partName=nombre;
        repoMount[posicion].number=numero_monta;
        repoMount[posicion].letterNumber=valor;
        cout<<"Particion montada con exito"<<endl;
    }else{
        QString vd="";
        int disponible=-1;
        for(int i=0;i<50;i++){
            if(repoMount[i].state==0){
                disponible=i;
            }
        }
        singleVar++;
        valor=singleVar;
        //vd=Return_ID_reporte(valor)+"1";
        repoMount[disponible].diskPath=ruta;
        repoMount[disponible].partName=nombre;
        repoMount[disponible].diskName=nombre_disco;
        repoMount[disponible].letterNumber=valor;
        repoMount[disponible].state=1;
        repoMount[disponible].MountID=vd;
        repoMount[disponible].number=1;
        cout<<"Particion Montada con exito"<<endl;
    }
    fclose(archivo);
    return 0;
}

void commands::ExecuteDismount(node *desmontaje){
    bool is_error=false;
    bool is_name=false;
    QString id_part="";
    for(int i=0;i<desmontaje->childs.size();i++){
        node tem=desmontaje->childs.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case id:
            id_part=tem.valor;
            is_name=true;
            break;
        default:
            is_error=true;
            cout<<"Error: Parametros no validos en la instruccion unmount"<<endl;
            break;
        }
    }

    if(is_name && !is_error){
        DoDismount(id_part);
    }else if(!is_name){
        cout<<"Error: No se encontro el parametro id en la instruccion unmount"<<endl;
    }

}

int commands::DoDismount(QString id_part){
    cout<<"INFO:: id particion a desmontar"<<id_part.toStdString()<<endl;
    for(int i=0;i<50;i++){
        if(repoMount[i].state==1){
            if(QString::compare(repoMount[i].MountID,id_part,Qt::CaseInsensitive)==0){
                repoMount[i].state=0;
                repoMount[i].diskPath="";
                repoMount[i].MountID="";
                repoMount[i].diskName="";
                repoMount[i].partName="";
                repoMount[i].number=-1;
                repoMount[i].letterNumber=-1;
                cout<<"INFO:: la particion se ha desmontado"<<endl;
                return 0;
            }
        }
    }
    cout<<"INFO:: La particion no existe montada"<<endl;
    return 0;
}

