#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <grp.h>
#include <pwd.h>

static const char *dirpath = "/home/haris/Documents";

char shift[] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

const int key =10;

char *getExt (char fspec[]) {
    char *e = strrchr (fspec, '.');
    if (e == NULL)
        e = "";
    return e;
}


void encript(char *enc){
	if(!strcmp(enc,".") || !strcmp(enc,"..")) return;
	int b=0;
	b = strlen(getExt(enc));
	for ( int i = 0; i < strlen(enc)-b ;i++) {
		if(enc[i] != '/'){
			for (int j = 0; j < 87; j++) {
	     		if(enc[i] == shift[j]) {
	        		enc[i] = shift[(j+key) % 87];
	        		break;
        		}
			}
		}
	}
}

void decript(char *enc){
	if(!strcmp(enc,".") || !strcmp(enc,"..")) return;
	int flag1=0,flag2=0;
	if(enc[0]=='e' && enc[1]=='n' && enc[2]=='c' && enc[3]=='v' && enc[4]=='1' && enc[5] == '_'){
		flag1 = 1;
		flag2 = 1;
	}
	int b=0;
	b = strlen(getExt(enc));
	for ( int i = 0; i < strlen(enc) -b ;i++) {
		if(enc[i] == '/'){
			flag1=0;
		}
		if((flag1!=1 && enc[i] != '/') && flag2==1){
			for (int j = 0; j < 87; j++) {
	     		if(enc[i] == shift[j]) {
	        		enc[i] = shift[(j+87-key) % 87];
	        		break;
        		}
			}
		}
	}
}




static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	sprintf(fpath, "%s%s",dirpath,name);
	if(strstr(fpath,"encv1_") != NULL){
		char arr[1000];
        	strcpy(arr,strstr(fpath,"encv1"));
        	decript(arr);
        	sprintf(fpath,"%s/%s",dirpath,arr);
	}
	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char dir[1000];
	char fpath[1000];
  	sprintf(dir, "%s", path);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
  	else
  	{
   		sprintf(fpath, "%s%s",dirpath,path);
		if(strstr(fpath,"encv1_") != NULL ){
		char arr[1000];
        	strcpy(arr,strstr(fpath,"encv1"));
        	decript(arr);
        	sprintf(fpath,"%s/%s",dirpath,arr);
            }
  		
  	}
	int res = 0;
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(fpath);
	if (dp == NULL)
	return -errno;
	while ((de = readdir(dp)) != NULL) {
	struct stat st;
	memset(&st, 0, sizeof(st));
	st.st_ino = de->d_ino;
  	st.st_mode = de->d_type << 12;
	if(strstr(fpath,"encv1_") != NULL){
		encript(de->d_name);
	}
      	res = (filler(buf, de->d_name, &st, 0));
	if(res!=0) break;
	}
	closedir(dp);
	return 0;
}


static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;
        char fpath[1000];
        char name[1000];
	sprintf(name,"%s",path);
        encript(name);
	sprintf(fpath, "%s%s",dirpath,name);
	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}


static int xmp_mkdir(const char *path, mode_t mode)
{
        int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	sprintf(fpath, "%s%s",dirpath,name);
	if(strstr(fpath,"encv1_") != NULL){
	        char *i = strstr(fpath,"encv1_");
	        decript(i);
        	sprintf(fpath,"%s%s",dirpath,i);
	}
        res = mkdir(fpath, mode);
        if (res == -1)
                return -errno;
        return 0;
}



static struct fuse_operations xmp_oper = {

.getattr = xmp_getattr,
.readdir = xmp_readdir,
.read = xmp_read,
.mkdir = xmp_mkdir,

};


int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
