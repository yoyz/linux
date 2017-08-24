#include "reportfsactivity.hh"

ReportFSActivity* ReportFSActivity::_instance = NULL;

#define RETURN_ERRNO(x) (x) == 0 ? 0 : -errno

ReportFSActivity* ReportFSActivity::Instance() {
	if(_instance == NULL) {
		_instance = new ReportFSActivity();
	}
	return _instance;
}

ReportFSActivity::ReportFSActivity()
{
  printf("ReportFSActivity::ReportFSActivity()\n");
}

ReportFSActivity::~ReportFSActivity()
{
  //int i;
  typedef std::map<std::string,int64_t>::const_iterator total_write_size_iterator;
  typedef std::map<std::string,int64_t>::const_iterator total_read_size_iterator;

  printf("ReportFSActivity::~ReportFSActivity()\n");


  printf("\n");
  printf("File Write\n");
  printf("=============\n");
  for (total_write_size_iterator twi = total_write_size.begin();
       twi!=total_write_size.end();
       twi++)
    {
      //std::cout << twi->first << " : " << twi->second << std::endl;
      printf("%.64s # %.12lu MiB count:%.8lu\n",
	     twi->first.c_str(),
	     twi->second/1024/1024,
	     total_write_count[twi->first]);
    }

  printf("\n");
  printf("File Read\n");
  printf("=============\n");

  for (total_read_size_iterator twi = total_read_size.begin();
       twi!=total_read_size.end();
       twi++)
    {
      //std::cout << twi->first << " : " << twi->second << std::endl;
      printf("%.64s # %.12lu MiB count:%.8lu\n",
	     twi->first.c_str(),
	     twi->second/1024/1024,
	     total_read_count[twi->first]);
	     
     
    }


}

void ReportFSActivity::AbsPath(char dest[PATH_MAX], const char *path) {
	strcpy(dest, _root);
	strncat(dest, path, PATH_MAX);
	//printf("translated path: %s to %s\n", path, dest);
}

void ReportFSActivity::setRootDir(const char *path)
{
  printf("setting FS root to: %s\n", path);
  _root = path;
}

int ReportFSActivity::Getattr(const char *path, struct stat *statbuf) {
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  //printf("getattr(%s)\n", fullPath);
  return RETURN_ERRNO(lstat(fullPath, statbuf));
}

int ReportFSActivity::Readlink(const char *path, char *link, size_t size) {
  //printf("readlink(path=%s, link=%s, size=%d)\n", path, link, (int)size);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(readlink(fullPath, link, size));
}

int ReportFSActivity::Mknod(const char *path, mode_t mode, dev_t dev) {
  printf("mknod(path=%s, mode=%d)\n", path, mode);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  
  //handles creating FIFOs, regular files, etc...
  return RETURN_ERRNO(mknod(fullPath, mode, dev));
}

int ReportFSActivity::Mkdir(const char *path, mode_t mode) {
  //printf("**mkdir(path=%s, mode=%d)\n", path, (int)mode);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(mkdir(fullPath, mode));
}

int ReportFSActivity::Unlink(const char *path)
{
  //printf("unlink(path=%s\n)", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(unlink(fullPath));
}

int ReportFSActivity::Rmdir(const char *path)
{
  //printf("rmkdir(path=%s\n)", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(rmdir(fullPath));
}

int ReportFSActivity::Symlink(const char *path, const char *link)
{
  printf("symlink(path=%s, link=%s)\n", path, link);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(symlink(fullPath, link));
}

int ReportFSActivity::Rename(const char *path, const char *newpath)
{
  printf("rename(path=%s, newPath=%s)\n", path, newpath);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(rename(fullPath, newpath));
}

int ReportFSActivity::Link(const char *path, const char *newpath)
{
  //printf("link(path=%s, newPath=%s)\n", path, newpath);
  char fullPath[PATH_MAX];
  char fullNewPath[PATH_MAX];
  AbsPath(fullPath, path);
  AbsPath(fullNewPath, newpath);
  return RETURN_ERRNO(link(fullPath, fullNewPath));
}

int ReportFSActivity::Chmod(const char *path, mode_t mode)
{
  //printf("chmod(path=%s, mode=%d)\n", path, mode);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(chmod(fullPath, mode));
}

int ReportFSActivity::Chown(const char *path, uid_t uid, gid_t gid)
{
  //printf("chown(path=%s, uid=%d, gid=%d)\n", path, (int)uid, (int)gid);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(chown(fullPath, uid, gid));
}

int ReportFSActivity::Truncate(const char *path, off_t newSize)
{
  //printf("truncate(path=%s, newSize=%d\n", path, (int)newSize);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(truncate(fullPath, newSize));
}

int ReportFSActivity::Utime(const char *path, struct utimbuf *ubuf)
{
  //printf("utime(path=%s)\n", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(utime(fullPath, ubuf));
}

int ReportFSActivity::Open(const char *path, struct fuse_file_info *fileInfo)
{
  //printf("open(path=%s)\n", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  fileInfo->fh = open(fullPath, fileInfo->flags);
  return 0;
}

int ReportFSActivity::Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo)
{
  int size_read=0;
  int64_t total=total_read_size[std::string(path)]+size;
  std::string spth=std::string(path);
  
  total_read_size[spth]=total;
  total_read_count[spth]++;

  //printf("read(path=%s, size=%d, offset=%d)\n", path, (int)size, (int)offset);
  size_read=pread(fileInfo->fh, buf, size, offset);
  return size_read;
    
  //return RETURN_ERRNO(pread(fileInfo->fh, buf, size, offset));
	
}

int ReportFSActivity::Write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo)
{
  int size_write=0;
  std::string spth=std::string(path);
  int64_t total=total_write_size[spth]+size;

  total_write_size[spth]=total;
  total_write_count[spth]++;
  //printf("write(path=%.64s, size=%.8d, offset=%.16d) total:%fMib\n", path, (int)size, (int)offset,(total/1024)/1024);

  size_write=pwrite(fileInfo->fh, buf, size, offset);
  return size_write;
		    
	//return RETURN_ERRNO(pwrite(fileInfo->fh, buf, size, offset));
	
}

int ReportFSActivity::Statfs(const char *path, struct statvfs *statInfo)
{
  //printf("statfs(path=%s)\n", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(statvfs(fullPath, statInfo));
}

int ReportFSActivity::Flush(const char *path, struct fuse_file_info *fileInfo)
{
  //printf("flush(path=%s)\n", path);
  //noop because we don't maintain our own buffers
  return 0;
}

int ReportFSActivity::Release(const char *path, struct fuse_file_info *fileInfo)
{
  //printf("release(path=%s)\n", path);
  return 0;
}

int ReportFSActivity::Fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
  //printf("fsync(path=%s, datasync=%d\n", path, datasync);
  if(datasync) {
    //sync data only
    return RETURN_ERRNO(fdatasync(fi->fh));
  } else {
    //sync data + file metadata
    return RETURN_ERRNO(fsync(fi->fh));
  }
}

int ReportFSActivity::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
  //printf("setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n",
  //path, name, value, (int)size, flags);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(lsetxattr(fullPath, name, value, size, flags));
}

int ReportFSActivity::Getxattr(const char *path, const char *name, char *value, size_t size)
{
  //printf("getxattr(path=%s, name=%s, size=%d\n", path, name, (int)size);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(getxattr(fullPath, name, value, size));
}

int ReportFSActivity::Listxattr(const char *path, char *list, size_t size)
{
  //printf("listxattr(path=%s, size=%d)\n", path, (int)size);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(llistxattr(fullPath, list, size));
}

int ReportFSActivity::Removexattr(const char *path, const char *name)
{
  //printf("removexattry(path=%s, name=%s)\n", path, name);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(lremovexattr(fullPath, name));
}

int ReportFSActivity::Opendir(const char *path, struct fuse_file_info *fileInfo)
{
  //printf("opendir(path=%s)\n", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  DIR *dir = opendir(fullPath);
  fileInfo->fh = (uint64_t)dir;
  return NULL == dir ? -errno : 0;
}

int ReportFSActivity::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo)
{
  //printf("readdir(path=%s, offset=%d)\n", path, (int)offset);
  DIR *dir = (DIR*)fileInfo->fh;
  struct dirent *de = readdir(dir);
  if(NULL == de) {
    return -errno;
  } else {
    do {
      if(filler(buf, de->d_name, NULL, 0) != 0) {
	return -ENOMEM;
      }
    } while(NULL != (de = readdir(dir)));
  }
  return 0;
}

int ReportFSActivity::Releasedir(const char *path, struct fuse_file_info *fileInfo)
{
  //printf("releasedir(path=%s)\n", path);
  closedir((DIR*)fileInfo->fh);
  return 0;
}

int ReportFSActivity::Fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo)
{
  return 0;
}

int ReportFSActivity::Init(struct fuse_conn_info *conn)
{
  return 0;
}

int ReportFSActivity::Truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo)
{
  //printf("truncate(path=%s, offset=%d)\n", path, (int)offset);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(ftruncate(fileInfo->fh, offset));
}


