#ifndef EXT2_H_
#define EXT2_H_
#include <stdint.h>

//  CHAR    1 byte
//  INT     2 byte
//  LONG    4 byte
struct MBR {
    uint8_t   bootstrap[446];
    uint8_t   pEntry1[16];
    uint8_t   pEntry2[16];
    uint8_t   pEntry3[16];
    uint8_t   pEntry4[16];
    uint16_t    signature;
};

struct partitionDesc {
    uint8_t   status;
    uint8_t   CHS1st_Head;
    uint8_t   CHS1st_sector_cylnderH;
    uint8_t   CHS1st_cylinderL;
    uint8_t   partType;
    uint8_t   CHSLast_Head;
    uint8_t   CHSLast_sector_cylnderH;
    uint8_t   CHSLast_cylinderL;
    uint32_t   LBA_1st_sector;
    uint32_t   sectorCoun;
};

struct superblock {
    uint32_t   s_inodes_count;             //  total number of inode;
    uint32_t   s_block_count;              //  total number of block;
    uint32_t   s_r_blocks_count;           //  total number of reserved block for superuser;
    uint32_t   s_free_blocks_count;        //  total number of free block (include super user)
    uint32_t   s_free_inodes_count;        //  total number of free inode
    uint32_t   s_first_data_block;         //  identification id of the first block that contain the superblock - for 1KB block = 1 - for block > 1KB = 0; 
    uint32_t   s_long_block_size;          //  shift value used for: block size = 1024 << s_long_block_size
     int32_t   s_log_frag_size;            //  shift value used for: fragment size = 1024 << s_long_frag_size or fragment size = 1024 >> -s_long_frag_size
    uint32_t   s_blocks_per_gruop;         //  number of block per group
    uint32_t   s_frags_per_group;          //  number of fragment per group
    uint32_t   s_inodes_per_group;         //  number of inode per group also used for calculate the size of the inode bitmap of each block group
    uint32_t   s_mtime;                    //  last time fs was mounted - UNIX time def POSIX
    uint32_t   s_wtime;                    //  last time fs write access - UNIX time def POSIX
    uint16_t   s_mnt_count;                //  number of time fs was mounted
    uint16_t   s_max_mnt_count;            //  number of time fs was mounted from last full check
    uint16_t   s_magic;                    //  FS identification number EXT2 = 0xEF53
    uint16_t   s_state;                    //  fs state EXT2_VALID_FS = 1 | EXT2_ERROR_FS = 2
    uint16_t   s_errors;                   //  to do in case of err EXT2_ERRORS_CONTINUE = 1 | EXT2_ERRORS_RO = 2 | EXT2_ERRORS_PANIC = 3
    uint16_t   s_minor_rev_level;          //  revision level
    uint32_t   s_lastcheck;                //  last file system check - UNIX time def POSIX
    uint32_t   s_checkinterval;            //  maximum unix time interval allowed between fs check - UNIX time def POSIX
    uint32_t   s_creator_os;               //  fs os creator EXT2_OS_LINUX = 0 | EXT2_OS_HURD = 1 | EXT2_OS_MASIX = 2 | EXT2_OS_FREEBSD = 3 | EXT2_OS_LITES = 4
    uint32_t   s_rev_level;                //  revision level value EXT2_GOOD_OLD_REV = 0 | EXT2_DYNAMIC_REV = 1
    uint16_t   s_def_resuid;               //  default user id for reserved blocks Ext2 default 0
    uint16_t   s_def_resgid;               //  default user group for reserved blocks Ext2 default 0    
    uint32_t   s_first_ino;                //  index first usable inode for standard file
    uint16_t   s_inode_size;               //  inode size
};      
        
struct blockGroup {     
    uint32_t   bg_block_bitmap;            //  first block bitmap id of the group
    uint32_t   bg_inode_bitmap;            //  first inode bitmap id of the group
    uint32_t   bg_inode_table;             //  first inode table id of the group
    uint16_t   bg_free_block_count;        //  total number of free block of the group
    uint16_t   bg_free_inode_count;        //  total inode of free block of the group
    uint16_t   bg_used_dirs_count;         //  number of inode allocated to directories of the group
    uint16_t   bg_pad;                     //  padding
    uint32_t   bg_reserved[3];     
};      
        
struct inode {      
    uint16_t   i_mode;                    //  indicate the format of the described file and the access rights
    uint16_t   i_uid;                     //  user id associated
    uint32_t   i_size;                    //  size of the file in byte - lower 32bit
    uint32_t   i_atime;                   //  access time (s) from 01/01/1970
    uint32_t   i_ctime;                   //  creation time (s) from 01/01/1970
    uint32_t   i_mtime;                   //  modification time (s) from 01/01/1970
    uint32_t   i_dtime;                   //  deleted time (s) from 01/01/1970
    uint16_t   i_gid;                     //  value of the POSIX group having access to this file
    uint16_t   i_links_count;             //  indicating how many times this this particular inode is linked (referred to)
    uint32_t   i_blocks;                  //  total number of 512-byte blocks reserved to contain the data of this inode
    uint32_t   i_flags;                   //  
    uint32_t   i_osd1;                    //  OS dependent
    uint32_t   i_block[15];               //  first 12block - pointer to block id where data are stored | 13 - single-indirect blocks | 14 - doubly-indirect blocks | 15 - triply-indirect blocks
    uint32_t   i_generation;              //  used to indicate the file version
    uint32_t   i_file_acl;                //  block number of extended attribute
    uint32_t   i_dir_acl;                 //  size of the file in byte - high 32bit
    uint32_t   i_faddr;                   //  location of file fragment (deprecated)
    uint32_t   i_osd2[3];                 //  OS dependent structure
};      
        
struct dirEntry {       
    uint32_t  d_inode;                   //  inode id of the file/dir
    uint16_t  d_rec_len;                 //  total record length used stote this record
    uint8_t   d_name_len;                //  name length
    uint8_t   d_file_type;               //  file type dir|regulatFile
    uint8_t   d_file_name[255];          //  name
};

typedef struct {
    uint32_t   inode_id;                  //  node id of the file/dir
    uint16_t    inode_type;                //  inode type
    uint8_t   inode_name[20];            //  inode name
} DIR;

typedef struct {
    uint32_t   last_inode_block_index;    //  last index used of i_block
    uint32_t   dir_entry_byte;            //  next entry byte (!= only for directory not empty)
} DIR_HNDL;

typedef struct {
    uint32_t   inode_id;                  //  file inode id
    uint32_t   last_byte;                 //  last byte read
} FILE_HNDL;


volatile uint32_t   EXT_base;             //  ext partition base address
volatile uint32_t   bg_[100];             //  block group base address
volatile uint16_t    bg_count;             //  block group count


volatile uint32_t   blockGroupNo;

volatile uint32_t   block_size;
volatile uint32_t   inodes_addr[512];     //  inode table base address
volatile uint32_t   first_ino;
volatile uint32_t   first_block_ino;      //  index first usable inode for standard file
volatile uint32_t   s_inodes_per_group;   //  inodes per group
volatile uint32_t   inode_block_count;    //  number of inode into a block
volatile uint16_t    inode_size;

extern volatile uint8_t    buffer[1024];

#define INODE_STRUCT_SIZE   128
#define BG_SIZEOFF          32
#define BG_LBA_OFFSET       4    
#define DIR_TEMP_OFFSET     264
#define MAX_F_NAME          20
#define EXT2_FT_DIR         2
#define EXT2_FT_REG_FILE    1
//#define EOF                 0x04
#define MAX_FLINE_LENGHT    1024

int        EXT_mount();
DIR        EXT_ls(DIR fileDir, DIR_HNDL* hndl);
char    EXT_readfile( FILE_HNDL *fileHndl, char* text );

#endif /* EXT2_H_ */
