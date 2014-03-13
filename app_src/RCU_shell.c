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
#include <stdint.h> /* uint32_t etc.*/
#include <stdlib.h> /* exit() */

uint32_t registerAccess(uint32_t, uint32_t, char *);

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

const char * dev_name = "/dev/sample2";

int main(int argc, char **argv)
{
	/* Check for arguments and print help prompt */
	if(argc < 3){
		//fprintf(stderr, "");
		printHelp();
		return -1;
	}
	
	uint32_t addr;
	uint32_t data = 0x0;

	/* Command parsing */
	int m, n,      /* Loop counters */
	    ch;        /* character buffer */
	    
	/* File pointer for choosing where to put results */
	FILE *fp = NULL;
	
	/* Go through arguments from last to first 
	 * This is done so that additional options
	 * are scanned before read/write options.
	*/
	
	for (n = argc-1; n > 0; n-- )
	{
	fprintf(stdout, "argc=%d, argv(n=%d) = %s\n", argc, n, argv[n] );
/*
		if(argv[n][0] == '-' )
		{
			switch( (int)argv[n][1] )
			{
*/
			/* Write output to file */
/*
			const char * strMode = "w";
			case 'o':
			case 'a':
				if (argc > n){
					if ((int)argv[n][1] == 'a' ) strMode = "a";
						fp = fopen(argv[n+1], strMode);
						if(fp == NULL){
						fprintf(stderr, "Could not open file %s for writing.\n", argv[n+1]);
						}
				} 
				else fprintf(stderr, "Missing argument to option %s \n", argv[n]);
				break;
*/
			/* Format string */
/*	
			case 'f':
				if(argc > n){
					
				}
				else fprintf(stderr, "Missing argument to option %s \n", argv[n]);
				break;
			}
		}
		else
*/
		switch( (int)argv[n][0] )
		{
		/* Read data in address */
		case 'r':
			if(argv[n][1]) break; /* Temporary solution */
			char temp[2];
			temp[0] = argv[n+2][0];
			temp[1] = argv[n+2][1];
			fprintf(stdout, "%s\n", argv[n+1]);
				if(strcmp(temp, "0x") == 0){
					fprintf(stderr, "Read does not take a hex value...\n");
					exit(1);	
				}
			addr = parseNumber(argv[n+1]);
			data = registerAccess(addr, 0x0, "r");
			/* Print results */
			if(!fp) fp=stdout;
			fprintf(fp, "%#x\n", data);
			break;
		/* Write data to address */
		case 'w':
			if(argv[n][1]) break; /* */
			addr = parseNumber(argv[n+1]);
			if (argc < 4){printHelp(); break;}
			else
			data = parseNumber(argv[n+2]);
			data = registerAccess(addr, data, "w");
			/* Print results */
			if(!fp) fp=stdout;
			fprintf(fp, "%#x\n", data);
			break;
		/* Write 0x0 to address */
		case 'c':
			if(argv[n][1]) break;
			/* Write zero to register address */
			addr = parseNumber(argv[n+1]);
			data = 0x0;
			data = registerAccess(addr, data, "w");
			/* Print results */
			if(!fp) fp=stdout;
			fprintf(fp, "%#x\n", data); /* The printing should happen outside the loop, this means we fill up a buffer*/
			break;
			
		/* Here comes the additional options */
		case '-':
			switch( (int)argv[n][1] )
			{
			/* Write output to file */
			case 'o':
			case 'a':
				if (argc > n){
					const char * strMode = "w";
					if ((int)argv[n][1] == 'a' ) strMode = "a";
					fp = fopen(argv[n+1], strMode);
					if(fp == NULL){
						fprintf(stderr, "Could not open file %s for writing.\n", argv[n+1]);
						exit(1);
					}
				}
				else fprintf(stderr, "Missing argument to option %s \n", argv[n]);
				break;
			/* Format string */
			case 'f':
				if(argc > n){
					
				}
				else fprintf(stderr, "Missing argument to option %s \n", argv[n]);
				break;
			}
			break;
		case '/': /* yeah, not really good - a lot of errors can occur*/
		case '0':
			break;
		default:
			fprintf(stderr, "Error, oops, we tripped\n");
			//exit(1);
			//return 1;
			break;
		}
	
	}

	/* This is the end */
 	if( fp != stdout ) fclose(fp);
	return 0;
}

/*
 * register access function
*/
uint32_t registerAccess(uint32_t address, uint32_t data, char * rORw)
{
		uint32_t c, x;
		int fd = -1;
		/* Read procedure */
		if (strcmp(rORw, "r") == 0) {
			if ((fd = open(dev_name, O_RDWR)) < 0) {
				fprintf(stderr, "unable to open %s: %s\n", dev_name, strerror(errno));
			    exit(-1);
				}
			/* First we seek */
			if ((x = lseek(fd, address, SEEK_SET)) < 0) {
				fprintf(stderr, "unable to seek %s: %s\n", dev_name, strerror(errno));
				exit(-1);
			}
			/* Second we read the requested memory */
			if ((x = read(fd, &c, 4)) < 0) {
				fprintf(stderr, "unable to read %s: %s\n", dev_name, strerror(errno));
	        	exit(-1);
			}
			if (fd >= 0) {
				close(fd);
			}	
			return c;
/*			fprintf(stdout, "0x%x\n", c); */
		}
		/* Write procedure */
		else if (strcmp(rORw, "w") == 0) {
			if ((fd = open(dev_name, O_RDWR)) < 0) {
				fprintf(stderr, "unable to open %s: %s\n", dev_name, strerror(errno));
			    exit(-1);
				}
			/* First we seek */
			if ((x = lseek(fd, address, SEEK_SET)) < 0) {
			fprintf(stderr, "unable to seek %s: %s\n", dev_name, strerror(errno));
			}
			/* Second we read the requested memory */
			if ((x = write(fd, (char*) data, sizeof(data))) < 0) {
				fprintf(stderr, "unable to write %s: %s\n", dev_name, strerror(errno));
			}
			/*fprintf(stdout, "0x%x\n", c);*/
			return data;
		}
		/* That's the two options we have! */
		else {
			fprintf(stderr, "registerAccess(): Something went wrong trying to understand what you wanted to do with the memory\n");
			return -1;
		}
}
