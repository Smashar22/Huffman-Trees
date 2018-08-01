#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "functions1.h"

#define BYTE 8
#define ASCII_R (122 - 40) /* range of Ascii symbols read from text */
#define ASCII_S 40
#define ASCII_END 122
#define ASCII_0 48
#define ASCII_1 49

int main(int argc, char *argv[])
{
  int size, depth, min_index, min;
  int i, j, k, prints;
  char *bit_patt, *buffer = NULL;
  hufftable *sym_ptr, symbols[ASCII_R];
  tree *root;
  list *head = NULL;
  list *curr = NULL;

  i = j = k = prints = min = 0;
  root = malloc(sizeof(tree));

  buffer = read_file(argc, argv, buffer); /* acquire Txt Input */
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

  printf("\n\n\t - - - PRINT TABLE - - - \n");
  print_huff(sym_ptr);
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
  /* counts the occurences of each char, and then populates huffmantable */
  int i, j, count;

  for(j=0; j<ASCII_R ;j++) {
    symb[j].letter = (ASCII_S + j);
    symb[j].ascii_int = (ASCII_S + j);
    symb[j].bit_p = 0;
    symb[j].bit_p_length = 0;

    for (i=0, count=0; input[i]; i++) {
      count += (input[i] == (ASCII_S + j));
    }
    symb[j].L_count = count;
    count = 0;
  }
  return symb; 
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
void print_buff(char *buff) {
  /* code: prints the contents of the buffer */
  printf("\n\t - - - BUFFER CONTENTS - - - \n");
  printf("\n%s\n\n", buff);
}
void print_huff(hufftable *ptr) {
  /* code: prints contents of the hufftable */
  int a, total = 0;
  printf("\n  Char\tBit Pattern\t\tLength * Letter Count\n");
  for(a=0; a<ASCII_R; a++) {
    if(ptr[a].L_count != 0) {
      printf("\n  '%c':\t[%s]\t\n", ptr[a].letter, ptr[a].bit_p);
      printf("\t\t\t\t( [%d]\t *  %d)\n", ptr[a].bit_p_length, ptr[a].L_count);      
    }
  }
  printf("\n");
  for(a=0; a<ASCII_R; a++) {
    if(ptr[a].L_count != 0) {
      total = total + ((ptr[a].L_count)*(ptr[a].bit_p_length)/BYTE);
    }
  }
  printf("\n Total Bytes = %d\n\n", total);
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







