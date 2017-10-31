#include <vector>
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <mutex>

std::vector <int> vec1;

class File
{
 public:
  File();
  std::string             path;
  std::mutex              mtx_f;
  int64_t size;

};

File::File()
{

}



int main()
{
  int i;
  vec1.push_back(12);
  vec1.push_back(100);
  vec1.push_back(9);
  vec1.push_back(21);
  for (i=0;i<vec1.size();i++)
    printf("%d ",vec1[i]);
  printf("\n");
}

