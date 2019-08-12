#include "types.h"
#include "stat.h"
#include "user.h"

char buf[1];

void
cat(int fd,int l)
{
  int n;
  int c = 0;
  while(((n = read(fd, buf, sizeof(buf))) > 0) && c<l) {
    if(*buf == '\n') c++;
    printf(1,"%c",*buf);
  }
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    // cat(0);
    exit();
  } 
  int n = atoi(argv[1]);
  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "cat: cannot open %s\n", argv[i]);
      exit();
    }
    printf(1,"--- %s ---\n",argv[i]);
    cat(fd,n);
    close(fd);
  }
  exit();
}
