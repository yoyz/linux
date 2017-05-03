#include <unistd.h>
#include "stdio.h"
#include "bfd.h"
int main(int argc, char **argv)
{
    if (argc < 2)
     { printf("Need a binary as argument\n"); exit(1); }
    bfd *tmp_bfd = NULL;                        //handler for libbfd
    asection *s;
    //tmp_bfd = bfd_openr ("main.elf", NULL);
    tmp_bfd = bfd_openr (argv[1], NULL);
    if (tmp_bfd == NULL) {
    printf ("Error openning file\n");
        exit(-1);
    }
    //check if the file is in format
    if (!bfd_check_format (tmp_bfd, bfd_object)) {
                if (bfd_get_error () != bfd_error_file_ambiguously_recognized) {
                        printf("Incompatible format\n");
                        exit(-1);
                }
    }
    printf ("exec file \"main.out\"'s format is %s\n",
        tmp_bfd->xvec->name);
    //printf("Entry point is at address %d\n",bfd_get_start_address(tmp_bfd));
    printf("Entry point is at address 0x%08x\n",bfd_get_start_address(tmp_bfd));
 
    /* load the corresponding section to memory */
    for (s = tmp_bfd->sections; s; s = s->next) {
        if (bfd_get_section_flags (tmp_bfd, s) & (SEC_LOAD)) {
            if (bfd_section_lma (tmp_bfd, s) != bfd_section_vma (tmp_bfd, s)) {
                printf ("loadable section %s: lma = 0x%08x (vma = 0x%08x)  size = 0x%08x\n", bfd_section_name (tmp_bfd, s), (unsigned int) bfd_section_lma (tmp_bfd, s), (unsigned int) bfd_section_vma (tmp_bfd, s), (unsigned int) bfd_section_size (tmp_bfd, s));
            } else {
                printf ("loadable section %s: addr = 0x%08x  size = 0x%08x\n", bfd_section_name (tmp_bfd, s), (unsigned int) bfd_section_lma (tmp_bfd, s), (unsigned int) bfd_section_size (tmp_bfd, s));
            }
        }
        else {
                 printf ("non-loadable section %s: addr = 0x%08x  size = 0x%08x \n", bfd_section_name (tmp_bfd, s), (unsigned int) bfd_section_vma (tmp_bfd, s), (unsigned int) bfd_section_size (tmp_bfd, s));
        }
    }                   //end of for loop
    return 0;
}
