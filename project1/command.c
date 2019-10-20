/*
* Description: Project 1 Commands c file.
*
* Author: Samuel Lundquist
*
* Date: 10/16/2019
*
* Notes:
*
*/
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "command.h"

void listDir() /*for the ls command*/
{
	/*
		List Current Directory Function

		- Uses getcwd to find current directory.
		- Filenames are then displayed with scandir
		so that they are displayed in proper order.
		- Four files are displayed per line, and are
		set with proper spacing so that they are in
		collumns. Unless the file name is larger than
		20 chars, in that case a single space is used
		after the file name.
	*/

	/* Variable assignment */
	char cdir[256];
	int i,n;
	struct dirent** dp;

	/* Get current directory */
	if(getcwd(cdir, sizeof(cdir)) == NULL)
	{
		const char error[] = "Error with getcwd()\n";
		write(1, error, sizeof(error) - 1);
		exit(1);
	}

	/* using scandir instead of opendir so that dir can be sorted on output */
	n = scandir(cdir, &dp, 0, alphasort);
	if (n < 0)
	{
		char str[256];
		sprintf(str, "Cannot scan directory: %s\n", cdir);
		write(1, str, strlen(str));
		exit(1);
	}

	/*
	* Iterate through files in directory.
	* Write 4 to standard out per line.
	* Spaces them apart equally for nice
	* easy to read format.
	*/
	for (i=0;i<n;i++)
	{
		char out[300];
		char spaces[20] = "";
		char* str = dp[i]->d_name;
		int len = strlen(str);
		int num_spcs = 20 - len;
		while(num_spcs)
		{
			strcat(spaces, " ");
			num_spcs--;
		}
		char a = ' ';
		if(((i+1) % 4 == 0) || i+1 == n)
		{
			a = '\n';
		}
		sprintf(out, "%s%s%c", str, spaces, a);
		write(1, out, strlen(out));
		free(dp[i]);
	}
	free(dp);
}

void showCurrentDir() /*for the pwd command*/
{
	/* Variable assignment */
	char cdir[256];

	/* Get current directory */
	if(getcwd(cdir, sizeof(cdir)) == NULL)
	{
		const char error[] = "Error with getcwd()\n";
		write(1, error, sizeof(error) - 1);
		exit(1);
	}

	/* Write directory to standard out */
	char str[256];
	sprintf(str, "Current Directory: %s\n", cdir);
	write(1, str, strlen(str));
}

void makeDir(char *dirName) /*for the mkdir command*/
{
	mode_t mode = 0777;

	if(mkdir(dirName, mode) == -1)
	{
		const char error[] = "Error with mkdir()\n";
		write(1, error, sizeof(error) - 1);
		exit(1);
	}
	if(chmod(dirName, mode) == -1)
	{
		const char error[] = "Error with chmod()\n";
		write(1, error, sizeof(error) - 1);
		exit(1);
	}
}

void changeDir(char *dirName) /*for the cd command*/
{

}

void copyFile(char *sourcePath, char *destinationPath) /*for the cp command*/
{

}

void moveFile(char *sourcePath, char *destinationPath) /*for the mv command*/
{

}

void deleteFile(char *filename) /*for the rm command*/
{

}

void displayFile(char *filename) /*for the cat command*/
{

}
