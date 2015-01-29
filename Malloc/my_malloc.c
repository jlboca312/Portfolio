#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "my_malloc.h"

#define HEAP_SIZE 2048
#define ALLOCATED 1
#define FREE 0

void *next_header(void *p);

static int COUNT = 0;
static char *myHeap;

struct header{
  unsigned int length: 29;
  unsigned int NOT_USED: 2;
  unsigned int allocated: 1;
};

struct footer{
  unsigned int length: 29;
  unsigned int NOT_USED: 2;
  unsigned int allocated: 1;
};

void *next_header(void *p){
  void *ptr;
  struct header *hp = (struct header*)p;

  ptr = hp;
  ptr = hp + hp->length;
  return ptr;
}

void *my_malloc(size_t size){
  void *p;
  struct header *hp;
  struct footer *fp;

  if(size == 0)
    return NULL;
    
  if(size > HEAP_SIZE){
    errno = ENOMEM;
    return NULL;
  }
  
  //aligns the size to 8                                                                                                                 
  while((size % 8) != 0){
    size++;
  }
  
  if(COUNT == 0){
    
    //creates heap of size 2048
    myHeap = malloc(HEAP_SIZE);
    COUNT++;
  
    //creates header
    hp = (struct header*)(&myHeap[0]);
    hp->length = size + 8;
    hp->allocated = ALLOCATED;
    
    //creates footer
    fp = (struct footer*) hp + (hp->length - 4);
    fp->length = size + 8;
    fp->allocated = ALLOCATED;

    //sets pointer to writable space
    p = hp + 4;
  
  }else{

    hp = (struct header*)(&myHeap[0]);

    //makes hp point to next block of memory
    hp = (struct header*)next_header((struct header*)hp);
    
    //creates header
    hp->length = size + 8;
    hp->allocated = ALLOCATED;

    //creates footer
    fp = (struct footer*)hp + (hp->length - 4);
    fp ->length = size +8;
    fp->allocated = ALLOCATED;

    //sets pointer to writable space
    p = hp + 4;
 
  }
  return p;
    
  
}

void my_free(void *p, int size){
  struct header* hp = (struct header*)p;

  //aligns the size to 8
  while((size % 8) != 0){
    size++;
  }

  //create new header
  hp->length = size +8;
  hp->allocated = FREE;

  //makes p point to space after footer
  p = hp + (hp->length - 4); 
}

int main(void){
  char *hello;

  hello = (char*)my_malloc(6);
  strcpy(hello, "Hello");
  printf("%s\n", hello);

  my_free(hello, 6);  // parameter '6' is the amount allocated by my_malloc
 
  hello = (char*)my_malloc(8);
  strcpy(hello, "Goodbye");
  printf("%s\n", hello);

  my_free(hello, 8);

  printf("'%s'\n", hello);  //shows that nothing is in hello because it was freed by the above line


  return 0;
}

