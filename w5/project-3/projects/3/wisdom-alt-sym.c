#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <klee/klee.h>

char greeting[] = "Hello there\n1. Receive wisdom\n2. Add wisdom\nSelection >";
char prompt[] = "Enter some wisdom\n";
char pat[] = "Achievement unlocked!\n";
char secret[] = "secret key";

int infd = 0; /* stdin */
int outfd = 1; /* stdout */

#define DATA_SIZE 128

typedef struct _WisdomList {
  struct  _WisdomList *next;
  char    data[DATA_SIZE];
} WisdomList; 

struct _WisdomList  *head = NULL;

typedef void (*fptr)(void);

void write_secret(void) {
  write(outfd, secret, sizeof(secret));
  return;
}

void pat_on_back(void) {
  write(outfd, pat, sizeof(pat));
  return;
}

void get_wisdom(void) {
  char buf[] = "no wisdom\n";
  if(head == NULL) {
    write(outfd, buf, sizeof(buf)-sizeof(char));
  } else {
    WisdomList  *l = head;
    while(l != NULL) {
      write(outfd, l->data, strlen(l->data));
      write(outfd, "\n", 1);
      l = l->next;
    }
  }
  return;
}

//simulates symoblic outside input
int sym_gets(char *buf) {
  int r = 0;
  char *ptr = buf;
  while(1) {
    r = klee_range(0, 255, "input");
    *ptr = r;
    ptr++;
    if(r == 0) {
      break;
    }
  }

  return r;
}

void put_wisdom(void) {
  char  wis[DATA_SIZE] = {0}; 
  int   r;
  char  *ptr;

  r = write(outfd, prompt, sizeof(prompt)-sizeof(char));
  if(r < 0) {
    return;
  }

  r = sym_gets(wis); 
  if (r == 0)
    return;

  WisdomList  *l = malloc(sizeof(WisdomList));

  if(l != NULL) {
    memset(l, 0, sizeof(WisdomList));
    strcpy(l->data, wis);
    if(head == NULL) {
      head = l;
    } else {
      WisdomList  *v = head;
      while(v->next != NULL) {
        v = v->next;
      }
      v->next = l;
    }
  }

  return;
}

fptr  ptrs[2] = { get_wisdom, put_wisdom };

int main(int argc, char *argv[]) {

  //while(1) {
      char  buf[20] = {0};
      int r;
      fptr p = pat_on_back;
      r = write(outfd, greeting, sizeof(greeting)-sizeof(char));
      if(r < 0) {
        return 0;
      }
      //r = read(infd, buf, sizeof(buf)-sizeof(char));
      klee_make_symbolic(buf, sizeof(buf), "buf");
      r = klee_range(-1, sizeof(buf), "r");
      if(r > 0) {
        buf[r] = '\0';
        int s = klee_range(0, 255, "v");
        if(s == 0 || s == 1) {
          fptr tmp = ptrs[s];
          tmp();
        }
      } else {
        return 0;
      }
  //}

  return 0;
}
