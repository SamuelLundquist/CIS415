/*
* Description: <write a brief description of your lab>
*
* Author: Samuel Lundquist
*
* Date: October 11th, 2019
*
* Notes:
* 1.
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/

/*-----------------------------Program Main----------------------------------*/
int main(int argc, char **argv) {
	setbuf(stdout, NULL);

	/* Main Function Variables */
  char* buf;
	size_t bsize = 32;
	const char s[2] = " ";
	char *token;
	int tnum = 0;
  int running = 1;

	/* Allocate memory for the input buffer. */
  buf = (char*)malloc(bsize * sizeof(char));

	/*main run loop*/

		/* Print >>> then get the input string */
    FILE* read_file = fopen(argv[1], "r");

    getline(&buf, &bsize, read_file);

		/* Tokenize the input string */
    token = strtok(buf, s);

		/* Display each token */
    while(token != NULL)
  	{
  		printf("\nT%d: %s", tnum, token);
  		token = strtok(NULL, s);
  		tnum += 1;
  	}

	/*Free the allocated memory*/
  free(buf);

	return 1;
}
/*-----------------------------Program End-----------------------------------*/
