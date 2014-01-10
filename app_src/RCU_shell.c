/*! \brief Main source for rcu-sh
 ** 
 ** 
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cmdInterpreter.h"


//const char * noArgPrompt[] = "enter operation (h/r/w):";

unsigned int parseBinary(char *str) {
  unsigned int val = 0;
  
  if (*str == 'b') {
    str++;
    while (*str) {
      if (*str == '0') {
        val <<= 1;
      } else if (*str == '1') {
        val = (val << 1) + 1;
      } else {
        goto binaryError;
      }
    }
  }
  return val;
 binaryError:
  fprintf(stderr,"Unrecognized numeric value: %s\n",str);
  return 999;
}

unsigned int parseNumber(char *str) {
  unsigned int addr = 0;

  if (!sscanf(str, "0x%x", &addr)) {
    if (!sscanf(str, "%u", &addr)) {
      addr = parseBinary(str);
    }
  }
  return addr;
}


int main(int argc, char **argv)
{
	/* Check for arguments and print help prompt */
	if(argc < 3){
		//fprintf(stderr, "");
		printHelp();
		return -1;
	}

	char * app_name = argv[0];
	char * dev_name = "/dev/sample2";
	int ret = -1;
	int fd = -1;
	int c, x;
	char * cmd =  argv[1];
	char * addr = argv[2];
	char * data = NULL;
	
	int doread=0;
	int dowrite=0;

	/* TODO: Change to switch statement */
	
	//switch(cmd)
	
	
	
	// read  
	if( (strcmp(cmd,"rd")==0 || strcmp(cmd,"r")==0 || strcmp(cmd,"read")==0) ){
		doread=1;
		dowrite=0;
	}
	// write
	if( (strcmp(cmd,"wr")==0 || strcmp(cmd,"w")==0 || strcmp(cmd,"write")==0) ){
		dowrite=1;
		doread=0;
		data = argv[3];
	}
	if( strcmp(cmd, "c") ){
		dowrite=1;
		doread=0;
		data="0x0";
	}
	
	// batch
	if( (strcmp(cmd,"b") == 0 || strcmp(cmd,"batch")) ){
		// do some file buffering
	}
	

	unsigned int intval ;
	unsigned int offset ;

	offset = parseNumber(addr);
//	offset <<= 4;
//	printf("this is the address: %u\n", offset); 
	intval = parseNumber(data);

	/*
	 * Open the sample device RD | WR
	 */
	if ((fd = open(dev_name, O_RDWR)) < 0) {
		fprintf(stderr, "%s: unable to open %s: %s\n", 
			app_name, dev_name, strerror(errno));
		goto Done;
	}
  
	/*
	 * Read the sample device byte-by-byte
	 */
	if ((x = lseek(fd, offset, SEEK_SET)) < 0) {
		fprintf(stderr, "%s: unable to seek %s: %s\n", 
			app_name, dev_name, strerror(errno));
		goto Done;
	}

	if(dowrite==1){
		if ((x = write(fd, (char*)intval, sizeof(intval))) < 0) {
			fprintf(stderr, "%s: unable to write %s: %s\n", 
				app_name, dev_name, strerror(errno));
		goto Done;
		}	
	}

	if(doread==1){
		if ((x = read(fd, &c, 4)) < 0) {
			fprintf(stderr, "%s: unable to read %s: %s\n", 
				app_name, dev_name, strerror(errno));
		goto Done;
		}	
		fprintf(stdout, "0x%x\n", c);
	}

  
	Done:
	if (fd >= 0) {
		close(fd);
	}
	return ret;
}
