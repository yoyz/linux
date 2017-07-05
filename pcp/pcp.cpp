#include <stdio.h>
#include "pcp.h"
#include <dirent.h>


int main(int argc, char ** argv)
{
  DirectoryStructure DS;
  DS.root.read("/home/peyrardj/build");
  DS.root.print();
}

/*
int main(int argc, char ** argv)
{
  DirectoryStructure DS;
  DS.root.Path=".";
  printf("<ls>\n");
  DIR * dir;
  struct dirent * de;
  dir=opendir(".");
  while ((de=readdir(dir))!=NULL)
    printf("%d %s\n",de->d_type, de->d_name);
  closedir(dir);
}
*/
