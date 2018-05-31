#include <stdio.h>     //printf, perror
#include <stdint.h>    //uint16_t
#include <sys/types.h> //open
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>    // exit
#include <unistd.h>    // write
#include <string.h>    // strlen

#define MY_STDIN   0
#define MY_STDOUT  1
#define MY_STDERR  2

#define S_MANUF 8
#define S_VL    11
#define S_BS    448
#define S_FSID  11
typedef struct 
{
  uint8_t   bootstrap0;                    //  0x00 3
  uint8_t   bootstrap1;                    //  0x01
  uint8_t   bootstrap2;                    //  0x02   Part of the bootstrap 3 bytes

  char      manufacturer[S_MANUF];      //  0x03 8 Optional manufacturer description
  uint16_t  bytes_per_block;               //  0x0b 2 Number of bytes per block (almost always 512).
  uint8_t   block_per_allocation_unit;     //  0x0d 1 Number of blocks per allocation unit
  uint16_t  reserved_block;                //  0x0e 2 Reserved block including the boot_block so usually 1
  uint8_t   number_of_FAT;                 //  0x10 1 Usually 1
  uint16_t  number_of_root_dir;            //       2 Including unused one, so 1
  uint16_t  number_of_block_of_media_16;   //  if it's bigger than 65 535 block, put 0 and use number_of_block_of_media_32
  uint8_t   media_descriptor;              //  old
  uint16_t  fat_size_in_block;             //  The number of blocks occupied by one copy of the File Allocation Table.
  uint16_t  block_per_track;               //  unused here
  uint16_t  number_of_head;                //  unused here
  uint32_t  number_of_hidden_block;        //  The number of hidden blocks. The use of this is largely historical, and it is nearly always set to 0
  uint32_t  number_of_block_of_media_32;   //  Total number of blocks in the entire disk (see also 0x13 )
  uint16_t  physical_drive_number;         //  Physical drive number. ( bootstrap )
  uint8_t   extended_boot_record_signature;//  bootstrap, This information is present primarily for the use of the bootstrap program
  uint32_t  volume_serial_number;          //  Unique number used for identification of a particular disk  
  char      volume_label[S_VL];            //  This is a string of characters for human-readable identification of the disk padded with spaces
  char      filesystem_identifier[S_FSID]; //  File system identifier padded at the end with spaces
  char      bootstrap[S_BS];               //  The remainder of the bootstrap program.
  uint16_t  boot_block_signature;          //  0x55 0xaa
} __attribute__((packed)) boot_block_f16;	


#define ERROR_FNOT_FOUND "file not found\n"

void print_boot_block(boot_block_f16 * bb)
{
  int i;
  printf("manufacturer                  : ");
  for (i=0;i<S_MANUF-1;i++) { putchar(bb->manufacturer[i]); }
  printf("\n");
  
  printf("byte_per_block                : ") ;
  printf("%d",bb->bytes_per_block);
  printf("\n");

  printf("block per allocation unit     : ");
  printf("%d",bb->block_per_allocation_unit);
  printf("\n");

  printf("reserved block                : ");
  printf("%d",bb->reserved_block);
  printf("\n");

  printf("number of fat                 : ");
  printf("%d",bb->number_of_FAT);
  printf("\n");

  printf("number of root dir            : ");
  printf("%d",bb->number_of_root_dir);
  printf("\n");

  printf("number of block of media16    : ");
  printf("%d",bb->number_of_block_of_media_16);
  printf("\n");
  
  printf("media descriptor              : ");
  printf("%d",bb->media_descriptor);
  printf("\n");

  printf("fat size in block             : ");
  printf("%d",bb->fat_size_in_block);
  printf("\n");

  printf("block per track               : ");
  printf("%d",bb->block_per_track);
  printf("\n");

  printf("number of head                : ");
  printf("%d",bb->number_of_head);
  printf("\n");

  printf("number of hidden block        : ");
  printf("%d",bb->number_of_hidden_block);
  printf("\n");
  
  printf("number of block of media32    : ");
  printf("%d",bb->number_of_block_of_media_32);
  printf("\n");

  printf("physical drive number         : ");
  printf("%d",bb->physical_drive_number);
  printf("\n");

  printf("extended_boot_record_signature: ");
  printf("%d",bb->extended_boot_record_signature);
  printf("\n");

  printf("volume serial number          : ");
  printf("%d",bb->volume_serial_number);
  printf("\n");

  printf("volume label                  : ");
  for (i=0;i<S_VL-1;i++) { putchar(bb->volume_label[i]); }
  printf("\n");

  printf("filesystem identifier         : ");
  for (i=0;i<S_FSID-1;i++) { putchar(bb->filesystem_identifier[i]); }
  printf("\n");

  printf("signature                     : ");
  //printf("0x%04x", bb->boot_block_signature);
  printf("%u", bb->boot_block_signature);
  printf("\n");

}


int main(int argc, char **argv)
{
  char * filename;
  boot_block_f16 bbf16;
  void *         data;
  data=&bbf16;
  int fid;
  
  //printf("%d\n",sizeof(boot_block_f16));
  if (argc==2)
    {
      filename=argv[1];
      //printf("%s\n",filename);
      fid=open(filename,O_RDONLY);
      if (fid<0) { write(MY_STDERR,ERROR_FNOT_FOUND,strlen(ERROR_FNOT_FOUND)); exit(1); }
      read(fid,data,512);
      print_boot_block(data);
    }
  if (argc==1) { printf("Argument needed\n"); exit(1); }
    
}
