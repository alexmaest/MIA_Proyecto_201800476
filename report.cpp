#include "report.h"

Report::Report()
{
    temporal_mount.estado='0';
    temporal_part.part_status='0';
    this->contenidos_archivos="";
    this->exclusivo_archivos=-1;
}

int Report::Reporte_DISK(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){return 0;}
    QString ruta_disco=montar_repo[posicion].disco_ruta;
    QString nombre_disco=montar_repo[posicion].nombre_disco;

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    mbr disco;
    ebr tem;

    fseek(archivo,0,SEEK_SET);
    fread(&disco,sizeof(mbr),1,archivo);

    QStringList listaR=value.split(".");
    QString nombre_fichero="";
    QString extension="";

    if(listaR.size()==2){
        nombre_fichero=listaR.at(0)+".dot";
        extension=listaR.at(1);
    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;
    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }
    float tamanio_disco=disco.mbr_tamano;
    int inicio=sizeof(mbr)+1;
    float disponible=0;
    float porcentaje=0.0;
    float ocupado=0;

    reporte<<"digraph H {"<<endl;
    reporte<<endl;
    reporte<<"tabla[shape=plaintext color=black"<<endl;
    reporte<<"label =<"<<endl;
    reporte<<"<table border='0' cellborder='1' cellspacing='0'>"<<endl;
    reporte<<"<tr><td>"<<nombre_disco.toStdString()<<"</td></tr>"<<endl;
    reporte<<"<tr>"<<endl;
    reporte<<"<td>MBR</td>"<<endl;
    for(int i=0;i<4;i++){
        if(disco.part[i].part_status=='1'){
            if(disco.part[i].part_type=='p' || disco.part[i].part_type=='P'){
                disponible=disco.part[i].part_start-inicio;
                if(disponible>0){
                    porcentaje=(float)((disponible/tamanio_disco)*100);
                    reporte<<"<td>Libre"<<fixed<<setprecision(0)<<porcentaje<<"% del disco</td>"<<endl;
                }
                    ocupado=disco.part[i].part_size;
                    porcentaje=(float)((ocupado/tamanio_disco)*100);
                    reporte<<"<td>Primaria"<<fixed<<setprecision(0)<<porcentaje<<"% del disco</td>"<<endl;

                inicio=disco.part[i].part_start+disco.part[i].part_size;
            }else if(disco.part[i].part_type=='e' || disco.part[i].part_type=='E'){
                disponible=disco.part[i].part_start-inicio;
                if(disponible>0){
                    porcentaje=(float)((disponible/tamanio_disco)*100);
                    reporte<<"<td>Libre"<<fixed<<setprecision(0)<<porcentaje<<"% del disco</td>"<<endl;
                }
                    int pos_siguiente=0;
                    reporte<<"<td>"<<endl;
                    reporte<<"<table border='0' cellborder='1' cellspacing='0'>"<<endl;
                    reporte<<"<tr><td>Extendia</td></tr>"<<endl;
                    reporte<<"<tr>"<<endl;
                    fseek(archivo,disco.part[i].part_start,SEEK_SET);
                    fread(&tem,sizeof(ebr),1,archivo);

                    while(pos_siguiente!=-1){
                        if(tem.part_status=='1'){
                            disponible=tem.part_start-inicio;
                            if(disponible>0){
                                porcentaje=(float)((disponible/tamanio_disco)*100);
                                reporte<<"<td>Libre"<<fixed<<setprecision(0)<<porcentaje<<"% del disco</td>"<<endl;
                            }
                                ocupado=tem.part_size;
                                //porcentaje=1/2;
                                cout<<porcentaje;
                                porcentaje=(float)((ocupado/tamanio_disco)*100);
                                reporte<<"<td>EBR"<<fixed<<setprecision(0)<<porcentaje<<"% del disco</td>"<<endl;

                            inicio=tem.part_start+tem.part_size;
                            pos_siguiente=tem.part_next;
                            if(pos_siguiente!=-1){
                                fseek(archivo,pos_siguiente,SEEK_SET);
                                fread(&tem,sizeof(ebr),1,archivo);
                            }
                        }else{
                            pos_siguiente=tem.part_next;
                            if(pos_siguiente!=-1){
                                fseek(archivo,pos_siguiente,SEEK_SET);
                                fread(&tem,sizeof(ebr),1,archivo);
                            }
                        }
                    }
                    reporte<<"</tr>"<<endl;
                    reporte<<"</table>"<<endl;
                    reporte<<"</td>"<<endl;

            }
        }else{
            disponible=tamanio_disco-inicio;
            if(disponible>0){
                porcentaje=(float)((disponible/tamanio_disco)*100);
                reporte<<"<td>Libre"<<fixed<<setprecision(2)<<porcentaje<<"% del disco</td>"<<endl;
                break;
            }
        }
    }
    fclose(archivo);

    reporte<<"</tr>"<<endl;
    reporte<<"</table>"<<endl;
    reporte<<">];"<<endl;
    reporte<<"}"<<endl; //final_reporte
    reporte.close();
    QString tExtension="";
    if(QString::compare(extension,"pdf",Qt::CaseInsensitive)==0){
        tExtension="-Tpdf";
    }else if(QString::compare(extension,"png",Qt::CaseInsensitive)==0){
        tExtension="-Tpng";
    }else if(QString::compare(extension,"jpg",Qt::CaseInsensitive)==0){
        tExtension="-Tjpg";
    }


    QString comando="dot "+tExtension+" "+nombre_fichero+" -o "+ruta+"/"+value;
    system(comando.toStdString().c_str());

    cout<<"INFO:: El reporte se ha creado con exito"<<endl;
    return 0;
}


int Report::Reporte_MBR(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){return 0;}

    QString ruta_disco=montar_repo[posicion].disco_ruta;
    QString nombre_disco=montar_repo[posicion].nombre_disco;

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    mbr disco;
    fseek(archivo,0,SEEK_SET);
    fread(&disco,sizeof(mbr),1,archivo);

    QStringList listaR=value.split(".");
    QString nombre_fichero="";
    QString extension="";

    if(listaR.size()==2){
        nombre_fichero=listaR.at(0)+".dot";
        extension=listaR.at(1);
    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }
    char fecha[100]="fecha";
    struct tm *timeinfo;
    timeinfo=localtime(&disco.mbr_fecha);
    strftime(fecha,100,"%c",timeinfo);
    //Empiezo a escribir en el archivo
    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;
    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }
    reporte<<"digraph H {"<<endl;
    reporte<<endl;
    reporte<<"tabla[shape=plaintext color=black"<<endl;
    reporte<<"label =<"<<endl;
    reporte<<"<table border='0' cellborder='1' cellspacing='0'>"<<endl;
    reporte<<"<tr><td>"<<nombre_disco.toStdString()<<"</td></tr>"<<endl;
    reporte<<"<tr>"<<endl;
    reporte<<"<td><b>Nombre</b></td>"<<endl;
    reporte<<"<td><b>Valor</b></td>"<<endl;
    reporte<<"</tr>"<<endl;
    reporte<<"<tr>"<<endl;
    reporte<<"<td>mbr_tamanio</td><td>"<<disco.mbr_tamano<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;
    reporte<<"<tr>"<<endl;
    reporte<<"<td>mbr_fecha_creacion</td><td>"<<fecha<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;
    reporte<<"<tr>"<<endl;
    reporte<<"<td>mbr_disk_signatura</td><td>"<<disco.mbr_signatura<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;
    reporte<<"<tr>"<<endl;
    reporte<<"<td>mbr_disk_fit</td><td>"<<disco.disk_fit<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;


    for(int i=0;i<4;i++){
        if(disco.part[i].part_status=='1' && disco.part[i].part_type=='p' || disco.part[i].part_type=='P'){
            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_status_"<<i<<"</td><td>"<<disco.part[i].part_status<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_type_"<<i<<"</td><td>"<<disco.part[i].part_type<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_fit_"<<i<<"</td><td>"<<disco.part[i].part_fit<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_start_"<<i<<"</td><td>"<<disco.part[i].part_start<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_size_"<<i<<"</td><td>"<<disco.part[i].part_size<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_name_"<<i<<"</td><td>"<<disco.part[i].part_name<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;
            reporte<<"<tr><td>-------</td></tr>"<<endl;

            //aca verifico si es extendia entonces hago las logicas
        }else if(disco.part[i].part_status=='1' && disco.part[i].part_type=='e' || disco.part[i].part_type=='E'){
            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_status_"<<i<<"</td><td>"<<disco.part[i].part_status<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_type_"<<i<<"</td><td>"<<disco.part[i].part_type<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_fit_"<<i<<"</td><td>"<<disco.part[i].part_fit<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_start_"<<i<<"</td><td>"<<disco.part[i].part_start<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_size_"<<i<<"</td><td>"<<disco.part[i].part_size<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>part_name_"<<i<<"</td><td>"<<disco.part[i].part_name<<"</td>"<<endl;
            reporte<<"</tr>"<<endl;
            reporte<<"<tr><td>-------</td></tr>"<<endl;
            int contador=1;
            int pos_inicial=disco.part[i].part_start;
            ebr tem;
            fseek(archivo,pos_inicial,SEEK_SET);
            fread(&tem,sizeof(ebr),1,archivo);
            int pos_siguiente=0;
            while(pos_siguiente!=-1){
                if(tem.part_status=='1'){
                    reporte<<"<tr><td>EBR_"<<contador<<"</td></tr>"<<endl;
                    reporte<<"<tr>"<<endl;

                    reporte<<"<td>part_status_"<<contador<<"</td><td>"<<tem.part_status<<"</td>"<<endl;
                    reporte<<"</tr>"<<endl;

                    reporte<<"<tr>"<<endl;
                    reporte<<"<td>part_fit_"<<contador<<"</td><td>"<<tem.part_fit<<"</td>"<<endl;
                    reporte<<"</tr>"<<endl;

                    reporte<<"<tr>"<<endl;
                    reporte<<"<td>part_start_"<<contador<<"</td><td>"<<tem.part_start<<"</td>"<<endl;
                    reporte<<"</tr>"<<endl;

                    reporte<<"<tr>"<<endl;
                    reporte<<"<td>part_size_"<<contador<<"</td><td>"<<tem.part_size<<"</td>"<<endl;
                    reporte<<"</tr>"<<endl;

                    reporte<<"<tr>"<<endl;
                    reporte<<"<td>part_next_"<<contador<<"</td><td>"<<tem.part_next<<"</td>"<<endl;
                    reporte<<"</tr>"<<endl;

                    reporte<<"<tr>"<<endl;
                    reporte<<"<td>part_name_"<<contador<<"</td><td>"<<tem.part_name<<"</td>"<<endl;
                    reporte<<"</tr>"<<endl;
                    reporte<<"<tr><td>-------</td></tr>"<<endl;
                }
                pos_siguiente=tem.part_next;
                if(pos_siguiente!=-1){
                    fseek(archivo,pos_siguiente,SEEK_SET);
                    fread(&tem,sizeof(ebr),1,archivo);
                }
                contador++;
            }

        }
    }
    fclose(archivo);

    reporte<<"</table>"<<endl;
    reporte<<">];"<<endl;
    reporte<<"}"<<endl; //final_reporte
    reporte.close();
    QString tExtension="";
    if(QString::compare(extension,"pdf",Qt::CaseInsensitive)==0){
        tExtension="-Tpdf";
    }else if(QString::compare(extension,"png",Qt::CaseInsensitive)==0){
        tExtension="-Tpng";
    }else if(QString::compare(extension,"jpg",Qt::CaseInsensitive)==0){
        tExtension="-Tjpg";
    }


    QString comando="dot "+tExtension+" "+nombre_fichero+" -o "+ruta+"/"+value;

    system(comando.toStdString().c_str());

    cout<<"INFO:: El reporte se ha creado con exito"<<endl;
    return 0;
}


int Report::Reporte_SB(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }

    QString ruta_disco=temporal_mount.disco_ruta;
    //QString nombre_disco=temporal_mount.nombre_disco;

    QStringList listaR=value.split(".");
    QString nombre_fichero="";
    QString extension="";

    if(listaR.size()==2){
        nombre_fichero=listaR.at(0)+".dot";
        extension=listaR.at(1);
    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    superBloque tem_sb;
    int inicio_particion=temporal_part.part_start;
    fseek(archivo,inicio_particion,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);
    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;
    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }
    char fecha[100]="fecha";
    struct tm *timeinfo;
    //timeinfo=localtime(&disco.mbr_fecha);
    //strftime(fecha,100,"%c",timeinfo);

    reporte<<"digraph H {"<<endl;
    reporte<<"aHtmlTable ["<<endl<<"shape=plaintext"<<endl<<"color=black "<<endl<<"label=<"<<endl;
    reporte<<"<table border='1' cellborder='1' cellspacing='0'>"<<endl;
    reporte<<"<tr>"<<endl<<"<td>Super Bloque</td>"<<endl<<"</tr>"<<endl;
    reporte<<"<tr><td><b>Nombre</b></td>"<<endl<<"<td><b>Valor</b></td>"<<endl<<"</tr>";

    reporte<<"<tr>"<<endl;
    reporte<<"<td>s_file_system</td><td>"<<tem_sb.filesystem_type<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;


    reporte<<"<tr>"<<endl;
    reporte<<"<td>s_inodes_count</td><td>"<<tem_sb.inodes_count<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>s_blocks_count</td><td>"<<tem_sb.bloques_count<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>bloque_libres</td><td>"<<tem_sb.free_bloques_count<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Inodos Libres</td><td>"<<tem_sb.free_inodes_count<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;



    reporte<<"<tr>"<<endl;
    timeinfo=localtime(&tem_sb.mount_time);
    strftime(fecha,100,"%c",timeinfo);
    reporte<<"<td>ultima Fecha de Montaje</td><td>"<<fecha<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    timeinfo=localtime(&tem_sb.unmount_time);
    strftime(fecha,100,"%c",timeinfo);
    reporte<<"<td>ultima Fecha de Desmontaje</td><td>"<<fecha<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>S_magic</td><td>"<<tem_sb.magic<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Tamanio de Inodo</td><td>"<<tem_sb.size_inodo<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Tamanio de Bloque</td><td>"<<tem_sb.size_bloque<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Primer Inodo libre</td><td>"<<tem_sb.primer_inodo_free<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Primer Bloque Libre</td><td>"<<tem_sb.primer_bloque_free<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Inicio Bitmap de Inodo</td><td>"<<tem_sb.bm_start_inodo<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Inicio Bitmap de Bloque</td><td>"<<tem_sb.bm_start_bloque<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Inicio de Inodos</td><td>"<<tem_sb.start_inodo<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"<tr>"<<endl;
    reporte<<"<td>Inicio de Bloques</td><td>"<<tem_sb.start_bloque<<"</td>"<<endl;
    reporte<<"</tr>"<<endl;

    reporte<<"</table>"<<endl;
    reporte<<">]; }";


    reporte.close();
    QString tExtension="";
    if(QString::compare(extension,"pdf",Qt::CaseInsensitive)==0){
        tExtension="-Tpdf";
    }else if(QString::compare(extension,"png",Qt::CaseInsensitive)==0){
        tExtension="-Tpng";
    }else if(QString::compare(extension,"jpg",Qt::CaseInsensitive)==0){
        tExtension="-Tjpg";
    }


    QString comando="dot "+tExtension+" "+nombre_fichero+" -o "+ruta+"/"+value;

    system(comando.toStdString().c_str());

    cout<<"INFO:: El reporte se ha creado con exito"<<endl;
    return 0;


}

int Report::Reporte_BM_Inode(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }

    QString ruta_disco=temporal_mount.disco_ruta;
    //QString nombre_disco=temporal_mount.nombre_disco;

    QStringList listaR=value.split(".");
    QString nombre_fichero="";


    if(listaR.size()==2 || listaR.size()==1){
        nombre_fichero=listaR.at(0)+".txt";

    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }

    superBloque tem_sb;
    int inicio_particion=temporal_part.part_start;
    fseek(archivo,inicio_particion,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);
    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;
    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }

    int numero_inodos=tem_sb.inodes_count;
    int inicio_bm_inodos=tem_sb.bm_start_inodo;

    int contador=0;//este contador es para separarlos de 20 en 20

    fseek(archivo,inicio_bm_inodos,SEEK_SET);
    char pos_bm='0';
    fread(&pos_bm,sizeof(char),1,archivo);

    for(int i=0;i<numero_inodos;i++){
        if(contador<20){
            reporte<<pos_bm<<" ";
            inicio_bm_inodos++;
            fseek(archivo,inicio_bm_inodos,SEEK_SET);
            fread(&pos_bm,sizeof(char),1,archivo);
            contador++;
        }else{
            reporte<<endl;
            reporte<<pos_bm<<" ";
            inicio_bm_inodos++;
            fseek(archivo,inicio_bm_inodos,SEEK_SET);
            fread(&pos_bm,sizeof(char),1,archivo);
            contador=1;
        }

    }
    reporte.close();
    cout<<"Reporte de bitmap de inodes creado con exito de la particion "<<temporal_part.part_name<<endl;
}

int Report::Reporte_BM_Bloque(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }

    QString ruta_disco=temporal_mount.disco_ruta;
    //QString nombre_disco=temporal_mount.nombre_disco;

    QStringList listaR=value.split(".");
    QString nombre_fichero="";


    if(listaR.size()==2 || listaR.size()==1){
        nombre_fichero=listaR.at(0)+".txt";

    }
    else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }

    superBloque tem_sb;
    int inicio_particion=temporal_part.part_start;
    fseek(archivo,inicio_particion,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);
    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;
    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }

    int numero_bloque=tem_sb.bloques_count;
    int inicio_bm_bloque=tem_sb.bm_start_bloque;

    int contador=0;//este contador es para separarlos de 20 en 20

    fseek(archivo,inicio_bm_bloque,SEEK_SET);
    char pos_bm='0';
    fread(&pos_bm,sizeof(char),1,archivo);

    for(int i=0;i<numero_bloque;i++){
        if(contador<20){
            reporte<<pos_bm<<" ";
            inicio_bm_bloque++;
            fseek(archivo,inicio_bm_bloque,SEEK_SET);
            fread(&pos_bm,sizeof(char),1,archivo);

            contador++;
        }else{
            reporte<<endl;
            reporte<<pos_bm<<" ";
            inicio_bm_bloque++;
            fseek(archivo,inicio_bm_bloque,SEEK_SET);
            fread(&pos_bm,sizeof(char),1,archivo);

            contador=1;
        }

    }
    reporte.close();
    cout<<"Reporte del bitmap de bloques creado con exito de la particion "<<temporal_part.part_name<<endl;
}


int Report::Reporte_Inode(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }

    QString ruta_disco=temporal_mount.disco_ruta;
    //QString nombre_disco=temporal_mount.nombre_disco;

    QStringList listaR=value.split(".");
    QString nombre_fichero="";
    QString extension="";

    if(listaR.size()==2){
        nombre_fichero=listaR.at(0)+".dot";
        extension=listaR.at(1);
    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    superBloque tem_sb;
    fseek(archivo,temporal_part.part_start,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);


    if(tem_sb.magic==2017){
        cout<<"Ocurrio un error en la particion realice un recovery si es posible"<<endl;
        fclose(archivo);
        return 0;
    }

    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;
    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }

    reporte<<"digraph R {"<<endl;
    reporte<<"rankdir = LR;"<<endl;
    reporte<<"label=\"Nombre_Particion: "<<temporal_part.part_name<<" Nombre_Disco: "<<temporal_mount.nombre_disco.toStdString()<<"\""<<endl;
    QString inode="inode";

    int posicion_bm_inode=tem_sb.bm_start_inodo;
    int numero_inodes=tem_sb.inodes_count;
    int inicio_inodes=tem_sb.start_inodo;

    char fecha[100]="fecha";
    struct tm *timeinfo;

    inodo temporal_inode;
    char estado='0';
    fseek(archivo,posicion_bm_inode,SEEK_SET);

    for(int i=0;i<numero_inodes;i++){
        fread(&estado,sizeof(char),1,archivo);
        if(estado=='1'){
            int puntero=inicio_inodes+(i*tem_sb.size_inodo);
            fseek(archivo,puntero,SEEK_SET);
            fread(&temporal_inode,sizeof(inodo),1,archivo);
            inode="inode";
            inode=inode+"_"+QString::number(i);
            reporte<<inode.toStdString()<<"[shape=record,color=blue"<<endl;
            reporte<<"label=\""<<"Posicion: "<<inode.toStdString()<<"\\n"<<"i_uid: "<<temporal_inode.usuario_id<<"\\n"<<endl;
            reporte<<"i_guid: "<<temporal_inode.grupo_id<<"\\n"<<endl;
            reporte<<"i_size: "<<temporal_inode.size_file<<"\\n"<<endl;
            timeinfo=localtime(&temporal_inode.ulti_time);
            strftime(fecha,100,"%c",timeinfo);
            reporte<<"i_atime: "<<fecha<<"\\n"<<endl;
            timeinfo=localtime(&temporal_inode.crear_time);
            strftime(fecha,100,"%c",timeinfo);
            reporte<<"i_ctime: "<<fecha<<"\\n"<<endl;
            timeinfo=localtime(&temporal_inode.modif_time);
            strftime(fecha,100,"%c",timeinfo);
            reporte<<"i_mtime: "<<fecha<<"\\n"<<endl;

            for(int i=0;i<15;i++){
                reporte<<"i_block_"<<i<<": "<<temporal_inode.apuntador[i]<<"\\n"<<endl;
            }

            reporte<<"i_type: "<<temporal_inode.type_file<<"\\n"<<endl;
            reporte<<"i_perm: "<<temporal_inode.permiso<<"\"]; "<<endl;


        }
        posicion_bm_inode++;
        fseek(archivo,posicion_bm_inode,SEEK_SET);
    }

    inode="inode";

    estado='0';
    QString anterior="";

    posicion_bm_inode=tem_sb.bm_start_inodo;

    fseek(archivo,posicion_bm_inode,SEEK_SET);
    for(int i=0;i<numero_inodes;i++){
       fread(&estado,sizeof(char),1,archivo);
       if(estado=='1'){
           inode="inode";
           inode=inode+"_"+QString::number(i);

           if(QString::compare(anterior,"")==0){
               anterior=inode;
           }
           else{
               reporte<<anterior.toStdString()<<"->"<<inode.toStdString()<<endl;
               anterior=inode;

           }

       }
        posicion_bm_inode++;
        fseek(archivo,posicion_bm_inode,SEEK_SET);
    }
    reporte<<"}"<<endl;
    reporte.close();
    QString tExtension="";
    if(QString::compare(extension,"pdf",Qt::CaseInsensitive)==0){
        tExtension="-Tpdf";
    }else if(QString::compare(extension,"png",Qt::CaseInsensitive)==0){
        tExtension="-Tpng";
    }else if(QString::compare(extension,"jpg",Qt::CaseInsensitive)==0){
        tExtension="-Tjpg";
    }


    QString comando="dot "+tExtension+" "+nombre_fichero+" -o "+ruta+"/"+value;

    system(comando.toStdString().c_str());

    cout<<"INFO:: El reporte se ha creado con exito"<<endl;

    return 0;
}

int Report::Reporte_Bloque(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }

    QString ruta_disco=temporal_mount.disco_ruta;
    //QString nombre_disco=temporal_mount.nombre_disco;

    QStringList listaR=value.split(".");
    QString nombre_fichero="";
    QString extension="";

    if(listaR.size()==2){
        nombre_fichero=listaR.at(0)+".dot";
        extension=listaR.at(1);
    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    superBloque tem_sb;
    fseek(archivo,temporal_part.part_start,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);


    if(tem_sb.magic==2017){
        cout<<"Ocurrio un error en la particion realice un recovery si es posible"<<endl;
        fclose(archivo);
        return 0;
    }


    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;
    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }

    reporte<<"digraph R {"<<endl;
    reporte<<"rankdir = LR;"<<endl;
    reporte<<"label=\" Nombre_Particion:"<<temporal_part.part_name<<" Nombre_disco:"<<temporal_mount.nombre_disco.toStdString()<<"\""<<endl;

    int start_bm_inode=tem_sb.bm_start_inodo;
    int start_inode=tem_sb.start_inodo;
    int start_bloque=tem_sb.start_bloque;

    QString anterior="";


    QString block="block";
    inodo tem_inodo;
    char estado='0';
    fseek(archivo,start_bm_inode,SEEK_SET);
    for(int i=0;i<tem_sb.inodes_count;i++){
        fread(&estado,sizeof(char),1,archivo);
        if(estado=='1'){//bitmap inode ocupados
            int puntero_inode=start_inode+(i*tem_sb.size_inodo);
            fseek(archivo,puntero_inode,SEEK_SET);
            fread(&tem_inodo,sizeof(inodo),1,archivo);
            if(tem_inodo.type_file=='0'){//carpeta

                carpeta tem_carpeta;
                for(int i=0;i<15;i++){
                    int status=tem_inodo.apuntador[i];
                    if(status!=-1){
                        if(i<12){//bloque directos,tipo carpeta
                            int apuntador=start_bloque+(status*sizeof(carpeta));
                            fseek(archivo,apuntador,SEEK_SET);
                            fread(&tem_carpeta,sizeof(carpeta),1,archivo);
                            block="block"+QString::number(status);

                            reporte<<block.toStdString()<<"[shape=record,color=red"<<endl;
                            QString name="Bloque Carpeta "+QString::number(status);
                            reporte<<"label=\""<<name.toStdString()<<"\\n"<<endl;
                            for(int j=0;j<4;j++){
                                reporte<<tem_carpeta.contenido[j].nombre_dir_file<<"        "<<tem_carpeta.contenido[j].apuntador_inodo<<"\\n"<<endl;
                            }
                            reporte<<"\"];"<<endl;
                        }else{

                        }
                        //ACA VAN LOS BLOQUES INDIRECTOS


                        if(QString::compare(anterior,"")==0){
                            anterior=block;
                        }else {
                            reporte<<anterior.toStdString()<<"->"<<block.toStdString()<<endl;
                            anterior=block;

                        }
                    }

                }


            }else{//archivos

                bloque_archivo tem_archivo;
                for(int i=0;i<15;i++){
                    int status=tem_inodo.apuntador[i];
                    if(status!=-1){
                        if(i<12){//bloque directos,tipo archivo
                            int apuntador=start_bloque+(status*sizeof(bloque_archivo));
                            fseek(archivo,apuntador,SEEK_SET);
                            fread(&tem_archivo,sizeof(bloque_archivo),1,archivo);
                            block="block"+QString::number(status);
                            reporte<<block.toStdString()<<"[shape=record,color=green"<<endl;
                            QString name="Bloque Archivo "+QString::number(status);
                            reporte<<"label=\""<<name.toStdString()<<"\\n"<<endl;
                            reporte<<tem_archivo.contenido<<"\\n"<<endl;
                            reporte<<"\"];"<<endl;
                        }

                        if(QString::compare(anterior,"")==0){
                            anterior=block;
                        }else {
                            reporte<<anterior.toStdString()<<"->"<<block.toStdString()<<endl;
                            anterior=block;

                        }
                    }



                }

            }

        }
        start_bm_inode++;
        fseek(archivo,start_bm_inode,SEEK_SET);
    }

    reporte<<"}"<<endl;
    reporte.close();
    QString tExtension="";
    if(QString::compare(extension,"pdf",Qt::CaseInsensitive)==0){
        tExtension="-Tpdf";
    }else if(QString::compare(extension,"png",Qt::CaseInsensitive)==0){
        tExtension="-Tpng";
    }else if(QString::compare(extension,"jpg",Qt::CaseInsensitive)==0){
        tExtension="-Tjpg";
    }


    QString comando="dot "+tExtension+" "+nombre_fichero+" -o "+ruta+"/"+value;

    system(comando.toStdString().c_str());

    cout<<"INFO:: El reporte se ha creado con exito"<<endl;

    return 0;
}


int Report::ReporteTree(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }

    QString ruta_disco=temporal_mount.disco_ruta;
    //QString nombre_disco=temporal_mount.nombre_disco;

    QStringList listaR=value.split(".");
    QString nombre_fichero="";
    QString extension="";

    if(listaR.size()==2){
        nombre_fichero=listaR.at(0)+".dot";
        extension=listaR.at(1);
    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    superBloque tem_sb;
    fseek(archivo,temporal_part.part_start,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);


    if(tem_sb.magic==2017){
        cout<<"Ocurrio un error en la particion realice un recovery si es posible"<<endl;
        fclose(archivo);
        return 0;
    }


    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;
    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }

    int pos_bmI=0;
    int padre=0;


    reporte<<"digraph g {"<<endl;
    reporte<<"graph [rankdir = LR];"<<endl;

    Tree_Inodo(padre,pos_bmI,-1,reporte,archivo,tem_sb);

    reporte<<"}"<<endl;
    reporte.close();
    QString tExtension="";
    if(QString::compare(extension,"pdf",Qt::CaseInsensitive)==0){
        tExtension="-Tpdf";
    }else if(QString::compare(extension,"png",Qt::CaseInsensitive)==0){
        tExtension="-Tpng";
    }else if(QString::compare(extension,"jpg",Qt::CaseInsensitive)==0){
        tExtension="-Tjpg";
    }


    QString comando="dot "+tExtension+" "+nombre_fichero+" -o "+ruta+"/"+value;

    system(comando.toStdString().c_str());

    cout<<"INFO:: El reporte se ha creado con exito"<<endl;

    return 0;
}

int Report::Tree_Inodo(int padre, int actual, int pos, ofstream &reporte, FILE *archivo,superBloque sb){
    inodo tem_i;
    int puntero=sb.start_inodo+(actual*sb.size_inodo);
    fseek(archivo,puntero,SEEK_SET);
    fread(&tem_i,sizeof(inodo),1,archivo);

    char fecha[100]="fecha";
    struct tm *timeinfo;

    reporte << "innode_" << actual;
        reporte << " [shape = \"record\",fillcolor=\"cadetblue1\",style=\"filled\" ,label=\"Inodo_" << actual << " ";
        reporte << "| {i_uid | " << tem_i.usuario_id << "}"<<endl;
        reporte << "| {i_gid | " << tem_i.grupo_id << "}"<<endl;
        reporte << "| {i_size | " << tem_i.size_file << "}"<<endl;
        timeinfo=localtime(&tem_i.ulti_time);
        strftime(fecha,100,"%c",timeinfo);
        reporte << "| {i_atime | " << fecha << "}"<<endl;
        timeinfo=localtime(&tem_i.crear_time);
        strftime(fecha,100,"%c",timeinfo);

        reporte << "| {i_ctime | " << fecha << "}"<<endl;
        timeinfo=localtime(&tem_i.modif_time);
        strftime(fecha,100,"%c",timeinfo);
        reporte << "| {i_mtime | " << fecha << "}"<<endl;
        reporte << "| {i_type | " << tem_i.type_file << "}"<<endl;
        reporte << "| {i_perm | " << tem_i.permiso << "}"<<endl;


        for(int i=0;i<15;i++){
            if(tem_i.apuntador[i]==-1){
                reporte << "|{AD" <<i<< "|<"<<i<< ">"<<tem_i.apuntador[i]<<"}"<<endl;

            }else{
                reporte << "|{AD" <<i<< "|<"<< i << ">"<<tem_i.apuntador[i]<<"}"<<endl;

            }
        }

        reporte << "\"];";
        reporte<< "\n";

        if(pos!=-1){//se hace cuando ya traigo algo
            reporte<<"bloque_"<<padre<<":<"<<pos<<"> -> "<<"innode_"<<actual<<";"<<endl;
        }
        int type=0;
        for(int i=0;i<15;i++){
            if(tem_i.apuntador[i]!=-1){
                if(tem_i.type_file=='0'){type=0;}
                else{type=1;}

                Tree_Bloque(actual,tem_i.apuntador[i],i,reporte,archivo,sb,type);
            }
        }
    return 0;
}


int Report::Tree_Bloque(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo){
    int apuntador=sb.start_bloque+(actual*sb.size_bloque);
    fseek(archivo,apuntador,SEEK_SET);
    if(pos<12){
        if(tipo==0){//carpeta
            carpeta tem_ca;
            fread(&tem_ca,sizeof(carpeta),1,archivo);

            reporte << "bloque_" << actual;
            reporte << "[shape = \"record\",fillcolor=\"red\",style=\"filled\", label=\"Bloque_" << actual << " ";

            for(int i=0;i<4;i++){
                //int pos=tem_ca.contenido[i].apuntador_inodo;
                //pos++;
                //este es el inodo padre                                                     //este es el siguiente
                reporte<< "|{" <<tem_ca.contenido[i].nombre_dir_file<<"|<"<<i<<">"<<tem_ca.contenido[i].apuntador_inodo<<"}"<<endl;
            }

            reporte << "\"];";
            reporte<< "\n";

            //toca conectar el inodo con el bloque

            reporte<<"innode_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;
            if(pos==0){
                for(int i=2;i<4;i++){
                    if(tem_ca.contenido[i].apuntador_inodo!=-1){
                        Tree_Inodo(actual,tem_ca.contenido[i].apuntador_inodo,i,reporte,archivo,sb);
                    }
                }
            }else{
                for(int i=0;i<4;i++){
                    if(tem_ca.contenido[i].apuntador_inodo!=-1){
                        Tree_Inodo(actual,tem_ca.contenido[i].apuntador_inodo,i,reporte,archivo,sb);
                    }
                }
            }



        }else if(tipo==1){//archivo
           bloque_archivo tem_ar;
           fread(&tem_ar,sizeof(bloque_archivo),1,archivo);

           QString con(QString::fromLocal8Bit(tem_ar.contenido));
           reporte << "bloque_" << actual;
           reporte << "[shape = \"record\",fillcolor=\"green\",style=\"filled\", label=\"Bloque_" << actual << " ";

           reporte<<"|"<<con.toStdString()<<endl;



           reporte << "\"];";
           reporte<< "\n";

           reporte<<"innode_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;
           return 0;
        }
        return 0;
    }else{
        Tree_Indirecto(padre,actual,pos,reporte,archivo,sb,tipo);
    }

    return 0;
}

int Report::Tree_Indirecto(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo){
    int apuntador=sb.start_bloque+(actual*sb.size_bloque);
    fseek(archivo,apuntador,SEEK_SET);

    if(pos==12){
        bloque_pointers tem_p;
        fread(&tem_p,sizeof(bloque_pointers),1,archivo);
        reporte << "bloque_" << actual;
        reporte << "[shape = \"record\",fillcolor=\"yellow\",style=\"filled\", label=\"Bloque_" << actual << " ";
        for(int i=0;i<16;i++){
            //int pos=tem_p.apuntador[i];
            //pos++;
            //este es el inodo padre
            reporte<< "|{" <<i<<"|<"<<i<<">"<<tem_p.apuntador[i]<<"}"<<endl;
        }
        reporte << "\"];";
        reporte<< "\n";
        reporte<<"innode_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;
        for(int i=0;i<16;i++){
            if(tem_p.apuntador[i]!=-1){
                Indirecto_Simple(actual,tem_p.apuntador[i],i,reporte,archivo,sb,tipo);
            }
        }
    }else if(pos==13){
        bloque_pointers tem_p;
        fread(&tem_p,sizeof(bloque_pointers),1,archivo);
        reporte << "bloque_" << actual;
        reporte << "[shape = \"record\",fillcolor=\"yellow\",style=\"filled\", label=\"Bloque_" << actual << " ";
        for(int i=0;i<16;i++){
            int pos=tem_p.apuntador[i];
            pos++;
            //este es el inodo padre
            reporte<< "|{" <<i<<"|<"<<i<<">"<<tem_p.apuntador[i]<<"}"<<endl;
        }
        reporte << "\"];";
        reporte<< "\n";
        reporte<<"innode_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;
        for(int i=0;i<16;i++){
            if(tem_p.apuntador[i]!=-1){
                Indirecto_Doble(actual,tem_p.apuntador[i],i,reporte,archivo,sb,tipo);
            }
        }
    }else if(pos==14){
        bloque_pointers tem_p;
        fread(&tem_p,sizeof(bloque_pointers),1,archivo);
        reporte << "bloque_" << actual;
        reporte << "[shape = \"record\",fillcolor=\"yellow\",style=\"filled\", label=\"Bloque_" << actual << " ";
        for(int i=0;i<16;i++){
            //int pos=tem_p.apuntador[i];
            //pos++;
            //este es el inodo padre
            reporte<< "|{" <<i<<"|<"<<i<<">"<<tem_p.apuntador[i]<<"}"<<endl;
        }
        reporte << "\"];";
        reporte<< "\n";
        reporte<<"innode_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;
        for(int i=0;i<16;i++){
            if(tem_p.apuntador[i]!=-1){
                Indirecto_Triple(actual,tem_p.apuntador[i],i,reporte,archivo,sb,tipo);
            }
        }
    }
    return 0;
}


int Report::Indirecto_Simple(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo){
    int apuntador=sb.start_bloque+(actual*sb.size_bloque);
    fseek(archivo,apuntador,SEEK_SET);

    //verificar que tipo de blque carpeta o archivo
    if(tipo==0){
        carpeta tem_ca;
        fread(&tem_ca,sizeof(carpeta),1,archivo);

        reporte << "bloque_" << actual;
        reporte << "[shape = \"record\",fillcolor=\"red\",style=\"filled\", label=\"Bloque_" << actual << " ";

        for(int i=0;i<4;i++){
            //int pos=tem_ca.contenido[i].apuntador_inodo;
            //pos++;
            //este es el inodo padre                                                     //este es el siguiente
            reporte<< "|{" <<tem_ca.contenido[i].nombre_dir_file<<"|<"<<i<<">"<<tem_ca.contenido[i].apuntador_inodo<<"}"<<endl;
        }

        reporte << "\"];";
        reporte<< "\n";

        //toca conectar el inodo con el bloque

        reporte<<"bloque_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;

        for(int i=0;i<4;i++){
            if(tem_ca.contenido[i].apuntador_inodo!=-1){
                Tree_Inodo(actual,tem_ca.contenido[i].apuntador_inodo,i,reporte,archivo,sb);
            }
        }
        return 0;
    }else if(tipo==1){
        bloque_archivo tem_ar;
        fread(&tem_ar,sizeof(bloque_archivo),1,archivo);

        QString con(QString::fromLocal8Bit(tem_ar.contenido));
        reporte << "bloque_" << actual;
        reporte << "[shape = \"record\",fillcolor=\"green\",style=\"filled\", label=\"Bloque_" << actual << " ";

        reporte<<"|"<<con.toStdString()<<endl;

        reporte << "\"];";
        reporte<< "\n";

        reporte<<"bloque_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;
        return 0;
    }
    return 0;
}


int Report::Indirecto_Doble(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo){
    int apuntador=sb.start_bloque+(actual*sb.size_bloque);
    fseek(archivo,apuntador,SEEK_SET);

    bloque_pointers tem_ca;
    fread(&tem_ca,sizeof(bloque_pointers),1,archivo);

    reporte << "bloque_" << actual;
    reporte << "[shape = \"record\",fillcolor=\"yellow\",style=\"filled\", label=\"Bloque_" << actual << " ";

    for(int i=0;i<16;i++){
        //int pos=tem_ca.apuntador[i];
        //pos++;
        //este es el inodo padre                                                     //este es el siguiente
        reporte<< "|{" <<i<<"|<"<<i<<">"<<tem_ca.apuntador[i]<<"}"<<endl;
    }

    reporte << "\"];";
    reporte<< "\n";

    //toca conectar el bloque con el bloque

    reporte<<"bloque_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;

    for(int i=0;i<16;i++){
        if(tem_ca.apuntador[i]!=-1){
            Indirecto_Simple(actual,tem_ca.apuntador[i],i,reporte,archivo,sb,tipo);
        }
    }
    return -1;
}

int Report::Indirecto_Triple(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo){
    int apuntador=sb.start_bloque+(actual*sb.size_bloque);
    fseek(archivo,apuntador,SEEK_SET);

    bloque_pointers tem_ca;
    fread(&tem_ca,sizeof(bloque_pointers),1,archivo);

    reporte << "bloque_" << actual;
    reporte << "[shape = \"record\",fillcolor=\"yellow\",style=\"filled\", label=\"Bloque_" << actual << " ";

    for(int i=0;i<16;i++){
        //int pos=tem_ca.apuntador[i];
        //pos++;
        //este es el inodo padre                                                     //este es el siguiente
        reporte<< "|{" <<i<<"|<"<<i<<">"<<tem_ca.apuntador[i]<<"}"<<endl;
    }

    reporte << "\"];";
    reporte<< "\n";

    //toca conectar el bloque con el bloque

    reporte<<"bloque_"<<padre<<":<"<<pos<<"> ->"<<"bloque_"<<actual<<";"<<endl;

    for(int i=0;i<16;i++){
        if(tem_ca.apuntador[i]!=-1){
            Indirecto_Doble(actual,tem_ca.apuntador[i],i,reporte,archivo,sb,tipo);
        }
    }
    return -1;
}


int Report::Reporte_Journaling(QString id_part, QString ruta, QString value){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }

    QString ruta_disco=temporal_mount.disco_ruta;
    //QString nombre_disco=temporal_mount.nombre_disco;

    QStringList listaR=value.split(".");
    QString nombre_fichero="";
    QString extension="";

    if(listaR.size()==2){
        nombre_fichero=listaR.at(0)+".dot";
        extension=listaR.at(1);
    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    superBloque sb;
    int inicio_particion=temporal_part.part_start;
    fseek(archivo,inicio_particion,SEEK_SET);
    fread(&sb,sizeof(superBloque),1,archivo);
    nombre_fichero=ruta+"/"+nombre_fichero;
    ofstream reporte;

    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }

    reporte<<"digraph H {"<<endl;
    reporte <<"aHtmlTable ["<<endl;
    reporte<<"shape=plaintext"<<endl;
    reporte<<"color=black"<<endl;
    reporte<<"label=<"<<endl;
    reporte<<"<table border='1' cellborder='1' cellspacing='0'>"<<endl;
    reporte<<"<tr><td>Operacion</td><td>Fecha</td></tr>"<<endl;


    int inicio=temporal_part.part_start+sizeof(superBloque);
    int fin=sb.inodes_count;

    fseek(archivo,temporal_part.part_start,SEEK_SET);
    fread(&sb,sizeof(superBloque),1,archivo);

    if(sb.filesystem_type==1){
        journaling tem_j;
        int apuntador=inicio;
        int contador=0;
        fseek(archivo,apuntador,SEEK_SET);
        for(int i=0;i<fin;i++){
            fread(&tem_j,sizeof(journaling),1,archivo);
            if(tem_j.status=='1'){

                reporte<<"<tr>"<<endl;
                char fecha[100]="fecha";
                struct tm *timeinfo;
                timeinfo=localtime(&tem_j.fecha);
                strftime(fecha,100,"%c",timeinfo);

                reporte<<"<td>"<<tem_j.operacion<<"</td><td>"<<fecha<<"</td>";

                reporte<<"</tr>"<<endl;
            }
            contador++;
            apuntador=inicio+(contador*sizeof(journaling));
            fseek(archivo,apuntador,SEEK_SET);
        }
    }

    reporte<<"</table> >]; }";

    reporte.close();
    QString tExtension="";
    if(QString::compare(extension,"pdf",Qt::CaseInsensitive)==0){
        tExtension="-Tpdf";
    }else if(QString::compare(extension,"png",Qt::CaseInsensitive)==0){
        tExtension="-Tpng";
    }else if(QString::compare(extension,"jpg",Qt::CaseInsensitive)==0){
        tExtension="-Tjpg";
    }


    QString comando="dot "+tExtension+" "+nombre_fichero+" -o "+ruta+"/"+value;

    system(comando.toStdString().c_str());

    cout<<"INFO:: El reporte se ha creado con exito"<<endl;
    return 0;
}


int Report::Reporte_File(QString id_part,QString ruta,QString value,QString ruta_archivo){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }



    QString nombre_fichero=ruta+"/"+value;
    ofstream reporte;




     QString ruta_disco=temporal_mount.disco_ruta;

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    superBloque tem_sb;
    int inicio_particion=temporal_part.part_start;
    fseek(archivo,inicio_particion,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);




    QStringList lista=ruta_archivo.split("/");

    reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
    if(reporte.fail()){
        cout<<"No se puede crear el archivo de reporte"<<endl;
        return 0;
    }

    fclose(archivo);
    if(tem_sb.magic==201800476){



        int inodo_encontrado=Buscar_Carpeta_Archivo(0,tem_sb,lista,1);

        if(inodo_encontrado>=0){

            int salida=Recorrer_Bloque_Archivos(inodo_encontrado,tem_sb);

            //aca se muestra en pantalla
            if(salida==0){
                    reporte<<this->contenidos_archivos.toStdString()<<endl;
                    cout<<"Se creo el reporte de file:"<<ruta_archivo.toStdString()<<endl;
            }
            this->contenidos_archivos="";
        }else{
            cout<<"No se encontro el archivo especificado para hacer el reporte de file"<<endl;
            reporte.close();
            return 0;
        }


    }else if(tem_sb.magic==2017){
        cout<<"La particion sufrio un problema no se pueden hacer el reporte file"<<endl;
        reporte.close();
        return 0;
    }else{

    }
    reporte.close();
    return 0;
}



int Report::Buscar_Carpeta_Archivo(int pos_bmi,superBloque sb, QStringList rutas,int pos){
    if(pos>rutas.size()-1){return pos_bmi;} //retorno todo;


    FILE *archivo;
    archivo=fopen(temporal_mount.disco_ruta.toStdString().c_str(),"rb+");
    fseek(archivo,temporal_part.part_start,SEEK_SET);
    fread(&sb,sizeof(superBloque),1,archivo);

    //buscar el inodo y recorrer los apuntadores
    inodo tem_inodo;
    carpeta tem_car;
    bloque_pointers tem_pon;

    int apuntador=sb.start_inodo+(pos_bmi*sb.size_inodo);

    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_inodo,sizeof(inodo),1,archivo);
    QByteArray array = rutas[pos].toLocal8Bit();
    char* nom_carpeta = array.data();



    for(int i=0;i<15;i++){
        //Verificar si es inodo archivo o carpeta?
        if(i<12){//********************************** APUNTADORES DIRECTOS******************************
            if(tem_inodo.apuntador[i]!=-1){

                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de carpetas
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_car,sizeof(carpeta),1,archivo);

                for(int j=0;j<4;j++){
                    if(tem_car.contenido[j].apuntador_inodo!=-1){
                        //existe la carpeta lo mando de manera recursiva
                        if(strcasecmp(tem_car.contenido[j].nombre_dir_file,nom_carpeta)==0){
                            pos++;
                            fclose(archivo);
                           // encontre=true;
                            return Buscar_Carpeta_Archivo(tem_car.contenido[j].apuntador_inodo,sb,rutas,pos);

                        }
                    }
                }

            }
        }else{//************************************ APUNTADORES INDIRECTOS************************************

            if(tem_inodo.apuntador[i]!=-1){
                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de apuntadores
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

                for(int j=0;j<16;j++){
                    if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                        //se mando las posicion del inodo

                        int salida= Buscar_Indirectos_C_A(rutas,sb,i,pos,tem_pon.apuntador[j],archivo);
                        if(salida>=0){
                            return salida;
                        }
                    }
                }
            }
        }

    }
    return -1;
}


int Report::Buscar_Indirectos_C_A(QStringList rutas, superBloque sb, int indirecto, int pos_ruta, int block, FILE *archivo){

    if(indirecto==12){

        if(this->exclusivo_archivos==-1){
            QByteArray array = rutas[pos_ruta].toLocal8Bit();
            char* nom_carpeta = array.data();
            int apuntador=sb.start_bloque+(block*sb.size_bloque);
            //aca seria bloque de carpetas
            carpeta tem_car;
            fseek(archivo,apuntador,SEEK_SET);
            fread(&tem_car,sizeof(carpeta),1,archivo);
            for(int j=0;j<4;j++){
                if(tem_car.contenido[j].apuntador_inodo!=-1){
                    //existe la carpeta lo mando de manera recursiva
                    if(strcasecmp(tem_car.contenido[j].nombre_dir_file,nom_carpeta)==0){
                        pos_ruta++;
                        //fclose(archivo);
                        return Buscar_Carpeta_Archivo(tem_car.contenido[j].apuntador_inodo,sb,rutas,pos_ruta);
                    }
                }
            }
            return -1;
        }else{
            int apuntador=sb.start_bloque+(block*sb.size_bloque);
            bloque_archivo tem_archivo;
            fseek(archivo,apuntador,SEEK_SET);
            fread(&tem_archivo,sizeof(bloque_archivo),1,archivo);

            //leer contenido del archivo y mostrar en consola
            QString conte(tem_archivo.contenido);

            this->contenidos_archivos=this->contenidos_archivos+conte;
        }

    }else if(indirecto==13){
        //en crear carpeta accedo al primer bloque de pointers
        //aca accedo a segundo bloque y estos ya van a tener bloques carpetas y mando restando a indirecto
        int pos_bloque=block;
        int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
        bloque_pointers tem_pon;
        //aca seria bloque de carpetas
        fseek(archivo,apuntador,SEEK_SET);
        fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

        for(int j=0;j<16;j++){
            if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                //se mando las posicion del inodo
                int regreso= Buscar_Indirectos_C_A(rutas,sb,indirecto-1,pos_ruta,tem_pon.apuntador[j],archivo);
                if(regreso>-1){
                    return regreso;//trae una pos inodo
                }
            }
        }

    }else if(indirecto==14){
        //crear carpeta el primer bloque pointers
        //mando crear indirecto y le resto a indirecto para que sea 12
        //luego retorna el inodo
        int pos_bloque=block;
        int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
        bloque_pointers tem_pon;
        //aca seria bloque de carpetas
        fseek(archivo,apuntador,SEEK_SET);
        fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

        for(int j=0;j<16;j++){
            if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                //se mando las posicion del inodo
                int regreso= Buscar_Indirectos_C_A(rutas,sb,indirecto-1,pos_ruta,tem_pon.apuntador[j],archivo);
                if(regreso>-1){
                    return regreso;//trae una pos de inodo
                }

            }
        }
    }
    return -1;
}


int Report::Recorrer_Bloque_Archivos(int pos_inodo,superBloque sb){

    this->exclusivo_archivos=1;

    FILE *archivo;
    archivo=fopen(temporal_mount.disco_ruta.toStdString().c_str(),"rb+");

    int apuntador=sb.start_inodo+(pos_inodo*sb.size_inodo);
    inodo tem_inodo;

    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_inodo,sizeof(inodo),1,archivo);

    bloque_archivo tem_arch;
    if(tem_inodo.type_file==0){
        cout<<"La ruta especificada es una carpeta"<<endl;
        return 2;
    }

    for(int i=0;i<15;i++){
        //Verificar si es inodo archivo o carpeta?
        if(i<12){//********************************** APUNTADORES DIRECTOS******************************
            if(tem_inodo.apuntador[i]!=-1){

                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de carpetas
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_arch,sizeof(bloque_archivo),1,archivo);
                QString conte(tem_arch.contenido);

                this->contenidos_archivos=this->contenidos_archivos+conte;

            }
        }else{//************************************ APUNTADORES INDIRECTOS************************************

            if(tem_inodo.apuntador[i]!=-1){
                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
                bloque_pointers tem_pon;
                //aca seria bloque de apuntadores
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

                for(int j=0;j<16;j++){
                    if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                        //se mando las posicion del inodo
                        QStringList vacia;
                        Buscar_Indirectos_C_A(vacia,sb,i,0,tem_pon.apuntador[j],archivo);
                    }
                }
            }
        }

    }
    fclose(archivo);
    this->exclusivo_archivos=-1;
    return 0;
}



int Report::Reporte_Ls(QString id_part, QString ruta, QString value, QString ruta_archivo){
    int posicion=Verificar_montaje(id_part);
    if(posicion==-1){
        cout<<"ERROR:: no se puede generar el reporte, ya que no esta montada la particion"<<endl<<endl;
        return 0;
    }

    QStringList listaR=value.split(".");
    QString nombre_fichero="";
    QString extension="";

    if(listaR.size()==2){
        nombre_fichero=listaR.at(0)+".dot";
        extension=listaR.at(1);
    }else{
        cout<<"No se especifico una extesion para crear el reporte"<<endl;
        return 0;
    }

    nombre_fichero=ruta+"/"+nombre_fichero;

    ofstream reporte;


     QString ruta_disco=temporal_mount.disco_ruta;

    FILE *archivo;
    archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: No se pudo encontrar el disco"<<endl;
        return 0;
    }
    superBloque tem_sb;
    int inicio_particion=temporal_part.part_start;
    fseek(archivo,inicio_particion,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);

    QStringList lista=ruta_archivo.split("/");


    fclose(archivo);
    if(tem_sb.magic==201800476){



        int inodo_encontrado=Buscar_Carpeta_Archivo(0,tem_sb,lista,1);

        if(inodo_encontrado>=0){

            archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
            int apuntador=tem_sb.start_inodo+(inodo_encontrado*tem_sb.size_inodo);
            fseek(archivo,apuntador,SEEK_SET);
            inodo tem_inodo;
            fread(&tem_inodo,sizeof(inodo),1,archivo);

            reporte.open(nombre_fichero.toStdString().c_str(),ios::out);
            if(reporte.fail()){
                cout<<"No se puede crear el archivo de reporte"<<endl;
                return 0;
            }

            reporte<<"digraph H {"<<endl;
            reporte<<"aHtmlTable ["<<endl;
            reporte<<"shape=plaintext"<<endl;
            reporte<<"color=black"<<endl;
            reporte<<"label=<"<<endl;
            reporte<<"<table border='1' cellborder='1' cellspacing='0'>"<<endl;

            reporte<<"<tr><td>REPORTE LS</td></tr>"<<endl;


            reporte<<"<tr>"<<endl;
            reporte<<"<td>Permisos</td>"<<endl;
            reporte<<"<td>Owner</td>"<<endl;
            reporte<<"<td>Grupo</td>"<<endl;
            reporte<<"<td>Size(en bytes)</td>"<<endl;
            reporte<<"<td>Fecha y Hora</td>"<<endl;
            reporte<<"<td>Tipo</td>"<<endl;
            reporte<<"<td>Name</td>"<<endl;
            reporte<<"</tr>"<<endl;

            reporte<<"<tr>"<<endl;
            reporte<<"<td>"<<tem_inodo.permiso<<"</td>"<<endl;

            Buscar_Usuario(tem_inodo.usuario_id,tem_inodo.grupo_id,reporte);

            reporte<<"<td>"<<tem_inodo.size_file<<"</td>"<<endl;

            char fecha[100]="fecha";
            struct tm *timeinfo;
            timeinfo=localtime(&tem_inodo.modif_time);
            strftime(fecha,100,"%c",timeinfo);
            reporte<<"<td>"<<fecha<<"</td>"<<endl;

            if(tem_inodo.type_file=='0'){
                reporte<<"<td>Carpeta</td>"<<endl;
            }else{
                reporte<<"<td>Archivo</td>"<<endl;
            }

            reporte<<"<td>"<<lista.at(lista.size()-1).toStdString()<<"</td>"<<endl;

            reporte<<"</tr>"<<endl;

            reporte<<"</table> >]; }";

            reporte.close();
            QString tExtension="";
            if(QString::compare(extension,"pdf",Qt::CaseInsensitive)==0){
                tExtension="-Tpdf";
            }else if(QString::compare(extension,"png",Qt::CaseInsensitive)==0){
                tExtension="-Tpng";
            }else if(QString::compare(extension,"jpg",Qt::CaseInsensitive)==0){
                tExtension="-Tjpg";
            }


            QString comando="dot "+tExtension+" "+nombre_fichero+" -o "+ruta+"/"+value;

            system(comando.toStdString().c_str());

            cout<<"INFO:: El reporte se ha creado con exito"<<endl;


        }else{
            cout<<"No se encontro el archivo especificado para hacer el reporte de file"<<endl;
            reporte.close();
            return 0;
        }


    }else if(tem_sb.magic==2017){
        cout<<"La particion sufrio un problema no se pueden hacer el reporte file"<<endl;
        reporte.close();
        return 0;
    }
    return 0;

}


int Report::Buscar_Usuario(int id_usuario,int id_grupo,ofstream &reporte){
   QString ruta_buscar="/user.txt";
   QStringList lista=ruta_buscar.split("/");

    QString ruta_disco=temporal_mount.disco_ruta;

   FILE *archivo;
   archivo=fopen(ruta_disco.toStdString().c_str(),"rb+");
   if(archivo==NULL){
       cout<<"Error: No se pudo encontrar el disco"<<endl;
       return 0;
   }
   superBloque tem_sb;
   int inicio_particion=temporal_part.part_start;
   fseek(archivo,inicio_particion,SEEK_SET);
   fread(&tem_sb,sizeof(superBloque),1,archivo);

   int inodo_encontrado=Buscar_Carpeta_Archivo(0,tem_sb,lista,1);

   if(inodo_encontrado>=0){//se encontro el inodo

       int salida=Recorrer_Bloque_Archivos(inodo_encontrado,tem_sb);

       //aca se muestra en pantalla
       if(salida==0){

            Datos_Usuario(id_usuario,id_grupo,this->contenidos_archivos,reporte);//busca el usuario y grupo (nombres)
       }
       this->contenidos_archivos="";


   }else{
       cout<<"No se encontro el archivo especificado para hacer el reporte de file"<<endl;

       return 0;
   }


}


int Report::Datos_Usuario(int id_usuario,int id_grupo,QString contenido,ofstream &reporte){

    QStringList registro=contenido.split("\\n");
    QString nombre_user="";


    QString nombre_grupo="";

    for(int i=0;i<registro.size()-1;i++){

        QString registroN=registro.at(i);

        QStringList campos=registroN.split(",");

        QString numero=campos.at(0);
        QString is_usuario=campos.at(1);

        if(QString::compare(is_usuario,"U")==0){
            int id=numero.toInt();

            if(id==id_usuario){
               nombre_user=campos.at(2);

               break;
            }

        }

    }

    for(int i=0;i<registro.size()-1;i++){
        QString registroN=registro.at(i);

        QStringList campos=registroN.split(",");

        QString numero=campos.at(0);
        QString is_usuario=campos.at(1);

        if(QString::compare(is_usuario,"G")==0){
            int id_g=numero.toInt();
            if(id_g==id_grupo){
                nombre_grupo=campos.at(2);
                break;
            }
        }
    }
    reporte<<"<td>"<<nombre_user.toStdString()<<"</td>"<<endl;
    reporte<<"<td>"<<nombre_grupo.toStdString()<<"</td>"<<endl;
   return 0;
}



int Report::Verificar_montaje(QString id_part){
    temporal_mount.estado='0';
    temporal_part.part_start='0';
    int posicion=-1;
    for(int i=0;i<50;i++){
        if(montar_repo[i].estado==1){
            if(QString::compare(id_part,montar_repo[i].id_mont,Qt::CaseInsensitive)==0){
                temporal_mount=montar_repo[i];
                temporal_mount.estado='1';
                posicion=i;
                FILE *archivo;
                archivo=fopen(temporal_mount.disco_ruta.toStdString().c_str(),"rb+");

                if(archivo==NULL){
                    fclose(archivo);
                    cout<<"Error: No se encontro el disco"<<endl;
                    return -1;
                }
                fseek(archivo,0,SEEK_SET);
                mbr disco;
                fread(&disco,sizeof(mbr),1,archivo);

                for(int j=0;j<4;j++){
                    if(disco.part[j].part_status=='1'){
                        if(strcasecmp(temporal_mount.nombre_part.toStdString().c_str(),disco.part[j].part_name)==0){
                            temporal_part=disco.part[j];
                            fclose(archivo);
                            return posicion;
                        }
                    }
                }
                fclose(archivo);
                return i;
            }
        }
    }
    cout<<"El id"<<id_part.toStdString()<<" especificado no pertenece a ninguna particion montada"<<endl;
    return -1;
}

