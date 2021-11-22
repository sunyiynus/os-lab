#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>


#define VERSION "pstree 0.0.1 copyright by uttep.\n"

#define DEBUG


/* Useful typedef */

#ifndef TURE
typedef enum {FALSE = 0, TRUE} bool;
#else
typedef int bool;
#endif



/* date struct imp */

typedef struct _treenode{
  int pid;
  int ppid;
  char *proc_name;
  struct _treenode* left;
  struct _treenode* right;
} treenode;


int tree_init(treenode** self);
int tree_dist(treenode** self);
treenode* tree_get_node(int pid, int ppid, const char* procname);
int tree_pre_order_traversal(treenode* node);

typedef int (*cmp_t)(void* a, void* b);
treenode* tree_find(treenode* self, cmp_t cmp_func, void* cmp_data);


/* paraper output format */
static short outputstate = 0;
static int indent = 0;
void set_output_state(int argc, char* argv[]);
void print_tree();
void print_node(treenode* node);
void print_version();


/* read dir */
int is_pid(const char* str);
int read_pid(treenode**);
FILE* open_proc_status(const char* restrict path_name);
int read_proc_pid(FILE* proc_status);
int read_proc_ppid(FILE* proc_status);
int read_proc_name(FILE* proc_status);
int close_proc_status(FILE* proc_status);


/* construct pstree */
int construct_tree(treenode* list);
int tree_add_son_pstree(treenode* tree_node_ptr, treenode* node_ptr);
int cmp_ppid(void* a, void* b);
int cmp_pid(void* a, void* b);
treenode* list_pop_front(treenode* list);
int list_push_back(treenode* list, treenode* node);
treenode* find_pid_proc(treenode* list, int pid);
int sort_brother(treenode* pstree);



/******** Data Struct IMP  ********/

#define CHECK_PTR_NULL(ptr) if( !ptr ) return -1
#define CHECK_PTR_VALUE(ptr) return ( ptr != NULL)?0:-1


int tree_init(treenode** self) {
    CHECK_PTR_NULL(self);
    (*self) = calloc(1, sizeof(treenode));
    if( *self == NULL ) return -1;
    (*self)->right = NULL;
    (*self)->left = NULL;
    (*self)->pid = 0;
    (*self)->ppid = 0;
    (*self)->proc_name = NULL;
    return 0;
}

treenode* tree_get_node(int pid, int ppid, const char* procname) {
    if( pid <= 0 || ppid <=0 || procname == NULL ) return NULL;
    treenode* tmp_ptr = NULL;
    if( (tmp_ptr = calloc(1,sizeof(treenode))) == NULL ) return tmp_ptr;
    tmp_ptr->right = NULL;
    tmp_ptr->left = NULL;
    tmp_ptr->pid = pid;
    tmp_ptr->ppid = ppid;
    tmp_ptr->proc_name = calloc(strlen(procname) + 1, sizeof(char));
    if( tmp_ptr->proc_name == NULL ) {
    free_goto:
        free(tmp_ptr);
        return NULL;
    }
    if (strncpy(tmp_ptr->proc_name, procname, strlen(procname) + 1) == -1) {
        goto free_goto;
    }
    return tmp_ptr;
}

int tree_pre_order_traversal(treenode* node) {
    if( !node ) return -1;
    print_node(node);
    tree_pre_order_traversal(node->left);
    tree_pre_order_traversal(node->right);
    return 0;
}



/******** Read Proc fs  ********/

treenode* get_proc() {
  DIR* dirp = opendir("/proc/");
  if( !dirp ) {
      perror("Get proc file failed...");
      exit(-1);
  }

  struct dirent* dp;

  treenode* list = tree_get_node(0,0,NULL);
  treenode* itr_list = list;

  for(;;) {
      errno = 0;
      dp = readdir(dirp);
      if( !dp ) break;

      if( strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 ) continue;

      if( is_pid(dp->d_name) ) {
          FILE* proc_status = open_proc_status(dp->d_name);
          int pid = read_proc_pid(proc_status);
          int ppid = read_proc_ppid(proc_status);
          char* p_name = read_proc_name(proc_status);
          close_proc_status(proc_status);
          
          itr_list->right = tree_get_node(pid, ppid, p_name);
          itr_list = itr_list->right;
      }
  }
}


FILE* open_proc_status(const char* restrict pid) {
    char path[256] = "/proc/";
    if( strncat(path, pid, sizeof(path)) == -1) {
        // TODO error handle
    }
    if( strncat(path, "/status", sizeof(path)) == -1) {
        // TODO error handle
    }
    FILE* status = fopen(path, "r");
    if( !status ) {
        // TODO error handle
    }
    return status;
}


char* get_proc_info(FILE* proc_status, const char* key) {
    char linebuff[1024] = {0};
    while( fgets(linebuff, sizeof(linebuff), proc_status) ) {
        char* first_occ = NULL;
        if ( (first_occ = strchr(linebuff, key)) == NULL) {
            // TODO
        } else {
           break;
        }
    }
    return get_value_from_line(linebuff);
}


int read_proc_ppid(FILE* proc_status) {
    char* result = get_proc_info(proc_status, "PPid");
    if ( result == NULL ) {
        return -1;
    }
#ifndef SAFE
    return strtol(result);
#else
    return atol(result);
#endif
}

char* get_value_from_line(char* str) {
    CHECK_PTR_NULL(str);
    const char* dem = ": ";
    char* ptr_arr[5] = {0};
    ptr_arr[0] = strtok(str, dem );
    for( int i = 1; ptr_arr[i]; ++i ) {
        ptr_arr[i] = strtok(NULL, dem);
    }

    if( ptr_arr[1] ) {
        char* tmp = calloc(strlen(ptr_arr[1]) + 1, sizeof(char));
        if( !tmp ) return NULL;
        if (strncpy( tmp, ptr_arr, strlen(ptr_arr[1]) + 1) == -1 ) {
            free(tmp);
            // TODO
        } else {
            return tmp;
        }

    }
}



/******** Output IMP  ********/

void set_output_state(int argc, char* argv[]) {
    int optchar = 0;
    char opterr;
    char* b_opt_arg;
    while( (optchar = getopt(argc, argv, "pnV")) != -1 ) {
      switch(optchar) {
          case 'n':
              outputstate += 1;
              break;
          case 'p':
              outputstate += 2;
              break;
          case 'V':
              printf(VERSION);
              break;
          default:
              printf("pstree [-npV] [-n] [-p] [-V] \n");
              break;
      }
    }
}

/* Example:
 * |-+- 01111 ...        CPPN 01111 ...
 * | |-+= 0001
 * | \-+=   01112 ...    B LPPG 01112 ...
 * |   |--= 01113 ...    B   CSSG 01113 ...
 * |   |--= 01113 ...    B   CSSG 01113 ...
 * |   \--= 01114 ...    B   LSSG 01114 ...
 * \--- 01115 ...        LSSN 01115 ...
 */



#define PRINT_FORMAT "%d %s \n"


// TODO 这儿怎么回事？？老是写不通
int print_tree(treenode* root, const char* buff, const char* prefix, const char* children_prefix){
  char * buff = strcat()
  if( !root ) return -1；

  for(treenode* itr = root->left; itr->right != NULL; ) {
      treenode* next = itr->right;
      if( itr->right != NULL ) {
          print_tree(next,  )
      }
      itr = itr->right;
  }
}


/*********** Construct tree IMP **********/

int cmp_pid(void* a, void* b) {
  treenode* tmp_node = (treenode*)a;
  int* pid = (int*)b;
  return ( tmp_node->pid == *pid)? 1:0;
}

int cmp_ppid(void* a, void* b) {
  treenode* tmp_node = (treenode*)a;
  int* ppid = (int*)b;
  return ( tmp_node->ppid == *ppid)? 1:0;
}

int tree_add_son_pstree(treenode* tree_node_ptr, treenode* node_ptr) {
    if( !tree_node_ptr || !node_ptr ) return -1;
    treenode** modify_itr = &(tree_node_ptr->left);
    while( *modify_itr ) {
        modify_itr = &((*modify_itr)->right);
    }
    *modify_itr = node_ptr;
    return 0;
}


treenode* list_pop_front(treenode* list) {
    if( !list ) return NULL;
    treenode* tmp_node = list->right;
    list->right = tmp_node->right;
    tmp_node->right = NULL;
    return tmp_node;
}

int list_push_back(treenode* list, treenode* node) {
    if( !list || !node ) return -1;
    list->left->right = node;
    list->left = node;
    return 0;
}

treenode* find_pid_proc(treenode* list, int pid) {
    if ( !list || ( pid <= 0)) return NULL;
    treenode** itr = &(list->right);
    while( (*itr)->right != NULL ) {
        if( (*itr)->pid == pid ) return (*itr);
    }
    return NULL;
}

int construct_tree(treenode* list) {
  if( !list ) return -1;
  treenode* root = NULL;
  if ( (root =find_pid_proc(list, 1)) == NULL) {
      /* some oprand */
      perror("can't find 1 process...");
  }

  while( list->right != NULL) {
      treenode* tmp_node = list_pop_front(list);
      treenode* tmp_tree_node = NULL; 
      if( (tmp_tree_node = tree_find(list, cmp_ppid, &(tmp_node->ppid))) == NULL) {
          list_push_back(list, tmp_node);
      }
      tree_add_son_pstree(tmp_tree_node, tmp_node); 
  }

  return 0;
}


#ifdef DEBUG
/* unit test */


#endif



/* test */


int main(int argc, char* argv[]) {
  set_output_state(argc, argv);
  
}
