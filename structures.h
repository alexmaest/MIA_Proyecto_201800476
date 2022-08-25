#ifndef STRUCTURES_H
#define STRUCTURES_H
#include<time.h>
#include<QString>
using namespace std;

typedef struct EBR{
    char status;
    char fit;
    int start;
    int singleSize;
    int next;
    char name[16];
}ebr;

typedef struct PARTITION{
    char state;
    char type;
    char fit;
    int start;
    int singleSize;
    char name[16];
}singlePartition;

typedef struct MBR{
    int singleSize;
    time_t singleDate;
    int signature;
    char fit;
    singlePartition part [4];
}mbr;

typedef struct MOUNT{
    QString diskPath;
    QString diskName;
    QString partName;
    QString MountID;
    int state;
    int number;
    int letterNumber;
}mount;

typedef struct SUPERBLOCK{
    int type;
    int inodesCounter;
    int blocksCounter;
    int blocksCount;
    int freeInodesCount;
    time_t mountTime;
    time_t unmountTime;
    int mountCounter;
    int magic;
    int inodeSize;
    int blockSize;
    int firstFreeInode;
    int firstFreeBlock;
    int startInodeBM;
    int startBlockBM;
    int startInode;
    int startBlock;
}superBlock;

typedef struct INODE{
    int userId;
    int groupId;
    int fileSize;
    time_t ultiTime;
    time_t createTime;
    time_t editTime;
    int pointer[15];
    char fileType;
    int permission;
}inode;

typedef struct CONTENT{
    char dirNameFile[12];
    int inodePointer;
}content;

typedef struct FOLDER{
    content singleContent[4];
}folder;

typedef struct FILEBLOCK{
    char content[64];
}fileBlock;

typedef struct POINTERSBLOCK{
    int pointer[16];
}pointersBlock;

typedef struct LOGIN{
    char session;
    char user[10];
    char pass[10];
    QString partID;
    int userID;
    int groupID;
    int permission;
}login;

typedef struct JOURNALING{
   char operation[250];
   time_t date;
   char status = 0;
}journaling;

class structures
{
public:
    structures();
};

#endif // STRUCTURES_H
