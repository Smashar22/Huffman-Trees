#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "functions2.h"

#define BYTE 8
#define ASCII_R (122 - 65) /* range of Ascii symbols read from text */
#define ASCII_ALPHA 26
#define ASCII_S 65
#define ASCII_END 122
#define ASCII_CAP 32 /* difference between lower/upper case */
#define ASCII_0 48
#define ASCII_1 49

int main(int argc, char *argv[])
{
  static int diag_nodes, index;
  int size, depth, min_index, min;
  int i, j, k, prints, total_nodes;
  char *bit_patt, *buffer = NULL;
  char l_Br[ASCII_R*2] = "";
  hufftable *sym_ptr, symbols[ASCII_R];
  tree *root, **print_dLvl;
  list *head = NULL;
  list *curr = NULL;

  i = j = k = prints = min = 0;
  root = malloc(sizeof(tree));

  buffer = read_file(argc, argv, buffer); /* acquire txt input */
  print_buff(buffer);
  sym_ptr = symbols;
  sym_ptr->bit_p = malloc(sizeof(char) * (ASCII_R+1));
  sym_ptr = buffer_scan(buffer, symbols); /* input acquired */

  list_min(sym_ptr, &min, &min_index); /* establish sort Link List and Tree */
  curr = list_create(node_create(ASCII_S+min_index, NULL, 0, min, NULL, NULL));
  head = curr;
  curr = list_fill(head, curr, sym_ptr, min_index);
  size = list_size(head);
  root = tree_build(head, curr, size); /* complete the Binary Tree */
  depth = tree_depth(root);
  bit_patt = malloc(sizeof(char) * (depth+1));
  bit_patt[0] = '\0';
  sym_ptr = bPat(root, root, bit_patt, sym_ptr);

  printf("\n\n - - - PRINT BASIC BINARY TREE - - - \n\n");
  print_tree(root);
  
  printf("\n\n - - - PRINT VISUAL BINARY TREE - - - \n\n");
  total_nodes = node_count(root, 0);
  print_dLvl = (tree**)malloc(sizeof(tree*) *total_nodes);
  for(j = 0; j < total_nodes; j++) { /* Init ptrs for diagonal level array */
    print_dLvl[j] = malloc(sizeof(tree));
    print_dLvl[j]->character = malloc(sizeof(char) * (ASCII_R+1));
    print_dLvl[j]->LR_bit = malloc(sizeof(char) * (ASCII_R+1));
  }

  while(prints < total_nodes) {
    k = 0;
    diag_nodes = diag_lvl_nodes(root, i, diag_nodes) - diag_nodes;
    if(i == 0) {
      prints = diag_nodes;
    }
    pr_lvl(root, print_dLvl, i, diag_nodes, index, l_Br); /*array unsorted*/
    index = 0;
    if(i > 0) {
      prints = prints + sort_print(print_dLvl, diag_nodes); /*sorts & prints*/
    }
    printf("\n");
    i++;
  }

  printf("\n\n");
  return 0;
}

/* ----------------------- *Input* --------------------------- */
char* read_file(int argc, char *argv[], char *source) {
  /* checks file and reads it into a buffer */
  FILE *fp;
  long size;
  size_t length;
  
  if(argc !=2) {
    printf("Error: please input file.txt to run program\n");
    exit(1);
  }
  if((fp = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Opening file error. File is NULL\n");
    exit(2);
  }
  if (fp != NULL) {
    if (fseek(fp, 0L, SEEK_END) == 0) {
      size = ftell(fp);
      source = malloc(sizeof(char) * (size + 1));
      if(!source) {
        printf("Attaining memory for buffer failed");
        exit(3);
      }
      fseek(fp, 0L, SEEK_SET);
      length = fread(source, sizeof(char), size, fp);
      if (ferror(fp) != 0) {
        fputs("Error reading file", stderr);
      } 
      else {
        source[length++] = '\0';
      }
    }
    fclose(fp);
  }
  return source;
}
hufftable *buffer_scan(char *input, hufftable symb[]) {
  /* counts the occurences of each char, and then populates hufftable */
  int i, j, count;

  for(j=0; j<ASCII_R ;j++) {
    symb[j].letter = (ASCII_S + j);
    symb[j].ascii_int = (ASCII_S + j);
    symb[j].bit_p = 0;
    symb[j].bit_p_length = 0;
    symb[j].L_count = 0;

    if(j < ASCII_ALPHA) {
      for (i=0, count=0; input[i]; i++) {
        count += (input[i] == (ASCII_S + j));
        count += (input[i] == (ASCII_S + j + ASCII_CAP));
      }
      symb[j].L_count = count;
      count = 0;
    }
  }
  return symb; 
}
void print_buff(char *buff) {
  /* code: prints the contents of the buffer */
  printf("\n\t - - - BUFFER CONTENTS - - - \n");
  printf("\n%s\n\n", buff);
}
int table_size(hufftable *table) {
  int a, count, size = 0;

  for(a = 0; a < ASCII_R; a++) {
    count = table[a].L_count;
    if(count) {
      size++;
    }
  }
  printf("\nTable has %d entries in it\n", size);
  return size;
}
/* ----------------------- *Linked List* --------------------- */
list *list_create(tree *entry) {
  list *ptr;

  ptr = (list*)malloc(sizeof(list));
  if(ptr == NULL){
    printf("\n Node creation failed \n");
    return NULL;
  }
  ptr->ptr_struct = entry;
  ptr->next = NULL;
  return ptr;
}
list *list_fill(list *head, list *curr, hufftable *sym_ptr, int min_index) {

  int i, count;
  tree *tmp;

  for(i = 0; i < ASCII_R; i++) {
    count = sym_ptr[i].L_count;
    if(count && (i != min_index) ) {
      tmp = node_create(ASCII_S + i, NULL, 0, count, NULL, NULL);
      curr = add_to_list(head, curr, tmp);
    }
  }
  return curr;
}
list *add_to_list(list* head, list* curr, tree* entry) {
  
  list *ptr;

  if(head == NULL){
    printf("\n List is not created, cannot add node. \n");
    return NULL;
  }

  ptr = (list*)malloc(sizeof(list));
  if(ptr == NULL){
    printf("\n Node creation failed \n");
    return NULL;
  }
  ptr->ptr_struct = entry;
  ptr->next = NULL;
  curr = list_insert(head, curr, ptr);

  return curr;
}
void list_min(hufftable *table, int *min, int *min_ref) {
  /* code: pass by reference, returns two minimum variables for the list */
  int b, a = 0;

  do{
    if(table[a].L_count != 0) {
      *min = table[a].L_count;
    }
    a++;
  } while(*min == 0); /* create a temp minimum value */

  for(b = 0; b < ASCII_R; b++) {
    if((table[b].L_count > 0) && (table[b].L_count < *min)) {
      *min = table[b].L_count;
      *min_ref = b;
    }
  } /* assign the correct minimum value */
}
int list_size(list *head) {
  /* note: only callable after head node created */
  int size = 1;
  list *ptr = head;

  if(ptr == NULL) {
    printf("\nHead node is NULL. Create a head node.\n");
  }
  while(ptr->next != NULL) {
    size++;
    ptr = ptr->next;
  }
  return size;
}
list *list_insert(list *head, list *curr, list *val) {
  list *ptr2; /* current location of pointer chasing */
  int inserted = 0;

  ptr2 = (list*)malloc(sizeof(list));
  ptr2 = head;

  while(inserted == 0){
    if(ptr2->next == NULL) {
      curr->next = ptr2->next = val;
      curr = ptr2 = val;
      inserted++;
    }
    if(!inserted) {
      if(ptr2->next->ptr_struct->total_bits >= val->ptr_struct->total_bits) {
        val->next = ptr2->next;
        ptr2->next = val;
        inserted++;
      }
    }
    if(ptr2->next != NULL) {
      ptr2 = ptr2->next;
    }
  }
  return curr;
}
list *list_remove(list *head) {
  /* code: removes front two entries, and sets head to correct position */
  list *ptr;

  ptr = (list*)malloc(sizeof(list));
  ptr = head;

  if(head == NULL) {
    printf("Linked List is Empty");
    return head;
  }
  if(list_size(head) == 2) {
    head = head->next;
    ptr->next = NULL;
    printf("Last two in list");
    return head;
  }
  head = head->next;
  ptr->next = NULL;
  ptr = head;
  head = head->next;
  ptr->next = NULL;
  return head;
}
char* list_top2(list *head, int *new_bits) {
  /* Combines top two linked list total_bits and char strings */

  list *ptr;
  char* string;
  char* string2;

  ptr = (list*)malloc(sizeof(list));
  ptr = head;

  *new_bits = *new_bits + ptr->ptr_struct->total_bits;
  string = malloc(sizeof(char) * (ASCII_R+1));
  strcpy(string, ptr->ptr_struct->character);

  ptr = ptr->next;
  *new_bits = *new_bits + ptr->ptr_struct->total_bits;
  string2 = malloc(sizeof(char) * (ASCII_R+1));

  strcpy(string2, ptr->ptr_struct->character);
  strcat(string, string2);

  return string;
}
/* ----------------------- *Bit Patterns* --------------------- */
int bitP_0_earlier(char* first, char* second) {
  /* code : compare two strings of same length. string with 0's earlier: */
  /* code: returns 1 if swap needed, 0 if not */
  int i, j;
  int first_0s, second_0s;

  first_0s = second_0s = 0;
  j = strlen(first);

  if(strlen(first) == strlen(second)) {
    for(i = 0; i < j; i++) {
      if(first[i] == ASCII_1 && second[i] == ASCII_1) {
        if(first_0s <= second_0s) {
          return 1; /* second needs to be swapped */
        }
      }
      else {
        if(first[i] == ASCII_0) {
          first_0s++;
        }
        if(second[i] == ASCII_0) {
          second_0s++;
        }
      }
    }
  }
  return 0;
}
int bitP_0_end(char* input) {
  /* code: check end of string has a zero */
  int length;
  char dest[ASCII_R+1] = "";

  if(input) {
    length = strlen(input);
    strncpy(dest, input, length);
    if(dest[length - 1] == ASCII_0) {
        return 1;
    }
  }
  return 0;
}
int cntAll_0s(char* input) {
  /* Counts number of 0's contained in string */
  int i, cnt, answer_bool, length;
  char dest[ASCII_R+1] = "";

  i = cnt = answer_bool = 0;

  if(input) {
    length = strlen(input);
    strncpy(dest, input, length);
    while(!answer_bool) {
      if(dest[i] == '\0') {
        answer_bool++;
      }
      if(answer_bool == 0) {
        if(dest[i] == ASCII_0) {
          cnt++;
          i++;
        }
        else {
          i++;
        }
      }
    }
  }
  return cnt;
}
int cnt1s_front(char* input) {
  /* code: counts number of 1's at front of string */
  int i, cnt, answer_bool, length;
  char dest[ASCII_R+1] = "";

  i = cnt = answer_bool = 0;

  if(input) {
    length = strlen(input);
    strncpy(dest, input, length);
    while(!answer_bool) {
      if(dest[i] == '\0' || dest[i] == ASCII_0) { /* end of string */
        answer_bool++;
      }
      if(answer_bool == 0) {
        if(dest[i] == ASCII_1) {
          cnt++;
          i++;
        }
        else {
          i++;
        }
      }
    }
  }
  return cnt;
}
hufftable *bPat(tree *root, tree *curr, char* bPatt, hufftable *ref) {
  /* Function fills Hufftable and Binary Tree with bit patterns */
  tree *ptr;
  int a;
  ptr = curr;

  if(ptr) {
    bPat(root, ptr->child_L, bPatt, ref);
    bPat(root, ptr->child_R, bPatt, ref);
    bPatt = gen_bPatt(root, ptr->character, bPatt);
    /* Input into hufftable and binary_tree */
    if(strncmp(ptr->LR_bit, "R", 1) != 0) {
      for(a=0; a<ASCII_R; a++) {
        if(ref[a].letter == *ptr->character && !ref[a].bit_p) {
          ref[a].bit_p = malloc(sizeof(char) * (ASCII_R+1));
          strcpy(ref[a].bit_p, bPatt);
          ref[a].bit_p_length = strlen(bPatt);
        }
      }
      ptr->bit_p = malloc(sizeof(char) * (ASCII_R+1));
      strcpy(ptr->bit_p, bPatt);
      bPatt[0] = '\0';
    }
  }
  else {
    return ref;
  }
  return ref;
}
char* gen_bPatt(tree *root, char* target, char* bit_pattern) {
  /* Using root and end branch target letter, generate a bit pattern */
  tree *ptr2;
  ptr2 = root;

  if(strncmp(ptr2->LR_bit, "R", 1) != 0) {
    strcat(bit_pattern, ptr2->LR_bit);
  }

  if(strcmp(ptr2->character, target) == 0) {
    return bit_pattern;
  }
  else {
    if(strstr(ptr2->child_R->character, target) == NULL) {
      gen_bPatt(ptr2->child_L, target, bit_pattern);
    }
    else {
      gen_bPatt(ptr2->child_R, target, bit_pattern);
    }
    return bit_pattern;
  }
}
/* ----------------------- *Print* --------------------- */
void print_level(tree* root, int level) {
  /* Prints a specified level */
  if(!root) {
    return;
  }
  if(level == 1) {
    if(strlen(root->character) > 1) {
      printf(" # {%s} ", root->bit_p);
    }
    else {
      printf(" %s {%s} ", root->character, root->bit_p);
    }
  }
  else if (level > 1) {
    print_level(root->child_L, level-1);
    print_level(root->child_R, level-1);
  }
}
void print_tree(tree* root) {
  /* Prints the binary tree in a basic form */

  int total_depth, i;
  total_depth = tree_depth(root) + 1;

  for(i = 0; i <= total_depth; i++) {
    print_level(root, i);
    printf("\n\n");
  }
}
int print_rights(tree *R, int prints, char *left_b) {
  /* code: prints right branchs from a root node */
  /* also, generates the left branch & space symbols for the following line */

  int i, count = 0;
  char *a = "|";
  char *b = " ";

  if(R != NULL) {    
    if(R->child_R == NULL) {
      printf("%s", R->character);
      printf(" ");
      strcat(left_b, b);
      strcat(left_b, b);
      prints++;
    }
    else {
      printf("#");
      strcat(left_b, a);
      count = r_dashes(R->child_L, count)*2;
      for(i = 0; i <= count; i++) {  
        printf("-");
        strcat(left_b, b);
      }
      prints++;
      prints = print_rights(R->child_R, prints, left_b);
    }
  }
  return prints;
}
int sort_print(tree **array, int arr_size) {
  /* code: sorts array of pointers and prints them */
  /* also, generates the left branch & space symbols for the following line */
  int i, j, k, l, size, count = 0;
  int prints = 0;
  char left_b[ASCII_R*2] = "";
  char *a = "|";
  char *b = " ";
  tree **ptr;
  tree *ptr2;

  size = arr_size;

  for(i = 0; i < arr_size; i++) {
    if(array[i]->bit_p == NULL) {
      size = size - 1;
    }
  }
  ptr = sort_length(array, size);
  ptr = sort_bitP(array, size);

  for(i = 0; i < size; i++) { /* Print entries */
    prints++;
    if((i == 0) && (int)strlen(array[i]->bit_p) != array[i]->level) {/*pre-spaces*/
      l = cnt1s_front(array[i]->bit_p) * 2;
      for(j = 0; j < l; j++) {
        printf(" ");
        strcat(left_b, b);
      }
    }
    if(array[i]->child_R != NULL) {
      printf("#");
      strcat(left_b, a);
      count = r_dashes(array[i]->child_L, count)*2;
      if (strlen(array[i]->child_R->character) == 1) {
        printf("-");
        strcat(left_b, b);
      }
      else {
        for(k = 0; k <= count; k++) {  
          printf("-");
          strcat(left_b, b);
        }
      }
      ptr2 = array[i];
      prints = print_rights(ptr2->child_R, prints, left_b);
    }
    else {
      printf("%s ", array[i]->character);
      strcat(left_b, b);
      strcat(left_b, b);
    }
  }
  printf("\n%s", left_b); /* prints the following line of left branchs */

  return prints;
}
/* ----------------------- *Binary Tree* --------------------- */
int tree_depth(tree *top) {
  tree *ptr = top;
  int left, right;

  if(ptr == NULL) {
    return -1;
  }
  else {
    left = tree_depth(ptr->child_L);
    right = tree_depth(ptr->child_R);

    if(left > right) {
      left++;
      return left;
    }
    else {
      right++;
      return right;
    }
  }
}
tree *node_create(int ascii,char* root,int LR_bit,int total_bits,tree *child_L,tree *child_R) {
  /* code: create binary tree nodes */
  tree *node;
  char str[2] = "\0";
  char str2[2] = "\0";

  if(LR_bit) {
    str2[0] = ASCII_1;
  }
  else {
    str2[0] = ASCII_0;
  }

  str[0] = ascii;
  node = malloc(sizeof(tree));
  if(node == NULL){
    printf("\n Node is NULL and could not be created\n");
    return NULL;
  }
  if(ascii) {
    node->character = malloc(sizeof(char) * (ASCII_R+1));
    strcpy(node->character, str);
  }
  else {
    node->character = malloc(sizeof(char) * (ASCII_R+1));
    strcpy(node->character, root);
  }
  node->LR_bit = malloc(sizeof(char) * (ASCII_R+1));
  strcpy(node->LR_bit, str2);
  node->total_bits = total_bits;
  node->child_L = child_L;
  node->child_R = child_R;
  return node;
}
tree *tree_build(list *head, list *curr, int size) {
  /* code: builds out the tree */
  char* str;
  int sum, i;
  list *ptr1, *ptr2;
  tree *tmp2;

  tmp2 = malloc(sizeof(tree));
  tmp2->character = malloc(sizeof(char) * (ASCII_R+1));
  tmp2->LR_bit = malloc(sizeof(char) * (ASCII_R+1));

  for(i = 0; i < (size-1); i++) {
    sum = 0;
    ptr1 = ptr2 = head;
    ptr2 = ptr2->next;

    str = list_top2(head, &sum); 
    tmp2 = node_create(0, str, 0, sum, ptr1->ptr_struct, ptr2->ptr_struct); 

    ptr1->ptr_struct->LR_bit = "0";
    ptr2->ptr_struct->LR_bit = "1";

    if(i == (size-2)) {   /* Root entry doesn't have a binary LR_bit */
      tmp2->LR_bit = "R";
    }
    curr = add_to_list(head, curr, tmp2);
    head = list_remove(head);
  }

  if((int)strlen(tmp2->character) != size) {
    printf("\nBinary Tree Build Incomplete - Top Root Not Inserted\n");
  }
  return tmp2;
}
tree **sort_length(tree **ptstr, int length) {
  /* code: swap positions so lengths incrementing are 1, 2, 3 etc */

  tree *temp;
  int h, i;

  for(h = 0; h < length - 1; h++) {
    for(i = 0; i < length - h - 1; i++) {
      if(strlen(ptstr[i]->bit_p) > strlen(ptstr[i+1]->bit_p)) {
        temp = ptstr[i];
        ptstr[i] = ptstr[i + 1];
        ptstr[i + 1] = temp;
      }
    }
  }
  return ptstr;
}
tree **sort_bitP(tree **ptstr, int length) {
  /* code: swap positions of strings of same length */

  tree *temp;
  int h, i;

  for(h = 0; h < length - 1; h++) {
    for(i = 0; i < length - h - 1; i++) {
      if(bitP_0_earlier(ptstr[i]->bit_p, ptstr[i+1]->bit_p) == 1) {
        temp = ptstr[i];
        ptstr[i] = ptstr[i + 1];
        ptstr[i + 1] = temp;
      }
    }
  }
  return ptstr;
}
int r_dashes(tree *ptr, int count) {
  /* code: calculates right offset dashes */
  /* code: input is a ptr->child_L from root */

  if(ptr) {
    count = r_dashes(ptr->child_L, count);
    if(ptr->child_R != NULL) {
      if(bitP_0_end(ptr->bit_p)) {
        count = count + (count_rights(ptr));
      }
      count = r_dashes(ptr->child_R, count);
    }
  }
  return count;
}
void r_counter(tree *ptr, int *counter) {
  /* code: utility function that counts the right branchs */

  if(ptr) {
    if(ptr->child_R != NULL) {
      (*counter)++;
      r_counter(ptr->child_R, counter);
    }
  }
}
int count_rights(tree *ptr) {
  /* code: Outputs count of right branchs */
  int counter = 0;
  int cnt;

  r_counter(ptr, &counter);

  cnt = counter;
  return cnt;
}
int node_count(tree *ptr, int count) {
  /* Counts total nodes in Binary Tree */

  if(ptr) {
    count++;
    count = node_count(ptr->child_L, count);
    count = node_count(ptr->child_R, count);
  }
  return count;
}
int diag_lvl_nodes(tree *root, int diag_lvl, int count2) {
  /* code: returns count of how many nodes are in a diag level */
  /* diag_lvl == 0's in bit_pattern */

  int zeros = 0;
  tree *ptr;
  ptr = root;

  if(ptr) {
    zeros = cntAll_0s(ptr->bit_p);
    if(zeros == diag_lvl) {
      count2++;
    }
    count2 = diag_lvl_nodes(ptr->child_R, diag_lvl, count2);
    count2 = diag_lvl_nodes(ptr->child_L, diag_lvl, count2);
  }
  else {
    return count2;
  }
  return count2;
}
int pr_lvl(tree *root,tree **array,int diag_lvl,int dLvl_cnt,int index,char *l_Br){
  /* code: For printing, gets an array of nodes of the same diagonal level */
  /* also, generates the left branch & space symbols for the following line */
  tree *ptr;
  int bitP_0s;
  int i, mark, count = 0;
  char *a = "|";
  char *b = " ";

  ptr = root;

  if(ptr) {
    if(diag_lvl == 0) {
      ptr->level = 0;
      if(strlen(ptr->character) > 1) {
        printf("#");
        strcat(l_Br, a);
        count = r_dashes(ptr->child_L, count)*2;
        if (ptr->child_R != NULL && strlen(ptr->child_R->character) == 1) {
          count = 0;
        }
        for(i = 0; i <= count; i++) {  
          printf("-");
          strcat(l_Br, b);
        }
      }
      else {
        printf("%s \n", ptr->character);
        strcat(l_Br, b);
        strcat(l_Br, b);
        printf("%s ", l_Br);
      }
      index = pr_lvl(ptr->child_R, array, diag_lvl, dLvl_cnt, index, l_Br);
    }
    else {
      bitP_0s = cntAll_0s(ptr->bit_p);
      ptr->level = 0;
      if(ptr->bit_p != NULL) {
        ptr->level = bitP_0s;
        mark = strlen(ptr->bit_p) - 1;
        if(bitP_0s == diag_lvl && ptr->bit_p[mark] != '1') {
          if(index <= dLvl_cnt) {
            array[index] = ptr;
            index++;
          }
          else {
            index = 0;
            array[index] = ptr;
            index++;
          }
        }
      }
    index = pr_lvl(ptr->child_R, array, diag_lvl, dLvl_cnt, index, l_Br);
    index = pr_lvl(ptr->child_L, array, diag_lvl, dLvl_cnt, index, l_Br);
    }
  }
  return index;
}

