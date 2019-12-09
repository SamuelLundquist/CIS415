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
static TQ registry[MAXTOPICS];
pthread_mutex_t meowtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_meowtx = PTHREAD_COND_INITIALIZER;
int globalFlag = 1;
int cleanFlag = 1;
static int PROG_STAT = 0;
static int NUMTOPICS = 0;
float DELTA = 10.0;
pthread_t pubs[MAXPUBS];
pthread_t subs[MAXSUBS];
threadargs pubargs[MAXPUBS];
threadargs subargs[MAXSUBS];
pthread_attr_t attr;
/******************************************************************************/

/******************************* Macros ***************************************/
#define ERROR_HANDLER(message)                                             \
	do { PROG_STAT = 1; printf(message); exit(PROG_STAT);                  \
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
	printf("\n-----------------------------------------------------------\n");
	printf("\nReady to begin, press enter to start threads.\n");
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
	for(int i = 0; i < NUMTOPICS; i++)
	{
		queue = &registry[i];
		if(strcmp(queue->topic, TQ_ID) == 0)
		{
			return queue;
		}
	}
	return NULL;
}

TQ *findQueueFromID(int id)
{
	TQ *queue;
	for(int i = 0; i < NUMTOPICS; i++)
	{
		queue = &registry[i];
		if(queue->id == id)
		{
			return queue;
		}
	}
	return NULL;
}

int enqueue(int id, topicEntry *TE)
{
	TQ *queue;
	int ret = 0;

	if((queue = findQueueFromID(id)) != NULL)
	{
		pthread_mutex_t mtx = queue->mutex;
		pthread_mutex_lock(&mtx);
		int t = queue->tail;
		if(queue->buffer[t].entryNum != -1)
		{
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

int dequeue(int id)
{
	TQ *queue;
	int ret = 0;

	if((queue = findQueueFromID(id)) != NULL)
	{
		pthread_mutex_t mtx = queue->mutex;
		pthread_mutex_lock(&mtx);

		int h = queue->head;
		clock_t newTime = clock();
		clock_t time = queue->buffer[h].t;
		newTime = newTime - time;
		double time_taken = ((double)newTime)/CLOCKS_PER_SEC;

		if(time_taken > DELTA && queue->buffer[h].entryNum > 0)
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

int getEntry(int id, int lastEntry, topicEntry *TE)
{
	TQ *queue;
	int ret = 0;

	if((queue = findQueueFromID(id)) != NULL)
	{
		pthread_mutex_t mtx = queue->mutex;
		pthread_mutex_lock(&mtx);
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
		pthread_mutex_unlock(&mtx);
	}
	return ret;
}

int makeThread(int type, int index, char *filename)
{
	int s, res;
	res = 0;
	threadargs args = { .id = index+1 };
	strncpy(args.filename, filename, sizeof(args.filename) - 1);
	if(type == 1)
	{
		if(index > MAXPUBS)
		{
			printf("[ERROR] Maximum publishers already reached\n");
			res = 1;
		}
		else
		{
			pubargs[index] = args;
			s = pthread_create(&pubs[index], &attr, publisher, &pubargs[index]);
			if(s)
			{
				printf("[ERROR] Failed to create thread\n");
				res = 1;
			}
		}
	}
	else if(type == 2)
	{
		if(index > MAXSUBS)
		{
			printf("[ERROR] Maximum subscribers already reached\n");
			res = 1;
		}
		else
		{
			subargs[index] = args;
			s= pthread_create(&subs[index], &attr, subscriber, &subargs[index]);
			if(s)
			{
				printf("[ERROR] Failed to create thread\n");
				res = 1;
			}
		}
	}
	return res;
}

void joinThreads(int numPubs, int numSubs, pthread_t p[], pthread_t s[])
{
	int i, stat, id;
	void *res;
	for(i = 0; i < numPubs; i++)
	{
		id = pubargs[i].id;
		stat = pthread_join(p[i], &res);
		if(stat != 0)
		{
			ERROR_HANDLER("[ERROR] pthread join");
		}
		else
		{
			printf("Publisher thread %d terminated.\n", id);
		}
	}
	for(i = 0; i < numSubs; i++)
	{
		id = subargs[i].id;
		stat = pthread_join(s[i], &res);
		if(stat != 0)
		{
			ERROR_HANDLER("[ERROR] pthread join");
		}
		else
		{
			printf("Subscriber thread %d terminated.\n", id);
		}
	}
}

void *publisher(void *args)
{
	int id;
	char *filename;
	id = ((threadargs *) args)->id;
	filename = ((threadargs *) args)->filename;

	pthread_mutex_lock(&meowtx);
	pthread_cond_wait(&cond_meowtx, &meowtx);
	pthread_mutex_unlock(&meowtx);

	FILE *fileptr;
	size_t len = 0;

	fileptr = fopen(filename, "r");
	if(fileptr == NULL)
	{
		printf("[ERROR] Publisher failed to open file.\n");
		PROG_STAT = 1;
	}
	while(fileptr != NULL && PROG_STAT == 0)
	{
		char *line = NULL;
		char command[16];
		getline(&line, &len, fileptr);
		sscanf(line, "%15s", command);

		if(strcmp(command, "put") == 0)
		{
			int topicID;
			char url[255];
			char capt[255];
			sscanf(line,"put %d \"%254[^\"]\" \"%254[^\"]\"",&topicID,url,capt);
			if(topicID <= 0 || strcmp(url, "") == 0 || strcmp(capt, "") == 0)
			{
				printf("[ERROR] Invalid arguments for put command.\n");
				PROG_STAT = 1;
			}
			else
			{
				TE_DEF(entry, ENTRYVAL, id);
				ENTRYVAL++;
				strncpy(entry.photoURL, url, sizeof(entry.photoURL) - 1);
				strncpy(entry.photoCaption, capt, sizeof(entry.photoCaption)-1);
				if(enqueue(topicID, &entry))
				{
					printf("P:%d Enqueued: %s\n", id, entry.photoCaption);
				}
				else
				{
					printf("[ERROR] P:%D Enqueue failed: %s\n", id, capt);
				}
			}
		}
		else if(strcmp(command, "sleep") == 0)
		{
			int time;
			sscanf(line, "sleep %d", &time);
			if(time >= 0)
			{
				usleep(time);
			}
			else
			{
				printf("[ERROR] Invalid time for sleep command: %d\n", time);
				PROG_STAT = 1;
			}
		}
		else if(strcmp(command, "stop") == 0)
		{
			printf("Stopping publisher thread %d\n", id);
			free(line);
			break;
		}
		else
		{
			/* Print Error */
			char command[32];
			sscanf(line, "%s", command);
			printf("[ERROR] Unrecognized Command: %s\n", command);
			PROG_STAT = 1;
		}
		free(line);
		sched_yield();
	}
	fclose(fileptr);
	return NULL;
}

void *subscriber(void *args)
{
	int id;
	char *filename;
	id = ((threadargs *) args)->id;
	filename = ((threadargs *) args)->filename;

	pthread_mutex_lock(&meowtx);
	pthread_cond_wait(&cond_meowtx, &meowtx);
	pthread_mutex_unlock(&meowtx);

	FILE *fileptr;
	size_t len = 0;
	fileptr = fopen(filename, "r");
	if(fileptr == NULL)
	{
		printf("[ERROR] Subscriber failed to open file.\n");
		PROG_STAT = 1;
	}
	while(fileptr != NULL && PROG_STAT == 0)
	{
		char *line = NULL;
		char command[16];
		getline(&line, &len, fileptr);
		sscanf(line, "%15s", command);

		if(strcmp(command, "get") == 0)
		{
			int topicQueueID;
			sscanf(line, "get %d", &topicQueueID);
			int res;
			int lastEntry = 0;
			TE_DEF(entry, 0, 0);
			while((res = getEntry(topicQueueID, lastEntry, &entry)) > 0)
			{
				lastEntry = (res == 1) ? lastEntry+1 : res;
				printf("S:%d got entry: %d\n", id, entry.entryNum);
				sched_yield();
			}
		}
		else if(strcmp(command, "sleep") == 0)
		{
			int time;
			sscanf(line, "sleep %d", &time);
			if(time >= 0)
			{
				usleep(time);
			}
			else
			{
				printf("[ERROR] Invalid time for sleep command: %d\n", time);
				PROG_STAT = 1;
			}
		}
		else if(strcmp(command, "stop") == 0)
		{
			printf("Stopping subscriber thread %d\n", id);
			free(line);
			break;
		}
		else
		{
			/* Print Error */
			char command[32];
			sscanf(line, "%s", command);
			printf("[ERROR] Unrecognized Command: %s\n", command);
			PROG_STAT = 1;
		}
		free(line);
		sched_yield();
	}
	fclose(fileptr);
	return NULL;
}

void *cleanup()
{
	printf("Cleanup Thread Initialized.\n");

	pthread_mutex_lock(&meowtx);
	pthread_cond_wait(&cond_meowtx, &meowtx);
	pthread_mutex_unlock(&meowtx);

	while(globalFlag)
	{
		if(cleanFlag)
		{
			for(int i = 0; i < NUMTOPICS; i++)
			{
				int id = registry[i].id;
				printf("Cleanup: %d\n", id);
				dequeue(id);
			}
			cleanFlag = 0;
		}
		sched_yield();
	}
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
	FILE *fileptr;
	size_t len = 0;
	int numPubs = 0;
	int numSubs = 0;

	if(checkArgv(argv))
	{
		ERROR_HANDLER("[ERROR] Invalid Arguments\n");
	}
	atexit(exitStat);
	pthread_attr_init(&attr);

	/* Start Menu */
	menuStart(argv[1]);

	/* Read File and Initialize Program */
	fileptr = fopen(argv[1], "r");
	if(fileptr == NULL)
	{
		ERROR_HANDLER("[ERROR] Failed to open file.\n");
	}
	while(fileptr != NULL && PROG_STAT == 0)
	{
		char *line = NULL;
		char command[16];
		getline(&line, &len, fileptr);
		sscanf(line, "%15s", command);
		//usleep(250000);
		if(strcmp(command, "create") == 0)
		{
			int id = 0;
			int len;
			char name[128] = "";
			sscanf(line, "create topic %d \"%127[^\"]\" %d", &id, name, &len);
			if(id <= 0 || len <= 0 || strcmp(name, "") == 0)
			{
				printf("[ERROR] Couldn't create topic.\n");
				PROG_STAT = 1;
			}
			else
			{
				TQ_DEF(tq, id, len);
				tq.buffer = malloc(sizeof(topicEntry) * len+1);
				tq.buffer[len].entryNum = -1;
				registry[NUMTOPICS] = tq;
				NUMTOPICS++;
			}
		}
		else if(strcmp(command, "query") == 0)
		{
			char var[64];
			int i;
			sscanf(line, "query %63s", var);
			if(strcmp(var, "publishers") == 0)
			{
				for(i = 0; i < numPubs; i++)
				{
					threadargs args = pubargs[i];
					printf("Publisher: %d File: %s\n", args.id, args.filename);
				}
			}
			else if(strcmp(var, "subscribers") == 0)
			{
				for(i = 0; i < numSubs; i++)
				{
					threadargs args = subargs[i];
					printf("Subscriber: %d File: %s\n", args.id, args.filename);
				}
			}
			else if(strcmp(var, "topics") == 0)
			{
				for(i = 0; i < NUMTOPICS; i++)
				{
					TQ tq = registry[i];
					printf("Topic: %d Length: %d\n", tq.id, tq.length);
				}
			}
			else
			{
				printf("[ERROR] Unrecognized item to query: %s\n", var);
				PROG_STAT = 1;
			}
		}
		else if(strcmp(command, "add") == 0)
		{
			char var[64];
			char filename[128];
			int res = 0;
			sscanf(line, "add %63s \"%127[^\"]\"", var, filename);
			if(strcmp(var, "publisher") == 0)
			{
				res = makeThread(1, numPubs, filename);
				if(res == 0)
				{
					numPubs++;
				}
			}
			else if(strcmp(var, "subscriber") == 0)
			{
				res = makeThread(2, numSubs, filename);
				if(res == 0)
				{
					numSubs++;
				}
			}
			else
			{
				printf("[ERROR] Unrecognized item to add: %s\n", var);
				res = 1;
			}
			PROG_STAT = res;
		}
		else if(strcmp(command, "delta") == 0)
		{
			float d = 0;
			sscanf(line, "delta %f", &d);
			if(d > 0)
			{
				printf("Delta value set to: %f\n", d);
				DELTA = d;
			}
			else
			{
				printf("[ERROR] Invalid delta value\n");
			}
		}
		else if(strcmp(command, "start") == 0)
		{
			/* Init Cleanup Thread and Alarm */
			pthread_t cleanupThread;
			pthread_create(&cleanupThread, &attr, cleanup, NULL);
			signal(SIGALRM, alarmHandler);
			alarm(2);
			usleep(200);

			/* Display Menu Before Sending Broadcast Condition to Threads */
			menuRun();

			/* pthread cond boadcast to start all threads */
			pthread_mutex_lock(&meowtx);
			pthread_cond_broadcast(&cond_meowtx);
			pthread_mutex_unlock(&meowtx);
			free(line);
			break;
		}
		else
		{
			/* Print Error */
			char command[32];
			sscanf(line, "%s", command);
			printf("[ERROR] Unrecognized Command: %s\n", command);
			PROG_STAT = 1;
		}
		free(line);
	}
	fclose(fileptr);

	if(PROG_STAT == 0)
	{
		joinThreads(numPubs, numSubs, pubs, subs);
		globalFlag = 0;
	}

	for(int i = 0; i < NUMTOPICS; i++)
	{
		free(registry[i].buffer);
	}

	return PROG_STAT;
}
