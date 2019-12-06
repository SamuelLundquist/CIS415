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
		//sched_yield(); put this in the dequeue function for cleanup thread
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
	char **TQ_LIST;
	topicEntry *TE_LIST;

	TQ_LIST = ((pubArgs *) args)->topics;
	TE_LIST = ((pubArgs *) args)->TE;
	id = ((pubArgs *) args)->id;

	printf("Publisher: %d\n", id);

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

	sleep(2);
}

void *subscriber(void *args)
{
	int id, lastEntry, res;
	topicEntry *entry;
	char *TQ_ID;

	id = ((subArgs *) args)->id;
	lastEntry = 0;
	entry = ((subArgs *) args)->TE;
	TQ_ID = ((subArgs *) args)->topic;
	printf("Subscriber: %d\n", id);
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
/******************************************************************************/

int main()
{
	//BROADCAST TO THREAD TO GET THOSE DAMN THREADS TO START DOIN SHIT
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

	/* Variables for pub and sub arguments for testing */
	TE_DEF(tree, "url", "This is a tree");
	TE_DEF(bush, "url", "This is a bush");
	TE_DEF(hedge, "url", "This is a hedge");
	topicEntry entries[] = { tree, bush, hedge };
	topicEntry entry;
	char **queues;
	queues = malloc(4 * sizeof(char*));
	for(int x = 0; x < 4; x++)
	{
		queues[x] = malloc(MAXNAME * sizeof(char));
	}
	queues[0] = "Nature";
	queues[1] = "Cityscapes";
	queues[2] = "Gaming";
	queues[3] = NULL;


	pthread_t pubs[MAXPUBS];
	pubArgs pubargs[MAXPUBS];

	pthread_t subs[MAXSUBS];
	subArgs subargs[MAXSUBS];

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	for(i = 0; i < MAXPUBS; i++)
	{
		pubArgs p;
		p.id = i;
		p.TE = entries;
		p.topics = queues;
		pubargs[i] = p;
	}

	for(i = 0; i < MAXPUBS; i++)
	{
		subArgs s;
		s.id = i;
		s.TE = &entry;
		s.topic = "Nature";
		subargs[i] = s;
	}

	/* Create Thread Pools */
	for(i = 0; i < MAXPUBS; i++)
	{
		pthread_create(&pubs[i], &attr, publisher, (void *) &(pubargs[i]));
	}
	for(i = 0; i < MAXSUBS; i++)
	{
		pthread_create(&subs[i], &attr, subscriber, (void *) &(subargs[i]));
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
	free(queues);

	atexit(exitStat);
	return EXIT_SUCCESS;
}
