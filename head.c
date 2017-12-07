#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];

void head(int fd, char *name, int x) {
  int n,line=0;
  int i;
  
  while((n = read(fd, buf, sizeof(buf))) > 0 && line<x) {
    for(i=0; i<=n && line<x; i++) {
      if(buf[i]!='\n') {
        printf(1,"%c",buf[i]);
      }
      else {
      	printf(1,"\n");
      	line++;
      }
    }
  }
  if(n<0) {
    printf(1, "HEAD: READ ERROR\n");
    exit();
  }
  printf("file : %s\n", name);
}

int main(int argc, char *argv[]) {
  if(argc <= 1) {
    printf(1,"HEAD : ARGUMENT KURANG\n");
    exit();
  }
  else if(argc==2) {
    
  }
  else {
    printf(1,"HEAD : SYNTAX ERROR\n");
    exit();
  }
}
