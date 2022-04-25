#include"element.h"
#include"fs.h"
#include<string.h>
#include<stdlib.h>
//scanf("%s", path) == 0 GO BACK TO THIS
int main(int argc, char** argv){
    char cmd[21];
    char path[PATHMAX];
    FS * fs = newFileSystem();
    
    while(scanf("%s", cmd) > 0){
        if(strcmp(cmd, "ls") == 0){
            if(scanf("%s", path) == 0){// if there is a path
                fs->fs_lsP(fs, path);
            }
            else {
                fs->fs_ls(fs);
            }
        }
        else if(strcmp(cmd, "pwd") == 0){
            fs->fs_pwd(fs);
        }
        else if(strcmp(cmd, "mkdir") == 0){
            if(scanf("%s", path) == 0){// if there is a path
                fs->fs_mkdir(fs, path);   
            }
        }
        else if(strcmp(cmd, "touch") == 0){
            if(scanf("%s", path) == 0){// if there is a path
                fs->fs_touch(fs, path);
            }
        }
        else if(strcmp(cmd, "cd") == 0){
            if(scanf("%s", path) == 0){// if there is a path
                fs->fs_cd(fs, path);
            }
        }
        else if(strcmp(cmd, "rm") == 0){
            if(scanf("%s", path) == 0){// if there is a path
                fs->fs_rm(fs, path);
            }
        }
        else if(strcmp(cmd, "rm -f") == 0){
            if(scanf("%s", path) == 0){// if there is a path
                fs->fs_rmf(fs, path);
            }
        }
        else if(strcmp(cmd, "find") == 0){
            //find local name
            char localName[21];
            if(scanf("%s", localName) == 1)
            printf("Searching For '%s'\n", localName);
            fs->fs_find(fs, localName);
        }
    }
    fs->destroy(fs);
    return 0;
}
