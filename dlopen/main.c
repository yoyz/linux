#include <stdio.h>
#include <dlfcn.h>

void main()
{
  void *dlhandle_toto = dlopen("./libtoto.so", RTLD_NOW);
  void *dlhandle_titi = dlopen("./libtiti.so", RTLD_NOW);
  void (*func_toto)();
  void (*func_titi)();

  //printf who am i
  printf("main\n");

  //resolve function from previously loaded lib
  func_toto=dlsym(dlhandle_toto, "toto");
  func_titi=dlsym(dlhandle_titi, "titi");

  //print address of the pointer
  printf("dlhandle : %d %d\n",dlhandle_toto,dlhandle_titi);
  printf(" main:\t\t0x%08.8X\n func_toto:\t0x%08.8X\n func_titi:\t0x%08.8X \n",main,func_toto,func_titi);

  //call the lib
  func_toto();
  func_titi();

  //free the lib from the main process
  dlclose(dlhandle_toto);
  dlclose(dlhandle_titi);

}
