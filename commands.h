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

};

#endif // COMMANDS_H
