// gcc -lext2fs -lcom_err openandlist.c  -o openandlist 

#include <stdio.h>
#include <ext2fs/ext2fs.h>


void print_err_and_exit(char * progname,char * fsname,errcode_t err)
{
 if (err) 
   {
     com_err(progname, err, "while opening filesystem %s", fsname);
     exit(1);
   }
}


void check_arc_argv(int argc,char ** argv)
{
  if (argc != 2) 
    { 
      printf("%s <blockdev|extImageFile>\n",argv[0]); 
      exit(1); 
    }

}

int main(int argc, char **argv)
{
  check_arc_argv(argc,argv);
  ext2_filsys fs;
  io_manager io_ptr = unix_io_manager;
  errcode_t err;
  char *progname=argv[0];
  char *fsname = argv[1];
  struct ext2_super_block *sb;

  add_error_table(&et_ext2_error_table);

  err=ext2fs_open(fsname,0,0,4096,unix_io_manager,&fs);
  print_err_and_exit(progname,fsname,err);

  //printf("group desc : %d\n",fs->group_desc_count);
  sb=fs->super;
  printf("volume name : %s\n",sb->s_volume_name);
  printf("volume uuid : %s\n",sb->s_uuid);
 
  err=ext2fs_close(fs);
  print_err_and_exit(progname,fsname,err);
  //  ext2fs_free(fs);
}
