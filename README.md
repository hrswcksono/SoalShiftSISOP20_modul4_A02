# SoalShiftSISOP20_modul4_A02

## Muhamad Haris W - 05111840000029
## Muhammad Rivadhli P - 05111840000128


## Soal
Di suatu perusahaan, terdapat pekerja baru yang super jenius, ia bernama jasir. Jasir baru bekerja selama seminggu di perusahan itu, dalam waktu seminggu tersebut ia selalu terhantui oleh ketidak amanan dan ketidak efisienan file system yang digunakan perusahaan tersebut. Sehingga ia merancang sebuah file system yang sangat aman dan efisien. Pada segi keamanan, Jasir telah menemukan 2 buah metode enkripsi file. Pada mode enkripsi pertama, nama file-file pada direktori terenkripsi akan dienkripsi menggunakan metode substitusi. Sedangkan pada metode enkripsi yang kedua, file-file pada direktori terenkripsi akan di-split menjadi file-file kecil. Sehingga orang-orang yang tidak menggunakan filesystem rancangannya akan kebingungan saat mengakses direktori terenkripsi tersebut. Untuk segi efisiensi, dikarenakan pada perusahaan tersebut sering dilaksanakan sinkronisasi antara 2 direktori, maka jasir telah merumuskan sebuah metode agar filesystem-nya mampu mengsingkronkan kedua direktori tersebut secara otomatis. Agar integritas filesystem tersebut lebih terjamin, maka setiap command yang dilakukan akan dicatat kedalam sebuah file log.
(catatan: filesystem berfungsi normal layaknya linux pada umumnya)
(catatan: mount source (root) filesystem adalah direktori /home/[user]/Documents)

Berikut adalah detail filesystem rancangan jasir:
1. Enkripsi versi 1:
   1. Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
   2. Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
   3. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.
   4. Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
   5. Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.

![image](https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/111.png)

Misal kan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10
“**encv1_rahasia/FOTO_PENTING/kelincilucu.jpg**” => “**encv1_rahasia/ULlL@u]AlZA(/g7D.|\_.Da_a.jpg**
**Note** : Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt.
   6. Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.

2. Enkripsi versi 2:
   1. Jika sebuah direktori dibuat dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.
   2. Jika sebuah direktori di-rename dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.
   3. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi direktori tersebut akan terdekrip.
   4. Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
   5. Pada enkripsi v2, file-file pada direktori asli akan menjadi bagian-bagian kecil sebesar 1024 bytes dan menjadi normal ketika diakses melalui filesystem rancangan jasir. Sebagai contoh, file File_Contoh.txt berukuran 5 kB pada direktori asli akan menjadi 5 file kecil yakni: File_Contoh.txt.000, File_Contoh.txt.001, File_Contoh.txt.002, File_Contoh.txt.003, dan File_Contoh.txt.004.
   6. Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lain yang ada didalam direktori tersebut (rekursif).

3. Sinkronisasi direktori otomatis:

Tanpa mengurangi keumuman, misalkan suatu directory bernama dir akan tersinkronisasi dengan directory yang memiliki nama yang sama dengan awalan sync_ yaitu sync_dir. Persyaratan untuk sinkronisasi yaitu:
   1. Kedua directory memiliki parent directory yang sama.
   2. Kedua directory kosong atau memiliki isi yang sama. Dua directory dapat dikatakan memiliki isi yang sama jika memenuhi:
      1. Nama dari setiap berkas di dalamnya sama.
      2. Modified time dari setiap berkas di dalamnya tidak berselisih lebih dari 0.1 detik.
   3. Sinkronisasi dilakukan ke seluruh isi dari kedua directory tersebut, tidak hanya di satu child directory saja.
   4. Sinkronisasi mencakup pembuatan berkas/directory, penghapusan berkas/directory, dan pengubahan berkas/directory.

Jika persyaratan di atas terlanggar, maka kedua directory tersebut tidak akan tersinkronisasi lagi.
Implementasi dilarang menggunakan symbolic links dan thread.

4. Log system:

   1. Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan.
   2. Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.
   3. Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.
   4. Sisanya, akan dicatat dengan level INFO.
   5. Format untuk logging yaitu:

![image](https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/222.png)


LEVEL    : Level logging
yy   	 : Tahun dua digit
mm    	 : Bulan dua digit
dd    	 : Hari dua digit
HH    	 : Jam dua digit
MM    	 : Menit dua digit
SS    	 : Detik dua digit
CMD     	 : System call yang terpanggil
DESC      : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)

Contoh format logging nantinya seperti:

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/333.png" width="920" height="130">


## Source Code

```javascript
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

void logsys(int level,char *call,const char *info){
	FILE *log = fopen("/home/haris/Desktop/fs.log" , "a");
	time_t now = time(0);
	struct tm tstruct = *localtime(&now);
	int tahun=0,bulan=0,hari=0,jam=0,menit=0,detik=0;
	tahun = tstruct.tm_year + 1900;
	bulan = tstruct.tm_mon + 1;
	hari = tstruct.tm_mday;
	jam = tstruct.tm_hour;
	menit = tstruct.tm_min;
	detik = tstruct.tm_sec;
	if(level == 1){
		fprintf(log, "WARNING::%02d%02d%02d-%02d:%02d:%02d::%s::%s\n",tahun%100,bulan,hari,jam,menit,detik,call,info);
	}
	else{
		fprintf(log, "INFO::%02d%02d%02d-%02d:%02d:%02d::%s::%s\n",tahun%100,bulan,hari,jam,menit,detik,call,info);
	}
	fclose(log);
}

void encryptdecrypt(char *level,  char *from , char *to){
	FILE *log = fopen("/home/haris/Desktop/data.log" , "a");
	time_t now = time(0);
	struct tm tstruct = *localtime(&now);
	int tahun=0,bulan=0,hari=0,jam=0,menit=0,detik=0;
	tahun = tstruct.tm_year + 1900;
	bulan = tstruct.tm_mon + 1;
	hari = tstruct.tm_mday;
	jam = tstruct.tm_hour;
	menit = tstruct.tm_min;
	detik = tstruct.tm_sec;
	if(to != NULL){
		fprintf(log,"%04d-%02d-%02d   %02d:%02d:%02d : %s\nform\t : %s\nto\t : %s\n",tahun,bulan,hari,jam,menit,detik,level,from,to);
	}
	else{
		fprintf(log,"%04d-%02d-%02d   %02d:%02d:%02d : %s\nform\t : %s\n",tahun,bulan,hari,jam,menit,detik,level,from);
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
	
	if (res == -1)
		return -errno;
	
	return 0;
}


//Read the target of a symbolic link
static int xmp_readlink(const char *path, char *buf, size_t size)
{
	char rr[1000];
	strcpy(rr,path);
	logsys(0, "READLINK" ,rr);
    	int res;
	char fpath[1000];	
	sprintf(fpath, "%s%s",dirpath,path);
	
    	res = readlink(fpath, buf, size - 1);

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
		encryptdecrypt("decrypt versi 1 RENAME", from1 , to1);
	}
	
	if(fto1 != NULL && ffrom1 == NULL){
		encryptdecrypt("encrypt versi 1 RENAME", from1 , to1);
		decript1(fto1);
		sprintf(to1,"%s/%s",dirpath,fto1);
	}
	if(ffrom2 != NULL && fto2 == NULL){
		encryptdecrypt("decrypt versi 2 RENAME", from1 , to1);
		getdir2(from1,0);
	}
		
	if(ffrom2 == NULL && fto2 != NULL){
		encryptdecrypt("encrypt versi 2 RENAME", from1 , to1);
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
	char rr[1000];
	strcpy(rr,path);
	logsys(0, "READ", rr);
	int fd;
	int res;
    	char fpath[1000];

	sprintf(fpath, "%s%s",dirpath,path);
	
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
	logsys(0,"MKDIR",path);
        int res;
	char fpath[1000];
	sprintf(fpath ,"%s%s",dirpath,path);
	if(strstr(fpath,"encv1_") != NULL){
		encryptdecrypt("encrypt versi 1 MKDIR", fpath, NULL);
	}
	if(strstr(fpath,"encv2_") != NULL){
		encryptdecrypt("encrypt versi 2 MKDIR", fpath , NULL);
	}
        res = mkdir(fpath, mode);
        if (res == -1)
                return -errno;
        return 0;
}

//Remove a file
static int xmp_unlink(const char *path)
{
	logsys(1,"UNLINK",path);
        int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
        res = unlink(fpath);

        if (res == -1)
                return -errno;
        return 0;
}

//Remove a directory
static int xmp_rmdir(const char *path)
{
	logsys(1, "RMDIR" ,path);
        int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
        res = rmdir(fpath);
        if (res == -1)
                return -errno;
        return 0;
}


static int xmp_symlink(const char *from, const char *to)
{
	char str[1000];
	sprintf(str,"%s::%s",from,to);
	logsys(0,"SYMLINK",str);
        int res;
        res = symlink(from, to);

        if (res == -1)
                return -errno;
        return 0;
}


static int xmp_link(const char *from, const char *to)
{
		char str[1000];
		sprintf(str,"%s::%s",from,to);
		logsys(0,"LINK",str);
        int res;
        res = link(from, to);
        if (res == -1)
                return -errno;
        return 0;
}

//Change the permission bits of a file
static int xmp_chmod(const char *path, mode_t mode)
{
	logsys(0,"CHMOD", path);
    int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
    res = chmod(fpath, mode);
    if (res == -1)
        return -errno;
    return 0;
}

//Change the owner and group of a file
static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	logsys(0,"CHOWN",path);
    	int res;
    	res = lchown(path, uid, gid);
    	if (res == -1)
        	return -errno;
    return 0;
}

//Change the size of a file
static int xmp_truncate(const char *path, off_t size)
{
	logsys(0,"TRUNCATE",path);
	int res;
	res = truncate(path, size);
    	if (res == -1)
        return -errno;
    return 0;
}


static struct fuse_operations xmp_oper = {
	
.getattr 		= xmp_getattr,
.readdir 		= xmp_readdir,
.readlink		= xmp_readlink,
.read 			= xmp_read,
.mkdir 			= xmp_mkdir,
.mknod 			= xmp_mknod,
.symlink 		= xmp_symlink,
.unlink 		= xmp_unlink,
.rmdir 			= xmp_rmdir,
.rename 		= xmp_rename,
.link 			= xmp_link,
.chmod 			= xmp_chmod,
.chown 			= xmp_chown,
.truncate 		= xmp_truncate,

};


int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
```
Direktori fuse

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/1.png" width="500" height="350">

Mengganti nama direktori abc dengan encv1_abc (enkripsi versi satu)

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_1.png" width="500" height="350">

Didalamnya akan terenkripsi

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_2.png" width="500" height="350">

Setelah direname jadi abc lagi (dekripsi versi satu) maka akan kembali seperti semula

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_3.png" width="500" height="350">

Mengganti nama direktori hij dengan encv2_hij (enkripsi versi dua)

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_12.png" width="500" height="350">

Didalamnya akan terenkripsi

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_4.png" width="500" height="350">

Dan didalamnya lagi berlaku rekursif

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_5.png" width="500" height="350">

Setelah direname jadi hij lagi (dekripsi versi dua) maka akan kembali seperti semula

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_11.png" width="500" height="350">

Kemudian buat direktori baru encv1_ dan encv2_

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_6.png" width="500" height="350">

Dan log nya adalah

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_7.png" width="500" height="350">

Kemudian beberapa aktifitas yang dilakukan

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_10.png" width="500" height="350">

Ini adalah catatan lognya

<img src="https://github.com/hrswcksono/SoalShiftSISOP20_modul4_A02/blob/master/gambar/Screenshot_9.png" width="500" height="350">
