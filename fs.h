#ifndef __fs_h__
#define __fs_h__

#include"element.h"
#include<stdlib.h>
#define PATHMAX 1001

typedef struct _file_system FS;
struct _file_system{
    //don't know if this is right
    void (*fs_ls)(FS*);
    void (*fs_lsP)(FS*, char*);
    void (*fs_pwd)(FS*);
    void (*fs_mkdir)(FS*, char*);
    void (*fs_touch)(FS*, char*);
    void (*fs_cd)(FS*, char*);
    void (*fs_rm)(FS*, char*);
    void (*fs_rmf)(FS*, char*);
    void (*fs_find)(FS*, char*);
    void (*destroy)(FS*);
};

FS * newFileSystem();

#endif
