#ifndef ___Element_DEFINITION_HEADER_
#define ___Element_DEFINITION_HEADER_

#include<stdio.h>
#include<string.h>
#include"fs.h"

typedef int bool;
#define TRUE 1
#define FALSE 0

#define PATHMAX 1001

typedef struct _node node;
struct _node{
    char name[21];//used to be char*
    char flag;
    node* parent;
    node* firstchild;
    node* nextsibling;
  
};

//returns position of pointer from \0 to end path
static char* PWD(node* current, char* p_start){
    if(current == NULL){
      p_start[0] = '\0';
      return p_start;
    }
    if(current->parent == NULL){
      p_start[0] = '/';
      p_start[1] = '\0';
      return p_start + 1;// why return \0?
    } 
    else{
      char* end = PWD(current->parent, p_start);
      snprintf(end, PATHMAX-(end-p_start), "/%s", current->name); //p_start was start
      return strlen(end) + end;//is this doing the same thing?
    }
}


//returns a subpath that doesn't include the first dir and stores the first dir in var localname
//assume in this function that P is relative, so modify passed in P if neccess
//should be good
static char* getLocalName(char* P, char* localname){
    while(*P != '/' && *P != '\0'){
      *localname = *P;
      localname++;
      P++;
    }
    *localname = '\0';
    if(*P = '/')
      P++;
    return P;
}


//sets current parent node to matching name in child directories or file. 
//should be good.
static node* getChild(node* current, char* name){
  if(current == NULL) return NULL;
  
  node* child = current->firstchild;
  if(child == NULL) return NULL;
  
  if(strcmp(child->name, name) == 0) return child;
  
  while(child->nextsibling != NULL){
    if(strcmp(child->nextsibling->name, name) == 0)
      return child->nextsibling;
    else
      child = child->nextsibling;
  }
  return NULL;
}




//(t->root, path++)
//follow sets a node n to path p_start. Assume all paths passed in are relative
//should be good
static node* follow(node* current, char* p_start){
    char localName[21];
    if(current == NULL) return current;
    if(p_start == '\0') return current;
    
    char* remaining = getLocalName(p_start, localName);//this and getChild will take care of the recursive case
    
    if(strcmp(localName, "..") == 0) return follow(current->parent, remaining);
    if(strcmp(localName, ".") == 0) return follow(current, remaining);
    
    node* c = getChild(current, localName);
    if(!c){
        printf("Path Error: directory \'%s\' does not exist.", PWD(current, p_start));// DO use PWD to print error messages w/ absolute paths
        return NULL;
    }
    if(c->flag == 'F' && remaining != NULL) /* do I even need to print an error?*/ return NULL;
    return follow(c, remaining);
}

//same here (t->root, path++) or (n, relative path for n)
static bool safeToCreate(node* n, char* P){
    //look similar to follow
    //follow the path and verify that each localname in the path is a file or directory
    //I'm thinking there's a var type that checks whether the last token in path
    //contains a period or not. file if period or dir if no period
    char* last = strrchr(P, '/');
    char prefixOfPath[21];
  
    char* remaining = getLocalName(P, prefixOfPath);
    node* c = getChild(n, prefixOfPath);
    
    if(!c){//if the prefix in the path exists...must be dir or file
      if(remaining == NULL){
        //if(type == c->flag) return FALSE;
        //else
      }
      else {
        if(c->flag == 'D'){//if dir
          //go to next prefix starting from last
          safeToCreate(c, remaining);
        }
        else/*(c->flag == 'F')*/{//if file or maybe . or ..
          //printf("Path Error: Cannot create sub-directory content, \'%s\'", PWD(current, p_start));
          // files can't have any children.
          return FALSE;
        }
      }
    }
    //if a child in the path cannot be matched but will be created in mkdir or touch and it's descendants
    //still safe to create.
    else return TRUE;
    //if(current == NULL) return FALSE;
    //if(p_start == '\0') return FALSE;
    //the token at the end of path, check if it already exists or not. if
    //it exists, return false.
}




static int charCompare(char left, char right){//may need static
    if(left == right) return 0;
    if(left == '.') return -1;
    if(right == '.') return 1;
    if(left == '-') return -1;
    if(right == '-') return 1;
    if(left == '_') return -1;
    if(right == '_') return 1;
    
    //below is for everything else whose ascii values do not need to be redefined
    if(left < right) return -1;
    else if(left > right) return 1;
}

//Frank's help, compares string
static int nameCompare(char* left, char* right){//may need static
    if(left == NULL && right == NULL) return 0;
    if(left == NULL && right != NULL) return -1;
    if(left != NULL && right == NULL) return 1;
    int cmp = charCompare(*left, *right);
    if(cmp == 0) return nameCompare(left++, right++);
    else return cmp;
}

// . < - < _ < 0-9 < A-Z < a-z
static int compare(node* left, node* right){
  if(left->flag == right->flag) 
      return nameCompare(left->name, right->name);
  else if(left->flag == 'D') return -1;
  else return 1;
}


//for mkdir and touch, inserts a new child somewhere within a child list (depending on order) and sets new child's properties
static void insert(node* parent, node* newchild){
  newchild->parent = parent;
  newchild->firstchild = NULL;
  
  //case when parent has no children
  if(parent->firstchild == NULL){
    newchild->nextsibling = NULL;
  }
  //case if parent already has child(ren)
  else {
    int result = compare(newchild, parent->firstchild);
    //if new child is first in child list
    if(result < 0){
      newchild->nextsibling = parent->firstchild;
      parent->firstchild->nextsibling = NULL;
    }
    //if new child is somewhere in the middle or last
    else if (result > 0){
      node* prev;
      node* cur;
      cur = parent->firstchild;
      prev = NULL;
      
      //if new child is in middle
      while(cur != NULL){
        if(compare(newchild, cur) < 0){
          prev->nextsibling = newchild; 
          newchild->nextsibling = cur;
          return;
        }
      }
      //if new child is last in child list
      prev->nextsibling = newchild;
      newchild->nextsibling = NULL;
    }
  }
}

#endif
