#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

char greeting[] = "Hello there\n1. Receive wisdom\n2. Add wisdom\nSelection >";
char prompt[] = "Enter some wisdom\n";
char pat[] = "Achievement unlocked!\n";
char secret[] = "secret key";

int infd = 0;                                               /* stdin */
int outfd = 1;                                              /* stdout */

#define DATA_SIZE 128                                       /* define a variable "DATA_SIZE" equal to 128 bytes */

typedef struct _WisdomList {                                /* define a structure named WisdomList */
  struct  _WisdomList *next;                                /* declare a pointer to the next item */
  char    data[DATA_SIZE];                                  /* declare an array named "data" able to hold 128 chars */
} WisdomList; 

struct _WisdomList  *head = NULL;                           /* set the first pointer to NULL */

typedef void (*fptr)(void);                                 /* define a "pointer" variable fptr */
                                                            /* a void pointer can point to the address of any variable type */
void write_secret(void) {
  write(outfd, secret, sizeof(secret));                     /* write secret ("secret key") of "size" sizeof(secret) to stdout */
  return;
}

void pat_on_back(void) {
  write(outfd, pat, sizeof(pat));                           /* write pat ("Achievement unlocked!") to stdout */
  return;
}

void get_wisdom(void) {
  char buf[] = "no wisdom\n";                               /* declare an array called buf and initialize it with "no wisdom\n" */
  if(head == NULL) {                                        /* if the first pointer points to NULL (no stored wisdom) */
    write(outfd, buf, sizeof(buf)-sizeof(char));            /* write the contents of buf (i.e., "no wisdom") to stdout */
  } else {                                                  /* if head is not NULL (i.e., some wisdom has been stored) */
    WisdomList  *l = head;                                  /* store the head pointer in "l" */
    while(l != NULL) {                                      /* as long as the pointer is not NULL */
      write(outfd, l->data, strlen(l->data));               /* find the data "l" is pointing to and write it to stdout */
      write(outfd, "\n", 1);                                /* write a newline char */
      l = l->next;                                          /* find the next pointer and store it in l */
    }
  }
  return;
}

void put_wisdom(void) {
  char  wis[DATA_SIZE] = {0};                               /* declare an array of chars named wis (128 bytes) and initialize all to 0 */
  int   r;                                                  /* declare an integer with name r */

  r = write(outfd, prompt, sizeof(prompt)-sizeof(char));    /* write the prompt to "Enter some wisdom" to stdout */
  if(r < 0) {
    return;
  }
 
  r = (int)gets(wis);                                       /* read a string from the "wis" array, cast to an int, and store it in r */ 
  if (r == 0)                                               /* declare an integer named "r" */
    return;

  WisdomList  *l = malloc(sizeof(WisdomList));              /* return a pointer "l" to the memory allocated for a struct of type and size WisdomList */

  if(l != NULL) {                                           /* if the pointer "l" does not point to NULL */
    memset(l, 0, sizeof(WisdomList));                       /* set a number of bytes (designated by sizeof(WisdomList)) and pointed to by "l" equal to 0 */
    strcpy(l->data, wis);                                   /* copy the contents of "wis" into "data" (the item pointed to by "l") */
    if(head == NULL) {                                      /* if no wisdom was entered before (head == NULL) */
      head = l;                                             /* change "head"pointer to "l" */
    } else {
      WisdomList  *v = head;                                /* get a pointer named "v" and assign it the address "head" is pointing to */                                
      while(v->next != NULL) {                              /* as long as the "next" pointer is not NULL */
        v = v->next;                                        /* move to the next pointer */
      }                                                     /* the above serves to traverse the linked list to the end */
      v->next = l;                                          /* at the end of the linked list, assign the pointer "l" as the "next" pointer */
    }
  }

  return;
}

fptr  ptrs[3] = { NULL, get_wisdom, put_wisdom };           /* declare a pointer array and initialize it with three values (NULL and pointers to the functions) */

int main(int argc, char *argv[]) {                          /* start the main function */

  while(1) {                                                /* start an infinite loop */
      char  buf[1024] = {0};                                /* declare an array called "buf" with 1024 bytes (i.e. 8 WisdomList items), initialize with all 0's */
      int r;                                                /* declare an integer named r */
      fptr p = pat_on_back;                                 /* declare a pointer "p" and initialize it with the address of return of the function "pat_on_back" */
      r = write(outfd, greeting, sizeof(greeting)-sizeof(char));/* write the greeting to stdout */
      if(r < 0) {                                           /* if write() returns -1 (error) */
        break;                                              /* break */
      }
      r = read(infd, buf, sizeof(buf)-sizeof(char));        /* read the contents of "buf" */
      if(r > 0) {                                           /* if something was read (r equals the number of bytes read) */
        buf[r] = '\0';                                      /* set the last item of buf to a null terminator char */
        int s = atoi(buf);                                  /* convert contents of buf into an integer, store in "s" */
        fptr tmp = ptrs[s];                                 /* use "s" (should be 1 or 2) as the index to assign a pointer to tmp */
        tmp();                                              /* call the function pointed to by tmp */
      } else {
        break;                                              /* if nothing was read in, break */
      }
  }

  return 0;
}
