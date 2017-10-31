#include <mutex>        // std::mutex
#include <vector>
#include <mutex>
#include <string>
#include <dirent.h>

class DirectoryPCP;

enum d_type
  {
    PCP_DT_BLK,
    PCP_DT_CHR,
    PCP_DT_DIR,
    PCP_DT_FIFO,
    PCP_DT_LNK,
    PCP_DT_REG,
    PCP_DT_SOCK,
    PCP_DT_UNKNOWN
  };

/*################################################################################*/

class InodePCP
{
 public:
  int64_t size;
  int32_t permission;
  int     type;
  DirectoryPCP * son;
};


/*################################################################################*/



/*################################################################################*/

class DirectoryPCP
{
public:
  DirectoryPCP();
  int  read(std::string pth);
  void print();

  std::string path;
  std::vector <std::string>    v_str;
  std::vector <InodePCP>       v_ino;
};


DirectoryPCP::DirectoryPCP()
{

}

void DirectoryPCP::print()
{
  int i;
  for (i=0;i<v_str.size();i++)
    printf("[%d] %s\n",
	   v_ino[i].type,
	   v_str[i].c_str());
}

int DirectoryPCP::read(std::string pth)
{
  struct dirent * de;
  DIR           * dir;
  path=pth;
  dir=opendir(path.c_str());
  while ((de=readdir(dir))!=NULL)
    {
      std::string str(de->d_name);
      InodePCP ino;

      if (de->d_type==DT_REG)   ino.type=PCP_DT_REG;
      if (de->d_type==DT_DIR)
	{
	  ino.type=PCP_DT_DIR;
	  ino.son=new(DirectoryPCP);
	  ino.son->path=str;
	}
      v_str.push_back(str);
      v_ino.push_back(ino);

      printf("%d %s\n",de->d_type, de->d_name);
    }
  closedir(dir);
  return 0;

}


class DirectoryStructure
{
 public:
  DirectoryPCP root;
};
