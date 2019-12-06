/******************************** main.c ***************************************
* Program Name: Project 3
* Author: Samuel Lundquist
* Date: 11.26.2019
* Description:
* Notes:
*******************************************************************************/

/************************ Preprocessor Directives *****************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "commands.h"
/******************************************************************************/

/******************************* Globals **************************************/
int ENTRYVAL = 1;
static TQ *registry[MAXTOPICS];
/******************************************************************************/

/****************************** Functions *************************************/
void exitStat(void)
{
	printf("Exited successfully\n");
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

int dequeue(char *TQ_ID, int entryNum)
{
	TQ *queue;
	int ret = 0;

	if((queue = findQueue(TQ_ID)) != NULL)
	{
		pthread_mutex_t mtx = queue->mutex;
		pthread_mutex_lock(&mtx);
		int h = queue->head;
		int t = queue->tail;
		int entry = queue->buffer[h].entryNum;
		if(entry > 0 && entry == entryNum)
		{
			queue->buffer[t].entryNum = 0;
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

void *publisher(void *args)
{
	int id;
	id = ((pubArgs *) args)->id;
	printf("Publisher: %d\n", id);
}

void *subscriber(void *args)
{
	int id;
	id = ((subArgs *) args)->id;
	printf("Subscriber: %d\n", id);
}
/******************************************************************************/

int main()
{
	int i;

	/* Init Topic Queues */
	TQ_DEF(topicQueue1, "Nature");
	TQ_DEF(topicQueue2, "Cityscapes");
	TQ_DEF(topicQueue3, "Gaming");
	TQ_DEF(topicQueue4, "Anime");
	TQ_DEF(topicQueue5, "Art");

	/* Place Topic Queues In Registry */
	registry[0] = &topicQueue1;
	registry[1] = &topicQueue2;
	registry[2] = &topicQueue3;
	registry[3] = &topicQueue4;
	registry[4] = &topicQueue5;

	TE_DEF(tree, "url", "This is a tree");
	TE_DEF(bush, "url", "This is a bush");
	TE_DEF(hedge, "url", "This is a hedge");
	topicEntry entries[] = { tree, bush, hedge };
	char **queues;
	queues = malloc(3 * sizeof(char*));
	for(int x = 0; x < 3; x++)
	{
		queues[x] = malloc(MAXNAME * sizeof(char));
	}
	queues[0] = "Nature";
	queues[1] = "Nature";
	queues[2] = "Nature";


	pthread_t pubs[MAXPUBS];
	pubArgs pubargs[MAXPUBS];
	pthread_t subs[MAXSUBS];
	subArgs subargs[MAXSUBS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	topicEntry entry;

	pubArgs pargs;
	pargs.id = 0;
	pargs.TE = entries;
	pargs.topics = queues;
	pubargs[0] = pargs;

	subArgs sargs;
	sargs.id = 0;
	sargs.TE = &entry;
	sargs.topic = "Nature";
	subargs[0] = sargs;

	/* Create Thread Pools */
	for(i = 0; i < MAXPUBS; i++)
	{
		pthread_create(&pubs[i], &attr, publisher, (void *) &pubargs[i]);
	}
	for(i = 0; i < MAXSUBS; i++)
	{
		pthread_create(&subs[i], &attr, subscriber, (void *) &subargs[i]);
	}

	/* Join Thread Pools */
	for(i = 0; i < MAXPUBS; i++)
	{
		pthread_join(pubs[i], NULL);
	}
	for(i = 0; i < MAXSUBS; i++)
	{
		pthread_join(subs[i], NULL);
	}

	atexit(exitStat);
	return EXIT_SUCCESS;
}
