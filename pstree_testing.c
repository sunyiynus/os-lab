#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <dirent.h>
#include <errno.h>



#define VERSION "pstree 0.0.1 copyright by uttep.\n"

#define pdebug(str) printf("\33[32m[Debug]\33[0m: %s \n", #str)
#define psdebug(str) printf("\33[32m[Debug]\33[0m: %s var is %s\n", #str, str)
#define pwarning(str) printf("\33[31m[Warning]\33[0m: %s \n", #str)
#define pinfo(str) printf("[Info]: %s \n", #str)

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


treenode* tree_get_node(int pid, int ppid, const char* procname);
int tree_destory(treenode** self);
int tree_free_por(treenode* node);

/* RAII in c */

/* Char Str free */
#define auto_sfree __attribute__ ((cleanup(sfree)))
#define auto_treefree __attribute__ ((cleanup(treefree)))

__attribute__((always_inline))
    inline void sfree(void* str) {
        if( !str ) return;
        free( *(void**)str );
#ifdef DEBUG
        pdebug(Free OK!);
#endif
    }

__attribute__((always_inline))
    inline void treefree(void* tree) {
        if( !tree ) return;
        tree_free_por( *(treenode**)tree );
    }

/* paraper output format */
static short outputstate = 0;
static int indent = 0;
void set_output_state(int argc, char* argv[]);
int print_tree_t(const treenode* const root, int indent);
void print_version();



/* read dir */
bool is_pid(const char* str);
int read_pid(treenode**);
char* get_value_from_line(const char* const str);
FILE* open_proc_status(const char* restrict path_name);
int read_proc_pid(FILE* proc_status);
int read_proc_ppid(FILE* proc_status);
char* read_proc_name(FILE* proc_status);
char* get_proc_info(FILE* proc_status, const char* key);


/* construct pstree */
treenode* construct_tree(treenode* list);
int tree_add_son_pstree(treenode* tree_node_ptr, treenode* node_ptr);
bool cmp_ppid(void* a, void* b);
bool cmp_pid(void* a, void* b);
treenode* list_pop_front(treenode* list);
int list_push_back(treenode* list, treenode* node);
treenode* list_search_pid(treenode* list, int pid);
int sort_brother(treenode* pstree);
void print_node(const treenode* const node);


/* Queue Data Structure */
struct _node;
struct _queue;
typedef struct _node node;
typedef struct _queue Queue;
typedef void*(*qfpf_t)(Queue*);
typedef void (*qfpb_t)(Queue*, void*);
typedef bool (*qfie_t)(Queue*);
typedef void* (*free_t)(void*);

void QueueInitialize(Queue** queue);
void QueueDestory(Queue** queue, free_t data_free);
void* _queue_pop_front(Queue* queue);
void _queue_push_back(Queue* self, void* data);
bool _queue_is_empty(Queue* self);

/* Queue IMP */
struct _node {
    void* data;
    struct _node* next;
    //struct _node* prev;
};


struct _queue{
    node* head;
    node* tail;
    qfpf_t pop_front;
    qfpb_t push_back;
    qfie_t is_empty;

    int size;
};



void QueueInitialize(Queue** queue) {
    if( !queue ) return;
    if( *queue != NULL ) return;
    (*queue) = calloc(1,sizeof(Queue));

    if( (*queue) == NULL ) return; 
    
    (*queue)->head = calloc(1, sizeof(node));
    if((*queue)->head == NULL) {
        free(*queue);
        return;
    }
    (*queue)->head->data = NULL;
    (*queue)->head->next = NULL;
    (*queue)->tail = (*queue)->head;
    
    (*queue)->push_back = _queue_push_back;
    (*queue)->pop_front = _queue_pop_front;
    (*queue)->is_empty = _queue_is_empty;
    (*queue)->size = 0;
}


void QueueDestory(Queue** queue, free_t data_free ) {
    if ( *queue == NULL ) return;

    if( (*queue)->head == NULL || (*queue)->tail == NULL ) {
        return;
    }
    void* tmp_data =  NULL;
    while( (tmp_data = (*queue)->pop_front(*queue)) != NULL) {
        data_free( tmp_data );
#ifdef DEBUG
        pdebug(Queue: Free data OK!);
#endif
    }
    
    free( (*queue)->head );
    (*queue)->head = NULL;
    (*queue)->tail = NULL;
    free(*queue);
    *queue = NULL;
}



void _queue_push_back(Queue* self, void* data) {
    if(self == NULL || data == NULL) return;
    node* tmp_ptr = calloc(1, sizeof(node));
    if(tmp_ptr == NULL) exit(-1);
    tmp_ptr->data = data;
    self->tail->next = tmp_ptr;
    self->tail = tmp_ptr;
    (self->size)++;
    printf("Queue Size: %d \n", self->size);
}


void* _queue_pop_front(Queue* self) {
    if( self->head == self->tail ) return NULL;

    node* tmp_ptr = self->head->next;
    
    if( self->head->next == self->tail ) {
        self->tail = self->head;
    }

    self->head->next = tmp_ptr->next;
    tmp_ptr->next = NULL;
    void* tmp_data = tmp_ptr->data;
    tmp_ptr->data = NULL;
    free(tmp_ptr);
    (self->size)--;
    printf("Queue Size: %d \n", self->size);
    return tmp_data;
}


bool _queue_is_empty(Queue* self) {
    if( !self ){
        exit(-1);
    };
    if( self->head == NULL || self->tail == NULL ) {
        exit(-1);
    }
    if(self->head == self->tail || self->size == 0) return TRUE;
    else return FALSE;
}


/* Queue Testing */
#ifdef DEBUG

void* free_int(void* data) {
    free(data);
}

void TEST_queue() {
    Queue* queue = NULL;
    QueueInitialize(&queue);
    assert(queue!= NULL);
    assert(queue->head != NULL);
    assert(queue->tail != NULL);
    assert(queue->tail == queue->head);
    int* data = calloc(1, sizeof(int));
    *data = 11;
    queue->push_back(queue, (void*)data);
    data = calloc(1, sizeof(int));
    *data = 10;
    queue->push_back(queue, (void*)data);
    
    data = (int*) queue->pop_front(queue);
    printf(" Queue Out: %d\n ", *data);
    free(data);
    assert(queue->is_empty(queue) == FALSE);
    data = (int*) queue->pop_front(queue);
    assert(queue->is_empty(queue) == TRUE);
    free(data);
    

    QueueDestory(&queue, free_int);
}

#endif



/******** Data Struct IMP  ********/

#define CHECK_PTR_NULL(ptr) if( !ptr ) return NULL
#define CHECK_PTR_VALUE(ptr) return ( ptr != NULL)?0:-1


int node_free(treenode* node) {
    if( !node ) return 0;
    if( node->proc_name ) {
        free(node->proc_name);
    }
    free(node);
#ifdef DEBUG
    printf(".");
#endif
    return 0;
}


treenode* tree_get_node(int pid, int ppid, const char* procname) {
    if( pid < 0 || ppid <0 || procname == NULL ) return NULL;
    treenode* tmp_ptr = NULL;
    if( (tmp_ptr = calloc(1,sizeof(treenode))) == NULL ) return tmp_ptr;

    tmp_ptr->right = NULL;
    tmp_ptr->left = NULL;
    tmp_ptr->pid = pid;
    tmp_ptr->ppid = ppid;
    tmp_ptr->proc_name = calloc(strlen(procname) + 1, sizeof(char));
    if( tmp_ptr->proc_name == NULL ) {
        free(tmp_ptr);
        return NULL;
    }
    strncpy(tmp_ptr->proc_name, procname, strlen(procname));
    return tmp_ptr;
}

int tree_initialize(treenode** self) {
    treenode* tmp_ptr = NULL;
    if( (tmp_ptr = calloc(1,sizeof(treenode))) == NULL ) return -1; 
    tmp_ptr->right = NULL;
    tmp_ptr->left = NULL;
    tmp_ptr->pid = 0;
    tmp_ptr->ppid = 0;
    *self = tmp_ptr;
    return 0;
}


int tree_destory(treenode** self) {
    if ( (*self) ) {
        tree_free_por(*self);
        (*self) = NULL;
        return 0;
    }
    return -1;
}

int tree_free_por(treenode* node) {
#ifndef OTHER_METHOD_FREE_TREE
    if( !node ) return -1;
    tree_free_por(node->left);
    tree_free_por(node->right);
    node_free(node);
#else
    while(1) {
        if( node == NULL ) {
            break;
        }
        else if( node->left != NULL ) {
            tree_free_por(node->left);
            node->left = NULL;
        }
        else if( node->right != NULL ) {
            tree_free_por(node->right);
            node->right = NULL;
        }
        else {
            free(node);
            return 0;
        }
    }
#endif
    return 0;
}

#ifdef TREE_LIST_FREE
int list_free(treenode* node) {
    treenode* tmp_itr;
}
#endif

#ifdef DEBUG
/* unit test */



void TEST_tree_get_node_normal(void) {
    printf("\n-------%s \n", __func__);
    treenode* tmp = tree_get_node(11, 12, "smba");
    assert(tmp != NULL);
    assert(tmp->pid == 11);
    assert(tmp->ppid == 12);
    assert(tmp->right == NULL);
    assert(tmp->left == NULL);
    psdebug(tmp->proc_name);
    pdebug(Failed testing);
    tree_destory(&tmp);
    assert(tmp == NULL);
}


#endif

/******** Read Proc fs  ********/

#define TREE_LIST_FREE

treenode* get_proc() {
  DIR* dirp = opendir("/proc/");
  if( !dirp ) {
      perror("Get proc file failed...");
      exit(-1);
  }

  struct dirent* dp;

  treenode* list = NULL;
  tree_initialize(&list);
  treenode* itr_list = list;

  for(;;) {
      errno = 0;
      dp = readdir(dirp);
      if( !dp ) break;
      psdebug(dp->d_name);

      if( strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 ) continue;

      if( is_pid(dp->d_name) ) {
          FILE* proc_status = open_proc_status(dp->d_name);
          int pid = read_proc_pid(proc_status);
          int ppid = read_proc_ppid(proc_status);
          auto_sfree char* p_name = get_proc_info(proc_status, "Name");
          fclose(proc_status);
          
          itr_list->right = tree_get_node(pid, ppid, p_name);
          itr_list = itr_list->right;
      }
  }
#ifdef TREE_LIST_FREE
  list->left = itr_list;
#endif
  closedir(dirp);
  return list;
}

bool is_pid(const char* str) {
    if( !str ) return FALSE;
    int pid = atoi(str);
    if( pid > 0 ) return TRUE;
    else return FALSE;
}


FILE* open_proc_status(const char* restrict pid) {
    char path[256] = "/proc/";
    if( strncat(path, pid, sizeof(pid)) == NULL) {
        // TODO error handle
    }
    if( strncat(path, "/status", 7) == NULL) {
        // TODO error handle
    }
    FILE* status = fopen(path, "r");
    if( !status ) {
        pwarning(Open file failed...);
    }
    return status;
}


char* get_proc_info(FILE* proc_status, const char* key) {
    fseek(proc_status,0, SEEK_SET);
    char linebuff[1024] = {0};
    while( fgets(linebuff, sizeof(linebuff), proc_status) ) {
        char* first_occ = NULL;
        int valid_len = strcspn(linebuff, "\n");
        linebuff[valid_len] = '\0';
        psdebug(linebuff);
        if ( (first_occ = strstr(linebuff, key)) == NULL) {
            pdebug(Str not found...); 
        } else {
           break;
        }
    }
    return get_value_from_line(linebuff);
}


int read_proc_ppid(FILE* proc_status) {
    auto_sfree char* result = get_proc_info(proc_status, "PPid");
    if ( result == NULL ) {
        return -1;
    }
    int tmp_ppid = atoi(result);
    return tmp_ppid;
}


int read_proc_pid(FILE* proc_status) {
    auto_sfree char* result = get_proc_info(proc_status, "Pid");
    if ( result == NULL ) {
        return -1;
    }
    int tmp_pid = atoi(result);
    return tmp_pid;
}



char* get_value_from_line(const char* const str) {
    if( !str ) return NULL;
    char tmp_srcbuf[1024] = {0};
    strncpy(tmp_srcbuf, str, strlen(str));

    psdebug(tmp_srcbuf);
    const char dem[] = ": ";
    char* ptr_arr[5] = {0};
    char* tmp_token = strtok(tmp_srcbuf, dem);
    psdebug(tmp_token);
    ptr_arr[0] = tmp_token;
    for( int i = 1; tmp_token; ++i ) {
        tmp_token = strtok(NULL, dem);
        ptr_arr[i] = tmp_token;
        psdebug(ptr_arr[i]);
        psdebug(tmp_token);
    }

    if( ptr_arr[1] ) {
        char* tmp = calloc(strlen(ptr_arr[1]) + 1, sizeof(char));
        if( !tmp ) return NULL;
        strncpy( tmp, ptr_arr[1], strlen(ptr_arr[1]));
        psdebug(tmp);
        return tmp;
    } else {
        return NULL;
    }
}


#ifdef DEBUG
/* unit test */

void debug_PrintProcess(const treenode* node) {
    printf("% s : %d \n", node->proc_name, node->pid);
}

void debug_PrintAllProcess(const treenode* tree) {
    treenode** tmp_itr = &(tree->right);
    while( *tmp_itr != NULL ) {
        debug_PrintProcess( *tmp_itr );
        tmp_itr = &( (*tmp_itr)->right );
    }
}

#define print_i(istr) printf("%d\n", istr)

void TEST_read_proc_info(void) {
    /*
    printf("\n-------%s \n", __func__);
    assert( is_pid("12") == TRUE );
    assert( is_pid("0") != TRUE );
    assert( is_pid("JIACK") != TRUE );

    FILE* tmp_file = open_proc_status("10");
    assert(tmp_file != NULL);
    char* info = get_value_from_line("Name:   10");
    assert(info != NULL);
    free(info);
    info = get_value_from_line("Namesame");

    free(info);
    assert(info == NULL);
    info = get_proc_info(tmp_file, "Pid");
    free(info);
    info = get_proc_info(tmp_file, "SigQ");
    free(info);
    int tmp = read_proc_pid(tmp_file);
    printf("%d\n", tmp);
    fclose(tmp_file);
    tmp_file = open_proc_status("890");
    assert(tmp_file == NULL);
*/
    auto_treefree treenode* tmp_node_list = get_proc();
    assert(tmp_node_list != NULL);
    debug_PrintAllProcess(tmp_node_list);
    
    //tree_destory(&tmp_node_list);
}


#endif


#ifdef DEBUG
/* unit test */
/* testing RAII imp by __attribute__(())
 *
 */

void Test_Raii() {
    auto_treefree treenode* tmp;
    tree_initialize(&tmp);
    tmp->right = tree_get_node(1,1,"Pname");
}


#endif


/*********** Construct tree IMP **********/

typedef bool(*cmp_t)(void*, void*);


bool cmp_pid(void* a, void* b) {
  treenode* tmp_node = (treenode*)a;
  int* pid = (int*)b;
  return ( tmp_node->pid == *pid)? 1:0;
}


bool cmp_ppid(void* a, void* b) {
  treenode* tmp_node = (treenode*)a;
  int* ppid = (int*)b;
  return ( tmp_node->ppid == *ppid)? TRUE:FALSE;
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
    if( !(list->right) ) return NULL;
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


treenode* list_search_pid(treenode* list, int pid) {
    if ( !list || ( pid <= 0)) return NULL;
    treenode** itr = &(list->right);
    treenode* result = NULL;
    while( (*itr)->pid != pid && (*itr) ) {
        itr = &((*itr)->right);
    }
    if( *itr == NULL ) {
        return *itr;
    } 

    result = *itr;
    *itr = result->right;
    result->right = NULL;
    
    return result;
}


void* nop_free(void* data) {
    //
}


treenode* tree_search(treenode* root, cmp_t cmp, int* id) {
    Queue* queue = NULL;    
    QueueInitialize(&queue);

    queue->push_back(queue, (void*)root);

    while( !queue->is_empty(queue) ) {

        treenode* tmp_node_ptr = queue->pop_front(queue);
        print_node(tmp_node_ptr);
        
        if( tmp_node_ptr->right != NULL ) {
            queue->push_back(queue, tmp_node_ptr->right); 
        }
        if( tmp_node_ptr->left != NULL ) {
            queue->push_back(queue, tmp_node_ptr->left); 
        }
        if( cmp(tmp_node_ptr, id) ) {
            QueueDestory(&queue, nop_free);
            return tmp_node_ptr;
        }
    }
    
    QueueDestory(&queue, nop_free);
    return NULL;
}


treenode* construct_tree(treenode* list) {
  if( !list ) return NULL;
  treenode* root = NULL;
  if ( (root =list_search_pid(list, 1)) == NULL) {
      /* some oprand */
      perror("can't find 1 process...");
      return NULL;
  }

  if ( (root->right = list_search_pid(list, 2)) == NULL) {
      /* some oprand */
      perror("can't find 1 process...");
      return NULL;
  }
  

  while( list->right != NULL) {
      treenode* tmp_node = list_pop_front(list);
      treenode* tmp_tree_node = NULL; 
    
      if( tmp_node->ppid == 0 ) {
          free(tmp_node);
          continue;
      }

      if( (tmp_tree_node = tree_search(root, cmp_pid, &(tmp_node->ppid))) == NULL) {
          list_push_back(list, tmp_node);
          continue;
      }
      tree_add_son_pstree(tmp_tree_node, tmp_node); 
  }

  return root;
}

#ifdef DEBUG

void print_node(const treenode* const node) {
    if( !node ) return;
    printf("\33[32m Tree Node:\33[0m %s Pid: %d, PPid: %d\n",
            /*strlen(node->proc_name), */ node->proc_name, node->pid, node->ppid);
}

void TEST_ConstructTree() {
    auto_treefree treenode* tree = get_proc();
    //tree->left = NULL;
    int i = 1;
    const treenode* const result = tree_search(tree, cmp_pid, &i);
    print_node(result);
    /*
    treenode* tmp = list_search_pid(tree, 1);
    assert(tmp != NULL);
    assert(tmp->pid == 1);
    print_node(tmp);
    free(tmp);
    */
    treenode* tmp = NULL;

    /* 
     * List Pop Front Test
    for( int i = 0; i < 200; ++i ) {
    tmp = list_pop_front(tree);
    assert( tmp != NULL );
    printf("%d ", strlen(tmp->proc_name));
    print_node(tmp);
    list_push_back(tree, tmp);
    }

    */
    auto_treefree treenode* newtree = construct_tree(tree);
    
    print_tree_t(newtree, 0);
    
    /* remove double pointing */
    tree->left = NULL;
}

#endif

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
    /*
  char * buff = strcat()
  if( !root ) return -1；

  for(treenode* itr = root->left; itr->right != NULL; ) {
      treenode* next = itr->right;
      if( itr->right != NULL ) {
          print_tree(next,  )
      }
      itr = itr->right;
  }
  */
}

int print_node_t(const treenode* const node, int indent) {
    if( !node ) return -1;
    for( int i = 0; i < indent; ++i ) {
        printf(" ");
    }
    printf("{%s %d %d}\n", node->proc_name, node->pid, node->ppid);
    return 0;
}


int print_tree_t(const treenode* const root, int indent) {
    if( !root ) return -1;
    print_node_t(root, indent);

    print_tree_t(root->left, indent+2);
    print_tree_t(root->right, indent);
    return 0;
}





/* test */


int main(int argc, char* argv[]) {
#ifdef DEBUG
    //TEST_tree_get_node_normal();
    //TEST_read_proc_info();
    //TEST_queue();
    TEST_ConstructTree();
#endif
  
}
