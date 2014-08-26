#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "../drivers/query_ioctl.h"

int main(void)
{
    int fd;
    int bitstream_file;
    int nbytes;
    char * data; /* map bitstream to this */
    struct stat sbuf; /* for holding file stats */

    fd = open("/dev/sysctrl", O_RDWR);
    /* test file opening */

	//char * buf = (char *)malloc(sizeof(char)*sz);
    
    if((bitstream_file = open("/mnt/bitstreams/M2S_SOMblinkerfuckerfabricFF.spi", O_RDONLY)) < 0){
    	perror("open");
    	exit(1);
    }
    if(stat("/mnt/bitstreams/M2S_SOMblinkerfuckerfabricFF.spi", &sbuf)<0){
    	perror("stat");
    	exit(1);    
    }
    nbytes = sbuf.st_size;
    char * buf = (char *)malloc(sizeof(char)*nbytes);
    read(bitstream_file, buf, nbytes);
    /*
    if(( data = mmap( (caddr_t)0, sbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0 ) )<0){
    	perror("mmap");
    	exit(1);
    }
    */
    
    query_arg_t q;
    if(ioctl(fd, READ_IDCODE, &q) == -1){
        perror("IDCODE it ballsed up!\n");
        return 1;    
    } else
    fprintf(stdout, "this was the id code: %#02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n"
				, q.serial_number[15], q.serial_number[14]
				, q.serial_number[13], q.serial_number[12]
				, q.serial_number[11], q.serial_number[10]
				, q.serial_number[ 9], q.serial_number[ 8]
				, q.serial_number[ 7], q.serial_number[ 6]
				, q.serial_number[ 5], q.serial_number[ 4]
				, q.serial_number[ 3], q.serial_number[ 2]
				, q.serial_number[ 1], q.serial_number[ 0]
	);
	
	if(ioctl(fd, READ_USERCODE, &q) == -1){
        perror("USERCODE it ballsed up!\n");
        return 1;    
    } else 
	    fprintf(stdout, "user code:%#02x%02x%02x%02x\n", q.user_code[3],q.user_code[2],
		                                     q.user_code[1],q.user_code[0]
	    );
	if(ioctl(fd, READ_DESIGN_VERSION, &q) == -1){
        perror("DESIGNVER it ballsed up!\n");
        return 1;    
    } else 
	    fprintf(stdout, "design version:%#02x%02x\n", q.design_version[1], q.design_version[0]);
	fprintf(stdout, "sbuf.st_size: %d\n", (int)sbuf.st_size);
	/*
	int i;
	for(i = 0; i<24; i++){
		printf("%p: %#x\n", (buf+i), *(buf+i));
	}
	*/
	
	write(fd, buf, sbuf.st_size);
	
	if(ioctl(fd, PROG_PROGRAM, &q) == -1){
    	perror("program ioctl failed\n");
    	return 1;
    }
	
    /*
    data = mmap(kaddress, filepointer)
    memcpy(kaddress, filepointer, filesize)
    ioctl(fd, PROG_AUTHENTICATE, &q)
    
    void *mmap(void *addr, size_t len, int prot, int flags,
              int fildes, off_t off);
    
    if(ioctl(fd, PROG_AUTHENTICATE, &q) == -1){
    	perror("Authenticate ioctl failed\n");
    	return 1;
    } else
    	write(fd, &data, nbytes);
	*/
//    if(fd) close(fd);
	if(fd) close(fd);
    if(bitstream_file) close(bitstream_file);
    //if(bfd) {fclose(bfd); free(buf);} 
    return 0;

}
