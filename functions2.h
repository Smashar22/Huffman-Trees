#include <stdio.h>

typedef struct list list;
typedef struct tree tree;
typedef struct hufftable hufftable;

struct list {                     
  tree *ptr_struct;
  list *next;
};

struct tree {                        
  char *character;
  int total_bits;
  int level;
  char *LR_bit;
  char *bit_p;
  tree *child_L, *child_R;
};

struct hufftable {                 
  char letter;
  char *bit_p;
  int bit_p_length;
  int ascii_int;
  int L_count;
};

/* ----- functions for READ FILE and BUFFER ----- */
void print_buff(char *buff);
int table_size(hufftable *table);
char *read_file(int argc, char *argv[], char *source);
hufftable *buffer_scan(char *input, hufftable symb[]);

/* ----- functions for LINKED LIST ----- */
int list_size(list *head);
void list_min(hufftable *table, int *min, int *min_ref);
list *list_create(tree *entry);
list *add_to_list(list *head, list *curr, tree *entry);
list *list_insert(list *head, list *curr, list *val);
list *list_fill(list *head, list *curr, hufftable *sym_ptr, int min_index);
list *list_remove(list *head);

/* ----- functions for BINARY TREE ----- */
int tree_depth(tree *top);
int count_rights(tree *ptr);
void r_counter(tree *ptr, int *counter);
int node_count(tree *ptr, int count);
int diag_lvl_nodes(tree *root, int diag_lvl, int count2);
tree *node_create(int ascii,char* root,int LR_bit,int total_bits,tree *child_L,tree *child_R);
tree *tree_build(list *head, list *curr, int size);
tree **sort_length(tree **ptstr, int length);
tree **sort_bitP(tree **ptstr, int length);
int pr_lvl(tree *root,tree **array,int diag_lvl,int dLvl_cnt,int index,char *l_Br);
int r_dashes(tree *ptr, int count);

/* ----- functions for BIT PATTERNS ----- */
int cntAll_0s(char* input);
int cnt1s_front(char* input);
int bitP_0_earlier(char* first, char* second);
int bitP_0_end(char* input);
char* gen_bPatt(tree *root, char* target, char* bit_pattern);
hufftable *bPat(tree *root, tree *curr, char* bPatt, hufftable *ref);

/* ----- functions for PRINT ----- */
void print_tree(tree* root);
void print_level(tree* root, int level);
int print_rights(tree *R, int prints, char *left_b);
int sort_print(tree **array, int arr_size);






