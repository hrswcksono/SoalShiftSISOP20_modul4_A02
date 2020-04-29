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
#include <time.h>

static const char *dirpath = "/home/haris/Documents";

char shift[] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

const int key =10;

void logsys(char *info){
	FILE *log = fopen("/home/haris/fs.log" , "a+");
	time_t now = time(0);
	struct tm tstruct = *localtime(&now);
	int tahun=0,bulan=0,hari=0,jam=0,menit=0,detik=0;
	tahun = tstruct.tm_year + 1900;
	bulan = tstruct.tm_mon;
	hari = tstruct.tm_mday;
	jam = tstruct.tm_hour;
	menit = tstruct.tm_min;
	detik = tstruct.tm_sec;
	if(strstr(level,"RMDIR") == NULL || strstr(level,"UNLINK") == NULL){
		fprintf(log, "WARNING::%d%d%d-%d:%d:%d::%s\n",tahun,bulan,hari,jam,menit,detik,info);
	}
	else{
		fprintf(log, "INFO::%d%d%d-%d:%d:%d::%s\n",tahun,bulan,hari,jam,menit,detik,info);
	}
	fclose(log);
}

char *getExt (char fspec[]) {
    char *e = strrchr (fspec, '.');
    if (e == NULL)
        e = "";
    return e;
}

void encript1(char *enc){
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

void decript1(char *enc){
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

void mergeFile(char *fileIn)
{
    int result = 0;
    FILE *fIn;
    FILE *fOut;
    void *buffer = malloc(1024);
    char name[1024];
	size_t size;
    fIn = fopen(fileIn, "wb");
	if(fIn == NULL){
		return;
	}
	else if(fIn != NULL){
		while (1){
			sprintf(name, "%s.%03d", fileIn, result);
			fOut = fopen(name, "rb");
			if (fOut == NULL){
				break;
			}
			size = fread(buffer, 1, 1024, fOut);
			fwrite(buffer, 1, size, fIn);
			if (fOut != NULL)
            {
                fclose(fOut);
            }
			remove(name);
			if(size != 0){
				result++;
			}
		}
	}
}

void splitFile(char *fileIn)
{
    int result = 0;
    FILE *fIn;
    FILE *fOut;
    void *buffer1 = malloc(1024);
	char buffer[1024];
    size_t size;
    size_t read;
    size_t written;
	size_t maxSize = 1024;
    if ((fileIn != NULL) && (maxSize > 0))
    {
        fIn = fopen(fileIn, "rb");
        if (fIn != NULL)
        {
            fOut = NULL;
            result = 0;
            while (!feof(fIn))
            {
                if (fOut == NULL)
                {
                    sprintf(buffer, "%s.%03d", fileIn, result);
                    fOut = fopen(buffer, "wb");
                    if (fOut == NULL)
                    {
                        result *= -1;
                        break;
                    }
                    size = 0;
                }
                read = sizeof(buffer1);
                if ((size + read) > maxSize)
                {
                    read = maxSize - size;
                }
                read = fread(buffer1, 1, read, fIn);
                if (read == 0)
                {
                    result *= -1;
                    break;
                }
                written = fwrite(buffer1, 1, read, fOut);
                if (written != read)
                {
                    result *= -1;
                    break;
                }
                size += written;
                if (size >= maxSize)
                {
                    fclose(fOut);
                    fOut = NULL;
                    result++;
                }
            }
            if (fOut != NULL)
            {
                fclose(fOut);
            }
            fclose(fIn);
			remove(fileIn);
        }
    }
}


//Get file attributes.
static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
	char *encv2 = strstr(path,"encv2_");
	sprintf(fpath, "%s%s",dirpath,path);
	
	if(encv1 != NULL){
        decript1(encv1);
        sprintf(fpath,"%s/%s",dirpath,encv1);
	}
	
	if(encv2 != NULL){
		
	}

	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}


//Read the target of a symbolic link
/*static int xmp_readlink(const char *path, char *buf, size_t size)
{
     int res;
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
	
	sprintf(fpath, "%s%s",dirpath,path);
	
	if(encv1 != NULL){
        decript1(encv1);
        sprintf(fpath,"%s/%s",dirpath,encv1);
	}
    res = readlink(path, buf, size - 1);
	if (res == -1)
        return -errno;
    buf[res] = '\0';
    return 0;
}*/


//Read directory
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
	//char *encv2 = strstr(path,"encv2_");
	sprintf(fpath, "%s%s",dirpath,path);
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
  	else
  	{  		
		if(encv1 != NULL ){
        	decript1(encv1);
        	sprintf(fpath,"%s/%s",dirpath,encv1);
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
	if(encv1 != NULL){
		encript1(de->d_name);
	}
    res = (filler(buf, de->d_name, &st, 0));
	if(res!=0) break;
	}
	closedir(dp);
	return 0;
}

//Read data from an open file
/*static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;
    char fpath[1000];
    char *encv1 = strstr(path,"encv1_");
	sprintf(fpath, "%s%s",dirpath,path);
	
	if(encv1 != NULL){
		decript1(encv1);
		sprintf(fpath,"%s/%s",dirpath,encv1);
	}
	
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


//Create a file node.
static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
        int res;
		char fpath[1000];
		char *encv1 = strstr(path,"encv1_");
	//	char *encv2 = strstr(path,"encv2_");
		sprintf(fpath ,"%s%s",dirpath,path);
		
		if(encv1 != NULL){
			decript1(encv1);
			sprintf(fpath,"%s/%s",dirpath,encv1);
		}
		
		if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
		}
		else if (S_ISFIFO(mode)){
			res = mkfifo(fpath, mode);
		}
		else{
			res = mknod(fpath, mode, rdev);
		}
		
        if (res == -1)
                return -errno;
        return 0;
}

//Create a directory.
static int xmp_mkdir(const char *path, mode_t mode)
{
        int res;
		char fullpath[1000];
		char *encv1 = strstr(path,"encv1_");
	//	char *encv2 = strstr(path,"encv2_");
		sprintf(fullpath ,"%s%s",dirpath,path);
		
		if(encv1 != NULL){
			decript1(encv1);
			sprintf(fullpath,"%s/%s",dirpath,encv1);
		}
		
		//if(encv2 != NULL){
			
		//}
        res = mkdir(fullpath, mode);
        if (res == -1)
                return -errno;
        return 0;
}

//Remove a file
static int xmp_unlink(const char *path)
{
        int res;
		char fpath[1000];
		char *encv1 = strstr(path,"encv1_");
		sprintf(fpath,"%s%s",dirpath,path);
		if(encv1 != NULL){
			decript1(encv1);
			sprintf(fpath,"%s/%s",dirpath,encv1);
		}
        res = unlink(fpath);
        if (res == -1)
                return -errno;
        return 0;
}

//Remove a directory
static int xmp_rmdir(const char *path)
{
        int res;
		char fpath[1000];
		char *encv1 = strstr(path,"encv1_");
		sprintf(fpath,"%s%s",dirpath,path);
		if(encv1 != NULL){
			decript1(encv1);
			sprintf(fpath,"%s/%s",dirpath,encv1);
		}
        res = rmdir(fpath);
        if (res == -1)
                return -errno;
        return 0;
}


static int xmp_symlink(const char *from, const char *to)
{
        int res;
		char from1[1000];
		char to1[1000];
		char *ffrom = strstr(from,"encv1_");
		char *fto = strstr(from,"encv1_");
		sprintf(from1,"%s%s",dirpath,from);
		sprintf(to1,"%s%s",dirpath,to);
		
		if(ffrom != NULL && fto == NULL){
			//decript1(ffrom);
			sprintf(from1,"%s/%s",dirpath,ffrom);
		}
		
		if(fto != NULL && ffrom == NULL){
			decript1(fto);
			sprintf(to1,"%s/%s",dirpath,fto);
		}
		
        res = symlink(from1, to1);
        if (res == -1)
                return -errno;
        return 0;
}

//Rename a file
static int xmp_rename(const char *from, const char *to)
{
        int res;
		char from1[1000];
		char to1[1000];
		char *ffrom = strstr(from,"encv1_");
		char *fto = strstr(from,"encv1_");
		sprintf(from1,"%s%s",dirpath,from);
		sprintf(to1,"%s%s",dirpath,to);
		
		if(ffrom != NULL && fto == NULL){
			//decript1(ffrom);
			sprintf(from1,"%s/%s",dirpath,ffrom);
		}
		else if(fto != NULL && ffrom == NULL){
			decript1(fto);
			sprintf(to1,"%s/%s",dirpath,fto);
		}
		
        res = rename(from1, to1);
        if (res == -1)
                return -errno;
        return 0;
}


static int xmp_link(const char *from, const char *to)
{
        int res;
		char from1[1000];
		char to1[1000];
		char *ffrom = strstr(from,"encv1_");
		char *fto = strstr(from,"encv1_");
		sprintf(from1,"%s%s",dirpath,from);
		sprintf(to1,"%s%s",dirpath,to);
		
		if(ffrom != NULL && fto == NULL){
			//decript1(ffrom);
			sprintf(from1,"%s/%s",dirpath,ffrom);
		}
		
		if(fto != NULL && ffrom == NULL){
			decript1(fto);
			sprintf(to1,"%s/%s",dirpath,fto);
		}

        res = link(from1, to1);
        if (res == -1)
                return -errno;
        return 0;
}

//Change the permission bits of a file
static int xmp_chmod(const char *path, mode_t mode)
{
        //(void) fi;
    int res;
		char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
	sprintf(fpath,"%s%s",dirpath,path);
	
	if(encv1 != NULL){
		decript1(encv1);
		sprintf(fpath,"%s/%s",dirpath,encv1);
	}
	
    res = chmod(fpath, mode);
    if (res == -1)
        return -errno;
    return 0;
}

//Change the owner and group of a file
static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
    //void) fi;
    int res;
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
	sprintf(fpath,"%s%s",dirpath,path);
	
	if(encv1 != NULL){
		decript1(encv1);
		sprintf(fpath,"%s/%s",dirpath,encv1);
	}
    res = lchown(fpath, uid, gid);
    if (res == -1)
        return -errno;
    return 0;
}

//Change the size of a file
static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
	sprintf(fpath,"%s%s",dirpath,path);
	
	if(encv1 != NULL){
		decript1(encv1);
		sprintf(fpath,"%s/%s",dirpath,encv1);
	}
	
	res = truncate(fpath, size);
	
    if (res == -1)
        return -errno;
    return 0;
}*/


static struct fuse_operations xmp_oper = {
	
.getattr 		= xmp_getattr,
.readdir 	= xmp_readdir,
/*.readlink	= xmp_readlink,
.read 		= xmp_read,
.mkdir 		= xmp_mkdir,
.mknod 	= xmp_mknod,
.symlink 	= xmp_symlink,
.unlink 		= xmp_unlink,
.rmdir 		= xmp_rmdir,
.rename 	= xmp_rename,
.link 			= xmp_link,
.chmod 	= xmp_chmod,
.chown 		= xmp_chown,
.truncate 	= xmp_truncate,*/

};


int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
