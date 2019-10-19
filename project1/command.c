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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "command.h"

void listDir() /*for the ls command*/
{
	/* Variable assignment */
	char cdir[256];
	DIR* dir;
	struct dirent* dp;

	/* Get current directory */
	if(getcwd(cdir, sizeof(cdir)) == NULL)
	{
		const char error[] = "Error with getcwd()\n";
		write(1, error, sizeof(error) - 1);
		exit(1);
	}

	/* Open current directory with opendir */
	if((dir = opendir(cdir)) == NULL)
	{
		char str[256];
		sprintf(str, "Cannot open directory: %s\n", cdir);
		write(1, str, strlen(str));
		exit(1);
	}

	/* Write directory contents to standard output */
	while((dp = readdir(dir)) != NULL)
	{
		char* file_name = dp->d_name;
		char str[80];
		sprintf(str, "%s\n", file_name);
		file_name = NULL;
		write(1, str, strlen(str));
	}

	closedir(dir);
}

void showCurrentDir() /*for the pwd command*/
{

}

void makeDir(char *dirName) /*for the mkdir command*/
{

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
