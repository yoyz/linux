#include <stdio.h>
#include <stdlib.h>

struct myobj
{
  int val;
  void (*setVal)(struct myobj * self, int val);
  void (*printVal)(struct myobj * self);
};


void setValFunc(struct myobj * self, int val)
{
  self->val=val;
}

void printValFunc(struct myobj * self)
{
  printf("%d",self->val);
}


struct myobj * prepareMyobj(void)
{
  struct myobj * newobj;
  newobj=malloc(sizeof(struct myobj));
  newobj->setVal=(&setValFunc);
  newobj->printVal=(&printValFunc);
  return newobj;
}


void main()
{
  printf("bla\n");
  struct myobj * obj;
  obj=prepareMyobj();
  obj->setVal(obj,10);
  obj->printVal(obj);
  
}
