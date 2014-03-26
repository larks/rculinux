#include <stdio.h>
#include <errno.h>	/* Error numbers */
#include <stddef.h> /* offsetof() */
#include <stdint.h> /* int types */
#include <sys/mman.h> /* mmap */
#include <sys/stat.h>
#include <fcntl.h> /* O_RDWR */
#include <stdlib.h> /* exit() */
#include <string.h> /* memset() */

char** tokenize(const char*);
char ***alloc_array(int, int);

int main()
{
	FILE * fd = NULL;
	if ((fd = fopen("pedestal_ramp.script", "r")) < 0) {
	//if ((fd = fopen("test.txt", "r")) < 0) {
		fprintf(stderr, "unable to open file\n");
	    exit(-1);
	}
//	char *tempbuf;
	//char **arguments;
	off_t fileSize;
	/* Second we read the requested memory */
	fseek(fd, 0, SEEK_END);
	fileSize = ftell(fd);
	rewind(fd);
	fprintf(stdout, "fileSize: %d\n", (uint32_t)fileSize);
	
	/* 
	 * Count number of lines 
	 * Also count number of strings and find out 
	 * number of cols we need for storing arguments
	 */
	int kar; /* Character holder */
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
		if( (kar != ' ') && (!comment) ) karteller++;
		if( (kar == ' ') && (!comment) ){
			strengteller++;
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
			current_pos = ftell(fd)+2;
				int l;
				char vikar[storkar];
				char ***argBuf;
				argBuf = alloc_array(1, biggest);
				fseek(fd, last_pos, SEEK_SET); // seek to start of line
				last_pos = current_pos;
				while( (kar=fgetc(fd)) != '\n' ){
					if( kar == '#' ) comment = 1;
					if( (kar != ' ') && (!comment) ){
						vikar[karteller] = kar;
						karteller++;
					}
					if( (kar == ' ') && (!comment) ){
						strncpy(argBuf[0][strengteller], vikar, karteller);
						strengteller++;
						karteller = 0;
					}
					if( ftell(fd) == last_pos-3) strncpy(argBuf[0][strengteller], vikar, karteller);
				}
				
				
			comment = 0;
			fseek(fd, last_pos, SEEK_SET); //
			int a;
			for(a=0; a!=biggest; a++){
				fprintf(stdout, "argBuf[0][%d]: %s ", a, argBuf[0][a]);
			}
			fprintf(stdout, "\n");
			free(argBuf);
		}
	}
//	rewind(fd);
	
	if (fd >= 0) {
		fclose(fd);
	}
	return 0;	
}

char** tokenize(const char* input)
{
    char* str = strdup(input);
    int count = 0;
    int capacity = 10;
    char** result = malloc(capacity*sizeof(*result));

    char* tok=strtok(str," "); 

    while(1)
    {
        if (count >= capacity)
            result = realloc(result, (capacity*=2)*sizeof(*result));

        result[count++] = tok? strdup(tok) : tok;

        if (!tok) break;

        tok=strtok(NULL," ");
    } 

    free(str);
    return result;
}
/*
char ***alloc_array(int rows, int columns) {
    char ***a = calloc(rows, sizeof(char **));
    int i;
    for(i = 0; i != rows; i++) {
        a[i] = calloc(columns, sizeof(char *));
    }
    return a;
}
*/
char ***alloc_array(int rows, int columns) {
    char ***a = (char ***) calloc(rows, sizeof(char **));
    int i,j;
    for(i = 0; i < rows; i++) {
        a[i] = (char **) calloc(columns, sizeof(char *));
        for(j = 0; j < columns; j++){
        	a[i][j] = (char *) calloc(rows, sizeof(char));
        }
    }
    return a;
}
