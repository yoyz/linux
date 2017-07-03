#include <stdio.h>
#include <dirent.h>

int main(int argc, char ** argv)
{ 
  DIR * dir;
  struct dirent * de;
  char pth[2]=".";
  
  if (argc==1)
      dir=opendir(pth);
  else
    if (argc==2)
      dir=opendir(argv[1]);    
  
  while ((de=readdir(dir))!=NULL)
    printf("%s\n",de->d_name);
  closedir(dir);  
}
