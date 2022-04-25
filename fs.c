#include"fs.h"
#include"element.h"

typedef struct _tree Tree;
struct _tree{
  FS ti;
  node* root;
  node* cwd;
};


void fs_ls(FS*);
void fs_lsP(FS*, char*);
void fs_pwd(FS*);
void fs_mkdir(FS*, char*);
void fs_touch(FS*, char*);
void fs_cd(FS*, char*);
void fs_rm(FS*, char*);
void fs_rmf(FS*, char*);
void fs_find(FS*, char*);
void destroy(FS*);

FS * newFileSystem(){
  Tree * t = (Tree*) malloc(sizeof(Tree));
  t->ti.fs_ls = fs_ls;
  t->ti.fs_lsP = fs_lsP;
  t->ti.fs_pwd = fs_pwd;
  t->ti.fs_mkdir = fs_mkdir;
  t->ti.fs_touch = fs_touch;
  t->ti.fs_cd = fs_cd;
  t->ti.fs_rm = fs_rm;
  t->ti.fs_find = fs_find;
  t->ti.destroy = destroy;
  t->root = NULL;
  t->cwd = NULL;
  
  return (FS*) t;
}


void node_ls(node* n){
  if(n == NULL) return;
  printf("%c %s", n->flag, n->name); 
  printf("%c %s", n->flag, n->nextsibling->name);
  //is the while necessary?
  while(n->nextsibling->nextsibling != NULL){
    node_ls(n->nextsibling->nextsibling);
  }
}

void fs_ls(FS* fs){
  Tree* t = (Tree*) fs;
  node* childOfCWD = t->cwd->firstchild;
  node_ls(childOfCWD);
}





void fs_lsP(FS* fs, char* P){
  Tree* t = (Tree*) fs;
  node* nodeToPath = follow(t->root, P++);//might be right
  if(nodeToPath == NULL){
    printf("Error Output: List Error: Cannot perform list operation.\n");
    return;
  }
  if(nodeToPath->flag == 'F'){// maybe need path != NULL?
    printf("Error Output: List Error: Cannot perform list operation. %s is a file.\n", PWD(t->root, P));
    return;
  }
  node* childOfAbPath = nodeToPath->firstchild;
  printf("Listing For \'%s\':", PWD(t->root, P++));//++ to skip leading '/'
  node_ls(childOfAbPath);
}

//call the pwd helper after getting the space
void fs_pwd(FS* fs){
  Tree* t = (Tree*) fs;
  char* P;
  P = PWD(t->cwd, "/");
  printf("%s", P);
}




//just might be done
void node_mkdir(node* n, char* P){
  char* token; 
  token = strtok(P, "/");
  
  if(strlen(token) == 0) return;
  
  node* child = getChild(n, token);
  
  if(child == NULL){
    node* newDir = (node*)malloc(sizeof(node));
    strcpy(newDir->name, token);
    //newDir->name = token;
    newDir->flag = 'D';
    insert(n, newDir);
    node_mkdir(newDir, getLocalName(P, token)); //subpath in notes? is getLocalName kinda the same?
  }
  else
    node_mkdir(child, getLocalName(P, token));
}


void fs_mkdir(FS* fs, char* P){
  Tree* t = (Tree*) fs;
  //need to check path type?
  node* currentORroot;
  if(P[0] == '/'){
    currentORroot = t->root;
    P = P++;
  }
  else currentORroot = t->cwd;
  
  //either in here or helper assign dir a flag=D
  if(safeToCreate(currentORroot, P)) node_mkdir(currentORroot, P);//create all missing dirs along given patt; is t->root right?
  else printf("Make Dir Error: Cannot create directory.\n");
}





//done?
void node_touch(node* n, char* P){
  char* last;
  last = strrchr(P, '/');//get last localName (including '/') in P
  
  char head[PATHMAX - strlen(last)]; //= ""; //to store entire path except last
  //memset(head, 0, PATHMAX*sizeof(char));//no sizeof char*? :)
  
  strncpy(head, P, strlen(P)-strlen(last)/*+1 */); //write to head. +1 to include '/'
  
  node_mkdir(n, head);
  
  node* dir = follow(n, head);
  node* newFile = (node*)malloc(sizeof(node));
  strcpy(newFile->name, last++); //++ to skip '/'
  newFile->flag = 'F';
  insert(dir, newFile);
}

void fs_touch(FS* fs, char* P){
  Tree* t = (Tree*) fs;
  node* currentORroot;
  if(P[0] == '/'){
    currentORroot = t->root;
    P = P++;
  }
  else currentORroot = t->cwd;

  if(safeToCreate(currentORroot, P)) node_touch(currentORroot, P);
  else printf("Touch Error: Cannot create file.\n");
}





void fs_cd(FS* fs, char* P){
  Tree* t = (Tree*) fs;
  if(follow(t->cwd, P) == NULL) printf("Change Dir Error: Cannot change working directory.\n");
  else follow(t->cwd, P);
}




void node_rm(node* n, char* P){
  //if(n == NULL) return n;
  node* childToRemove = follow(n, P);
  
  char* last = strrchr(P, '/');//get last localName (including '/') in P
  char head[PATHMAX - strlen(last)]; //= ""; //to store entire path except last
  
  strncpy(head, P, strlen(P)-strlen(last)); //write to head
  
  node* pathHead = follow(n, head);
  
  //case if child to remove is the only one in child list
  if(pathHead->firstchild == childToRemove && childToRemove->nextsibling == NULL){
    childToRemove->parent = NULL;
    free(childToRemove);
  }
  //if child is first in child list
  else if(pathHead->firstchild == childToRemove){
    pathHead->firstchild = childToRemove->nextsibling;
    free(childToRemove);
  }
  //if child to remove is in middle or last
  else {
    node* prev;
    node* next;
    
    next = pathHead->firstchild; //error: incompatible types when assigning to type ‘node’ from type ‘struct node *’
    prev = NULL;
    //is in middle
    while(next != NULL){//error: invalid operands to binary != (have ‘node’ and ‘void *’)
      //is call order done right here?
      prev->nextsibling = next; //error: incompatible types when assigning to type ‘struct node *’ from type ‘node’
      next = childToRemove->nextsibling; //error: incompatible types when assigning to type ‘node’ from type ‘struct node *’
      free(childToRemove);
      return;
    }
    //is last
    prev->nextsibling = NULL;
    free(childToRemove);
  }
}

void fs_rm(FS* fs, char* P){
  Tree* t = (Tree*) fs;
  
  node* currentORroot;
  node* result;
  
  if(P[0] == '/'){
    result = follow(t->root, P++);
    currentORroot = t->root;
    P = P++;
  }
  else{
    result = follow(t->cwd, P);
    currentORroot = t->cwd;
  }
  
  
  if(result == NULL){
    printf("Remove Error: Cannot remove file or directory.\n");
    return;
  }
  if(result->firstchild != NULL){
    printf("Remove Error: directory \'%s\' is not empty", PWD(result, P));
    return;
  }
  node_rm(currentORroot, P);//remove file or direc
}

/*void node_rmf(node* n, char* P){

}*/

void fs_rmf(FS* fs, char* P){
  Tree* t = (Tree*) fs;
  
  node* currentORroot;
  node* result;
  
  if(P[0] == '/'){
    result = follow(t->root, P++);
    currentORroot = t->root;
    P = P++;
  }
  else {
    result = follow(t->cwd, P);
    currentORroot = t->cwd;
  }
  
  if(result == NULL){
    printf("Remove Error: Cannot remove file or directory.");
    return;
  }
  //else if path can be followed, recursively delete subtree under directory
  node_rm(currentORroot, P);
}


void find(node* n, char* name){
  if(n == NULL) return;
  if(strcmp(n->name, name) == 0) printf("%c %s", n->flag, PWD(n, n->name));
  if(n->firstchild != NULL) find(n->firstchild, name);
  if(n->nextsibling != NULL) find(n->nextsibling, name);
}

void fs_find(FS* fs, char* name){
  Tree* t = (Tree*) fs;
  find(t->root, name);
}

void destroy(FS* fs){
  Tree* t = (Tree*) fs;
  free(t);
}
