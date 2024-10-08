#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/futex.h>

#define NB_SECONDS 20

static char *str_second[NB_SECONDS] =
{
  "zero", "un", "deux", "trois", "quatre", "cinq",
  "six", "sept", "huit", "neuf", "dix",
  "onze", "douze", "treize", "quatorze", "quinze",
  "seize", "dix sept", "dix huit", "dix neuf"
};

char data[100];

int futex_var;

#define futex_wait(addr, val) \
           syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL)
#define futex_wakeup(addr, nb) \
           syscall(SYS_futex, addr, FUTEX_WAKE, nb)

static void LOCK(int *f)
{
int old;

  while (1)
  {
    old = __sync_val_compare_and_swap(f, 0, 1);

    if (0 == old)
    {
      // The futex variable was free as it was equal to 0
      // Now, it is set to 1
      return;
    }
    else
    {
      // The futex variable is equal to 1
      // ==> It is not modified
      futex_wait(f, 1);
    }
  }
} // LOCK

static void UNLOCK(int *f)
{
  (void)__sync_fetch_and_sub(f, 1);

  futex_wakeup(f, 1);
} // UNLOCK

static void *thd_main(void *p)
{
unsigned int i = 0;

 (void)p;

  while(1)
  {
    // Update of the counter
    LOCK(&futex_var);
    strcpy(data, str_second[i]);
    UNLOCK(&futex_var);

    // Sleep 1 second
    sleep(1);

    // Incrementation of the counter in the range [0, NB_SECONDS[
    i = (i + 1) % NB_SECONDS;
  } // End while

  return NULL;
} // thd_main

int main(void)
{
pthread_t tid;

  // Creation of the thread
  (void)pthread_create(&tid, NULL, thd_main, NULL);

  // Interaction with the operator
  while(fgetc(stdin) != EOF)
  {
    // Display the current value of the counter
    LOCK(&futex_var);
    printf("Current counter value: %s\n", data);
    UNLOCK(&futex_var);
  } // End while

  return 0;
} // main

