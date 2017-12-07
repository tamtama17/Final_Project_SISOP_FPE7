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
  printf(1,"file : %s\n", name);
}

int main(int argc, char *argv[]) {
  int fd,i;
  
  if(argc <= 1) {
    printf(1,"HEAD : ARGUMENT KURANG\n");
    exit();
  }
  else if(argc==2) {
    if(strcmp(argv[1],"--help")==0) {
      printf(1,"[HELP] Penggunaan :\n\n");
      printf(1," head [FILE]       Menampilkan 10 baris pertama\n");
      printf(1,"                   dari sebuah FILE ke standard output.\n\n");
      printf(1," head [N] [FILE]   Menampilkan N baris pertama\n");
      printf(1,"                   dari sebuah FILE ke standard output.\n\n");
      printf(1," head --help       Menampilkan bantuan penggunaan ini.\n\n");
      exit();
    }
    for(i = 1; i < argc; i++) {
        if((fd = open(argv[i], 0)) < 0) {
          printf(1, "HEAD : CAN'T OPEN %s\n", argv[i]);
          exit();
        }
        head(fd, argv[i], 10);
        close(fd);
    }
  }
  
  else if(argc==3) {
    int x = atoi(argv[1]);
    for(i = 2; i < argc; i++) {
        if((fd = open(argv[i], 0)) < 0) {
          printf(1, "HEAD : CAN'T OPEN %s\n", argv[i]);
          exit();
        }
        head(fd, argv[i], x);
        close(fd);
    }
  }
  
  else {
    printf(1,"HEAD : SYNTAX ERROR\n");
    exit();
  }
  exit();
}
