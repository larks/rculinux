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
/*
struct ArgDef {
	char *e
};
*/
char **alloc_array(int);
uint32_t registerAccess(uint32_t, uint32_t, char *);
void executeCommands(uint32_t arg_count, char **arguments);
//char **createArgumentBufferFromFile(FILE *fp, );

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
/* File pointer for choosing where to put results */
FILE *fp = NULL;
FILE * fd = NULL;
int main(int argc, char **argv)
{
	/* Check for arguments and print help prompt */
	if(argc < 3){
		//fprintf(stderr, "");
		printHelp();
		return -1;
	}
//	int i; 
/*	for(i=0; i!=argc; i++)*/
/*	fprintf(stdout, "%s\n", argv[i]);*/
	
	/* Check for comments */
	uint32_t argcBuffer = 0;
	while( argcBuffer != argc ){
		if( argv[argcBuffer][0] == '#')
			break;
		argcBuffer++;
/*		fprintf(stdout, "argcBuf: %d\n", argcBuffer);*/
	}
//	fprintf(stdout, "argcBuf: %d\n", argcBuffer);
	
	/* check the first arguments */
	//if( (argv[1][0] == 'r') || 
	  //  (argv[1][0] == 'w') ||
	   // (argv[1][0] == 'c') ){
	   if(argv[1][0] == 'r'){
	    	//fprintf(stdout, "entered\n");
			executeCommands(argcBuffer, argv);
			//fprintf(stdout, "Hei\n");
	}
	/* Run command execution on each line in file */
	if(argv[1][0] == 'b'){
		//FILE * fd = NULL;
		fprintf(stdout, "argv[2]: %s", argv[2]);
		if ((fd = fopen(argv[2], "r")) < 0) {
			fprintf(stderr, "unable to open file\n");
		    exit(-1);
		}
		
		int kar; 
		int space_kar = 0; /* Character holder */
		uint32_t newline_count = 0; /* Line counter */
		unsigned long karteller = 0; /* Character counter */
		uint32_t strengteller = 0; /* String counter */
		uint32_t biggest = 0; /* Biggest number of strings */
		uint32_t storkar = 0; /* Hold the size of the largest string */
		uint8_t comment = 0; /* Indicate if we reached a comment */
		uint32_t current_pos = 0;
		uint32_t last_pos = ftell(fd);
		
		while ( (kar=fgetc(fd)) != EOF){
		if( kar == '#' ) comment = 1; /* we have a comment, stop counting */
		if( (kar != ' ') && (!comment) && (kar != '\r') ){
			karteller++;
			space_kar = 0;
		}
		if( (kar == ' ') && (!comment) ){
			if(space_kar<1){
				strengteller++;
			}
			++space_kar;
			if(karteller>storkar) storkar = karteller;
			karteller = 0;
		}
		if(kar == '\n'){
			strengteller++;
			newline_count++;
			if(strengteller>biggest) biggest=strengteller;
			strengteller=0;
			karteller = 0;
			comment = 0;
			current_pos = ftell(fd);
				//int l;
				char vikar[storkar];
				char **argBuf;
				argBuf = alloc_array(biggest);
				fseek(fd, last_pos, SEEK_SET); // seek to start of line
				last_pos = current_pos;
				while( (kar=fgetc(fd)) != '\n' ){
					if( kar == '#' ) comment = 1;
					if( (kar != ' ') && (!comment) && (kar != '\r') ){ 
						vikar[karteller] = kar;
						karteller++;
					}
					if( (kar == ' ') && (!comment) ){
						strncpy(argBuf[strengteller], vikar, karteller);
						strengteller++;
						karteller = 0;
						/*vikar = '\0';*/
						vikar[0] = '\0';
					}
				}
				strncpy(argBuf[strengteller], vikar, karteller);
				fseek(fd, last_pos, SEEK_SET);
				executeCommands(strengteller, argBuf);
				biggest = 0;
				storkar = 0;
				karteller = 0;
				comment = 0;
				strengteller = 0;
				
				free(argBuf);
		}
		}
		
	}
	/* This is the end */
	if (fd != NULL) fclose(fd); /* close batch file */
 	if( (fp != stdout) && (fp != NULL) ) fclose(fp);
	return 0;
}

void executeCommands(uint32_t arg_count, char **arguments)
{
	uint32_t addr;
	uint32_t data = 0x0;
	int n;      /* Loop counter */
//	    ch;        /* character buffer */
	/* Go through arguments from last to first 
	 * This is done so that additional options
	 * are scanned before read/write options.
	 */
	for (n = arg_count-1; n > 0; n-- )
	{
		switch( (int)arguments[n][0] )
		{
		/* Read data in address */
		case 'r':
			if(arguments[n][1]) break; /* Temporary solution */
			char temp[2];
			temp[0] = arguments[n+2][0];
			temp[1] = arguments[n+2][1];
			//fprintf(stdout, "%s\n", arguments[n+1]);
				if(strcmp(temp, "0x") == 0){
					fprintf(stderr, "Read does not take a hex value...\n");
					exit(1);	
				}
			addr = parseNumber(arguments[n+1]);
			if(parseNumber(arguments[n+2])){
				uint32_t iCnt;
				for(iCnt = addr; iCnt < (addr+parseNumber(arguments[n+2])); iCnt++ ){
					data = registerAccess(iCnt, 0x0, "r");
					if(!fp) fp=stdout;
					fprintf(fp, "%#x: %#x\n", iCnt, data);
				}
			}
			else{
				data = registerAccess(addr, 0x0, "r");
				/* Print results */
				if(!fp) fp=stdout;
				fprintf(fp, "%#x: %#x\n", addr, data);
			}
			break;
		/* Write data to address */
		case 'w':
			if(arguments[n][1]) break; /* */
				addr = parseNumber(arguments[n+1]);
			if (arg_count < 4){printHelp(); break;}
			else
				data = parseNumber(arguments[n+2]);
				data = registerAccess(addr, data, "w");
				data = registerAccess(addr, 0x0, "r");
			/* Print results */
			if(!fp) fp=stdout;
				fprintf(fp, "%#x: %#x\n", addr, data);
			break;
		/* Write 0x0 to address */
		case 'c':
			if(arguments[n][1]) break;
			/* Write zero to register address */
			addr = parseNumber(arguments[n+1]);
			data = 0x0;
			data = registerAccess(addr, data, "w");
			data = registerAccess(addr, 0x0, "r"); /*read back value*/
			/* Print results */
			if(!fp) fp=stdout;
			fprintf(fp, "%#x: %#x\n", addr, data); /* The printing should happen outside the loop, this means we fill up a buffer*/
			break;
			
		/* Here comes the additional options */
		case '-':
			switch( (int)arguments[n][1] )
			{
			/* Write output to file */
			case 'o':
			case 'a':
				if (arg_count > n){
					const char * strMode = "w";
					if ((int)arguments[n][1] == 'a' ) strMode = "a";
					fp = fopen(arguments[n+1], strMode);
					if(fp == NULL){
						fprintf(stderr, "Could not open file %s for writing.\n", arguments[n+1]);
						exit(1);
					}
				}
				else fprintf(stderr, "Missing argument to option %s \n", arguments[n]);
				break;
			/* Format string */
			case 'f':
				if(arg_count > n){
					
				}
				else fprintf(stderr, "Missing argument to option %s \n", arguments[n]);
				break;
			}
			break;
		case '/': /* yeah, not really good - a lot of errors can occur*/
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '#': /* Comments are thrown away */
			break;
		default:
			fprintf(stderr, "Error, oops, we tripped\n");
			//exit(1);
			//return 1;
			break;
		}
	
	}
	//return 0;
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
			if (fd >= 0) {
				close(fd);
			}	
			return data;
		}
		/* That's the two options we have! */
		else {
			fprintf(stderr, "registerAccess(): Something went wrong trying to understand what you wanted to do with the memory\n");
			return -1;
		}
}

char **alloc_array(int columns) {
    char **a = (char **) calloc(columns, sizeof(char *));
    int i;
    for(i = 0; i < columns; i++) {
        a[i] = (char *) calloc(1, sizeof(char ));
    }
    return a;
}
