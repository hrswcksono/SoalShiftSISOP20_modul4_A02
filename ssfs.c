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

void logsys(int level,char *call,char *info){
	FILE *log = fopen("/home/haris/Desktop/fs.log" , "a");
	time_t now = time(0);
	struct tm tstruct = *localtime(&now);
	int tahun=0,bulan=0,hari=0,jam=0,menit=0,detik=0;
	tahun = tstruct.tm_year + 1900;
	bulan = tstruct.tm_mon;
	hari = tstruct.tm_mday;
	jam = tstruct.tm_hour;
	menit = tstruct.tm_min;
	detik = tstruct.tm_sec;
	if(level == 1){
		fprintf(log, "WARNING::%d%d%d-%d:%d:%d::%s::%s\n",tahun,bulan,hari,jam,menit,detik,call,info);
	}
	else{
		fprintf(log, "INFO::%d%d%d-%d:%d:%d::%s::%s\n",tahun,bulan,hari,jam,menit,detik,call,info);
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

void mergeFile( char *fileIn)
{
    int result = 0;
    FILE *fIn;
    FILE *fOut;
    char buffer[1024];
    char name[1024];
	char r[1000];
	int len = strlen(fileIn) - strlen(getExt(fileIn));
	memset(&r,'\0',sizeof(r));
	strncpy(r,fileIn,len);
	printf("%s",r);
	size_t size;
    fIn = fopen(r, "a");
	if(fIn == NULL){
		return;
	}
	else if(fIn != NULL){
		while (1){
			sprintf(name, "%s.%03d", r, result);
			fOut = fopen(name, "rb");
			if (fOut == NULL){
				break;
			}
			size = fread(buffer, sizeof(char), 1024, fOut);
			fwrite(buffer, 1, size, fIn);
			if(fOut != NULL){
				fclose(fOut);
			}
			remove(name);
			result++;
		}
	}
	fclose(fIn);
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


void getdir2( char *dirPath , int cur) {
	DIR *dp;
	struct dirent *de;
	dp = opendir(dirPath);
	if (dp == NULL)
		return;
	while ((de = readdir(dp)) != NULL) {
		if (strcmp(de->d_name,".")==0 || strcmp(de->d_name,"..")==0) {
			continue;
		}
		char fpath[500];
		sprintf(fpath,"%s/%s",dirPath,de->d_name);
		if (de->d_type == DT_REG) {
			if(cur == 1)splitFile(fpath);
			if(cur == 0)mergeFile(fpath);
		} else if (de->d_type == DT_DIR) {
			getdir2(fpath , cur);
		} else {
			continue;
		}
	}
	closedir(dp);
	return;
}

  
//Get file attributes.
static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");

	sprintf(fpath, "%s%s",dirpath,path);
	
	if(encv1 != NULL){
        decript1(encv1);
        sprintf(fpath,"%s/%s",dirpath,encv1);
	}
	
	res = lstat(fpath, stbuf);
	char rr[1000];
	strcpy(rr,path);
	logsys(0 ,"LS" ,rr);
	
	if (res == -1)
		return -errno;
	
	return 0;
}


//Read the target of a symbolic link
static int xmp_readlink(const char *path, char *buf, size_t size)
{
     int res;
	char fpath[1000];	
	sprintf(fpath, "%s%s",dirpath,path);
	
    res = readlink(fpath, buf, size - 1);
	char rr[1000];
	strcpy(rr,path);
	logsys(0, "READLINK" ,rr);
	if (res == -1)
        return -errno;
    buf[res] = '\0';
    return 0;
}


//Read directory
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
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
	char rr[1000];
	strcpy(rr,path);	
	logsys(0, "CD" , rr);
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


//Rename a file
static int xmp_rename(const char *from, const char *to)
{
        int res;
		char from1[1000];
		char to1[1000];
		char *ffrom1 = strstr(from,"encv1_");
		char *fto1 = strstr(to,"encv1_");
		char *ffrom2 = strstr(from,"encv2_");
		char *fto2 = strstr(to,"encv2_");		
		sprintf(from1,"%s%s",dirpath,from);
		sprintf(to1,"%s%s",dirpath,to);
		
		if(ffrom1 != NULL && fto1 == NULL){
			char str[1000];
			sprintf(str,"%s::%s",from,to);
			logsys(0,"SYMLINK",str);
		}
		
		if(fto1 != NULL && ffrom1 == NULL){
			decript1(fto1);
			sprintf(to1,"%s/%s",dirpath,fto1);
		}
		if(ffrom2 != NULL && fto2 == NULL){
			getdir2(from1,0);
		}
		
		if(ffrom2 == NULL && fto2 != NULL){
			getdir2(from1,1);
		}
		
        res = rename(from1, to1);
		char str[1000];
		sprintf(str,"%s::%s",from,to);
		logsys(0,"RENAME",str);
        if (res == -1)
                return -errno;
        return 0;
}


//Read data from an open file
static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;
    char fpath[1000];

	sprintf(fpath, "%s%s",dirpath,path);
	
	(void) fi;
	
	fd = open(fpath, O_RDONLY);
	char rr[1000];
	strcpy(rr,path);
	logsys(0, "READ", rr);
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
		sprintf(fpath ,"%s%s",dirpath,path);
		char rr[1000];
		strcpy(rr,path);
		logsys(0, "MKNOD", rr);

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
		sprintf(fullpath ,"%s%s",dirpath,path);
        res = mkdir(fullpath, mode);
		char rr[1000];
		strcpy(rr,path);
		logsys(0,"MKDIR",rr);
        if (res == -1)
                return -errno;
        return 0;
}

//Remove a file
static int xmp_unlink(const char *path)
{
        int res;
		char fpath[1000];
		sprintf(fpath,"%s%s",dirpath,path);
        res = unlink(fpath);
		char rr[1000];
		strcpy(rr,path);
		logsys(1,"UNLINK",rr);
        if (res == -1)
                return -errno;
        return 0;
}

//Remove a directory
static int xmp_rmdir(const char *path)
{
        int res;
		char fpath[1000];
		sprintf(fpath,"%s%s",dirpath,path);
        res = rmdir(fpath);
		char rr[1000];
		strcpy(rr,path);
		logsys(1,"RMDIR",rr);
        if (res == -1)
                return -errno;
        return 0;
}


static int xmp_symlink(const char *from, const char *to)
{
        int res;
		char from1[1000];
		char to1[1000];
		sprintf(from1,"%s%s",dirpath,from);
		sprintf(to1,"%s%s",dirpath,to);
        res = symlink(from1, to1);
		char str[1000];
		sprintf(str,"%s::%s",from,to);
		logsys(0,"SYMLINK",str);
        if (res == -1)
                return -errno;
        return 0;
}


static int xmp_link(const char *from, const char *to)
{
        int res;
		char from1[1000];
		char to1[1000];
		sprintf(from1,"%s%s",dirpath,from);
		sprintf(to1,"%s%s",dirpath,to);
        res = link(from1, to1);
		char str[1000];
		sprintf(str,"%s::%s",from,to);
		logsys(0,"LINK",str);
        if (res == -1)
                return -errno;
        return 0;
}

//Change the permission bits of a file
static int xmp_chmod(const char *path, mode_t mode)
{
    int res;
		char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
    res = chmod(fpath, mode);
	char rr[1000];
	strcpy(rr,path);
	logsys(0,"CHMOD", rr);
    if (res == -1)
        return -errno;
    return 0;
}

//Change the owner and group of a file
static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
    int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
    res = lchown(fpath, uid, gid);
	char rr[1000];
	strcpy(rr,path);
	logsys(0,"CHOWN",rr);
    if (res == -1)
        return -errno;
    return 0;
}

//Change the size of a file
static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = truncate(fpath, size);
	char rr[1000];
	strcpy(rr,path);
	logsys(0,"TRUNCATE",rr);
    if (res == -1)
        return -errno;
    return 0;
}


static struct fuse_operations xmp_oper = {
	
.getattr 			= xmp_getattr,
.readdir 			= xmp_readdir,
.readlink			= xmp_readlink,
.read 				= xmp_read,
.mkdir 				= xmp_mkdir,
.mknod 			        = xmp_mknod,
.symlink 			= xmp_symlink,
.unlink 			= xmp_unlink,
.rmdir 				= xmp_rmdir,
.rename 			= xmp_rename,
.link 				= xmp_link,
.chmod 			        = xmp_chmod,
.chown 				= xmp_chown,
.truncate 			= xmp_truncate,

};


int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
