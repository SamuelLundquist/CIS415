/******************************** main.c ***************************************
* Program Name: Project 3
* Author: Samuel Lundquist
* Date: 12.6.2019
* Description:
* Notes:
*******************************************************************************/

/************************ Preprocessor Directives *****************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "commands.h"
/******************************************************************************/

/******************************* Globals **************************************/
int ENTRYVAL = 1;
static TQ *registry[MAXTOPICS];
pthread_mutex_t meowtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_meowtx = PTHREAD_COND_INITIALIZER;
int globalFlag = 1;
int cleanFlag = 1;
int PROG_STAT = 0;
unsigned int DELTA = 10.0;
/******************************************************************************/

/******************************* Macros ***************************************/
#define ERROR_HANDLER(message)                                             \
	do { PROG_STAT = 1; printf(message); exit(PROG_STAT);  \
	} while(0)
/******************************************************************************/

/****************************** Functions *************************************/
void exitStat(void)
{
	printf("\n-----------------------------------------------------------\n");
	if(PROG_STAT)
	{
		printf("Program failed\n\n");
	}
	else
	{
		printf("Exited successfully\n\n");
	}
}

void menuStart(char* filename)
{
	system("clear");
	printf("\n\t\t[--- INSTAQUACK ---] \n");
	printf("-----------------------------------------------------------\n");
	printf("\n\t Ready to read instructions from '%s'\n", filename);
	printf("\n\t Press enter to start program.\n\n");
	printf("-----------------------------------------------------------\n");
	getchar();
	system("clear");
	printf("\n\t\t[--- INSTAQUACK ---] \n");
	printf("-----------------------------------------------------------\n");
	printf("\nReading...\n");
}

void menuRun()
{
	system("clear");
	printf("\n\t\t[--- INSTAQUACK ---] \n");
	printf("-----------------------------------------------------------\n");
	printf("\n\t Ready to begin\n");
	printf("\n\t Press enter to start threads.\n\n");
	printf("-----------------------------------------------------------\n");
	getchar();
	system("clear");
	printf("\n\t\t[--- INSTAQUACK ---] \n");
	printf("-----------------------------------------------------------\n");
	printf("\nStarting...\n");
}

int checkArgv(char *argv[])
{
	int ret = 0;
	if (argv[1] == NULL || argv[2] != NULL)
	{
		ret = 1;
	}
	return ret;
}

TQ *findQueue(char *TQ_ID)
{
	TQ *queue;
	for(int i = 0; i < MAXTOPICS; i++)
	{
		queue = registry[i];
		if(strcmp(queue->topic, TQ_ID) == 0)
		{
			return queue;
		}
	}
	return NULL;
}

int enqueue(char *TQ_ID, topicEntry *TE)
{
	TQ *queue;
	int ret = 0;

	if((queue = findQueue(TQ_ID)) != NULL)
	{
		pthread_mutex_t mtx = queue->mutex;
		pthread_mutex_lock(&mtx);
		int t = queue->tail;
		if(queue->buffer[t].entryNum != -1)
		{
			TE->entryNum = ENTRYVAL;
			ENTRYVAL++;
			TE->t = clock();
			queue->buffer[t] = *TE;
			queue->entries++;
			t++;
			queue->tail = t % (queue->length+1);
			ret = 1;
		}
		pthread_mutex_unlock(&mtx);
	}
	return ret;
}

int dequeue(char *TQ_ID)
{
	TQ *queue;
	int ret = 0;

	if((queue = findQueue(TQ_ID)) != NULL)
	{
		clock_t newTime = clock();
		pthread_mutex_t mtx = queue->mutex;
		pthread_mutex_lock(&mtx);
		int h = queue->head;
		clock_t time = queue->buffer[h].t;
		newTime = newTime - time;
		double time_taken = ((double)newTime)/CLOCKS_PER_SEC;
		if(time_taken > 10.0 && queue->buffer[h].entryNum > 0)
		{
			printf("Dequeueing item\n");
			queue->buffer[queue->tail].entryNum = 0;
			queue->buffer[h].entryNum = -1;
			h++;
			queue->head = h % queue->length;
			ret = 1;
		}
		pthread_mutex_unlock(&mtx);
	}
	return ret;
}

int getEntry(char *TQ_ID, int lastEntry, topicEntry *TE)
{
	TQ *queue;
	int ret = 0;

	if((queue = findQueue(TQ_ID)) != NULL)
	{
		int i = queue->head;
		int entry = queue->buffer[i].entryNum;
		lastEntry++;
		while(entry > 0)
		{
			if(entry >= lastEntry)
			{
				*TE = queue->buffer[i];
				ret = (entry > lastEntry) ? entry : 1;
				break;
			}
			i++;
			entry = queue->buffer[i].entryNum;
		}
	}
	return ret;
}

void cancelThreads(int numPubs, int numSubs, pthread_t p[], pthread_t s[])
{
	int i, stat;
	for(i = 0; i < numPubs; i++)
	{
		stat = pthread_cancel(p[i]);
		if(stat != 0)
		{
			ERROR_HANDLER("[ERROR] pthread cancel");
		}
	}
	for(i = 0; i < numSubs; i++)
	{
		stat = pthread_cancel(s[i]);
		if(stat != 0)
		{
			ERROR_HANDLER("[ERROR] pthread cancel");
		}
	}
}

void joinThreads(int numPubs, int numSubs, pthread_t p[], pthread_t s[])
{
	int i;
	void *res;
	for(i = 0; i < numPubs; i++)
	{
		pthread_join(p[i], &res);
		if(res == PTHREAD_CANCELED)
		{
			printf("Canceled publisher thread.\n");
		}
		else
		{
			printf("Publisher thread terminated normally.\n");
		}
	}
	for(i = 0; i < numSubs; i++)
	{
		pthread_join(s[i], &res);
		if(res == PTHREAD_CANCELED)
		{
			printf("Canceled subscriber thread.\n");
		}
		else
		{
			printf("Subscriber thread terminated normally.\n");
		}
	}
}

void *publisher(void *args)
{
	int id;
	char **TQ_LIST;
	topicEntry *TE_LIST;
	id = ((threadargs *) args)->id;

	printf("Publisher: %d\n", id);

	pthread_mutex_lock(&meowtx);
	pthread_cond_wait(&cond_meowtx, &meowtx);
	pthread_mutex_unlock(&meowtx);
	/*
	int i = 0;
	while(TQ_LIST[i] != NULL)
	{
		topicEntry entry = TE_LIST[i];
		entry.pubID = id;
		if(enqueue(TQ_LIST[i], &entry))
		{
			printf("P:%d Enq - %s to %s\n", id, entry.photoCaption, TQ_LIST[i]);
			i++;
			sched_yield();
			sleep(1);
		}
		else
		{
			sched_yield();
		}
	}
	*/
	return NULL;
}

void *subscriber(void *args)
{
	int id, lastEntry, res;
	topicEntry *entry;
	char *TQ_ID;

	id = ((threadargs *) args)->id;
	lastEntry = 0;

	printf("Subscriber: %d\n", id);

	pthread_mutex_lock(&meowtx);
	pthread_cond_wait(&cond_meowtx, &meowtx);
	pthread_mutex_unlock(&meowtx);
	/*
	while(globalFlag)
	{
		if((res = getEntry(TQ_ID, lastEntry, entry)) > 0)
		{
			lastEntry = (res == 1) ? lastEntry+1 : res;
			printf("S:%d got entry: %d\n", id, entry->entryNum);
			sched_yield();
		}
		else
		{
			printf("S:%d could not get entry\n", id);
			sched_yield();
			sleep(1);
		}
	}
	*/
	return NULL;
}

void *cleanup()
{
	printf("Cleanup Thread Initialized.\n");

	pthread_mutex_lock(&meowtx);
	pthread_cond_wait(&cond_meowtx, &meowtx);
	pthread_mutex_unlock(&meowtx);
	/*
	while(globalFlag)
	{
		if(cleanFlag)
		{
			for(int i = 0; i < MAXTOPICS; i++)
			{
				char *TQ_ID = registry[i]->topic;
				printf("Cleanup: %s\n", TQ_ID);
				dequeue(TQ_ID);
			}
			cleanFlag = 0;
		}
		sched_yield();
	}
	*/
	return NULL;
}

void alarmHandler(int signal)
{
	cleanFlag = 1;
	alarm(2);
}
/******************************************************************************/

int main(int argc, char *argv[])
{
	int i;
	FILE *fileptr;
	size_t len = 0;
	int numPubs = 0;
	int numSubs = 0;

	if(checkArgv(argv))
	{
		ERROR_HANDLER("[ERROR] Invalid Arguments\n");
	}
	atexit(exitStat);

	/* Init Arrays for Publishers and Subscribers */
	pthread_t pubs[MAXPUBS];
	pthread_t subs[MAXSUBS];
	threadargs pubargs[MAXPUBS];
	threadargs subargs[MAXSUBS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	/* Start Menu */
	menuStart(argv[1]);

	/* Read File and Initialize Program */
	fileptr = fopen(argv[1], "r");
	if(fileptr == NULL)
	{
		ERROR_HANDLER("[ERROR] Failed to open file.\n");
	}
	while(fileptr != NULL)
	{
		char *line = NULL;
		char command[16];
		getline(&line, &len, fileptr);
		sscanf(line, "%s", command);

		if(strcmp(command, "create") == 0)
		{

		}
		else if(strcmp(command, "query") == 0)
		{

		}
		else if(strcmp(command, "add") == 0)
		{

		}
		else if(strcmp(command, "delta") == 0)
		{
			unsigned int d;
			sscanf(line, "%*s %d", d);
			DELTA = d;
		}
		else if(strcmp(command, "start") == 0)
		{
			/* Close file and Reset File Pointer */
			fclose(fileptr);
			fileptr = NULL;

			/* Init Cleanup Thread and Alarm */
			pthread_t cleanupThread;
			pthread_create(&cleanupThread, &attr, cleanup, NULL);
			signal(SIGALRM, alarmHandler);
			alarm(2);

			/* Display Menu Before Sending Broadcast Condition to Threads to Start */
			menuRun();

			/* pthread cond boadcast to start all threads, waits for them to start */
			pthread_mutex_lock(&meowtx);
			pthread_cond_broadcast(&cond_meowtx);
			pthread_mutex_unlock(&meowtx);
		}
		else
		{
			/* Print Error */
			char command[32];
			sscanf(line, "%32s", &command);
			printf("[ERROR] Unrecognized Command: %s\n", command);

			/* Close file and Reset File Pointer */
			fclose(fileptr);
			fileptr = NULL;

			/* Cancel Threads and Set Program Status to EXIT_FAILURE */
			cancelThreads(numPubs, numSubs, pubs, subs);
			PROG_STAT = 1;
		}
		free(line);
	}

	joinThreads(numPubs, numSubs, pubs, subs);
	globalFlag = 0;

	return PROG_STAT;
}
