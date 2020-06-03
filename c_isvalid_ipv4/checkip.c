#include <stdio.h>
#include <string.h>
# https://stackoverflow.com/questions/1088622/how-do-i-create-an-array-of-strings-in-c

int isValidIp4 (char *str) {
    int segs = 0;   /* Segment count. */
    int chcnt = 0;  /* Character count within segment. */
    int accum = 0;  /* Accumulator for segment. */

    /* Catch NULL pointer. */

    if (str == NULL)
        return 0;

    /* Process every character in string. */

    while (*str != '\0') {
        /* Segment changeover. */

        if (*str == '.') {
            /* Must have some digits in segment. */

            if (chcnt == 0)
                return 0;

            /* Limit number of segments. */

            if (++segs == 4)
                return 0;

            /* Reset segment values and restart loop. */

            chcnt = accum = 0;
            str++;
            continue;
        }
        /* Check numeric. */

        if ((*str < '0') || (*str > '9'))
            return 0;

        /* Accumulate and check segment. */

        if ((accum = accum * 10 + *str - '0') > 255)
            return 0;

        /* Advance other segment specific stuff and continue loop. */

        chcnt++;
        str++;
    }

    /* Check enough segments and enough characters in last segment. */

    if (segs != 3)
        return 0;

    if (chcnt == 0)
        return 0;

    /* Address okay. */

    return 1;
}

int main()
{
  char   cstr[16][64];
  char   dstr[64];
  
  strcpy(cstr[0],"1");
  strcpy(cstr[1],"1.1");
  strcpy(cstr[2],"1.1.1");
  strcpy(cstr[3],"1.1.1.1");
  strcpy(cstr[4],"255.255.255.255");
  strcpy(cstr[5],"255.255.255.255");
  strcpy(cstr[6],"mgs.MGS.exports.10.3.1.165@o2ib");
  strcpy(cstr[7],"MGS.exports.10.3.1.165");

  for ( int i=0; i<8;i++)
    {
      printf("%s : %d\n", cstr[i], isValidIp4(cstr[i]));
    }

  
  for ( int i0=0; i0<260;i0++)
    {
      sprintf(dstr,"%d.%d.%d.%d",i0,i0,i0,i0);
      printf("%s : %d\n", dstr, isValidIp4(dstr));
    }
}
