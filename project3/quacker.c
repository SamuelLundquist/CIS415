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
	}
	return ret;
}

int dequeue(char *TQ_ID, int entryNum)
{
	TQ *queue;
	int ret = 0;

	if((queue = findQueue(TQ_ID)) != NULL)
	{
		int h = queue->head;
		int t = queue->tail;
		int entry = queue->buffer[h].entryNum;
		if(entry >= 0 && entry == entryNum)
		{
			queue->buffer[t].entryNum = 0;
			queue->buffer[h].entryNum = -1;
			h++;
			queue->head = h % queue->length;
			ret = 1;
		}
	}
	return ret;
}

/******************************************************************************/

int main()
{
	TQ_DEF(topicQueue1, "Nature");
	//TQ_DEF(topicQueue2, "Cityscapes");
	//TQ_DEF(topicQueue3, "Gaming");

	registry[0] = &topicQueue1;
	TE_DEF(tree, 1, "Tree", "Look at this tree");
	printf("%d:%ld %s - %s\n", tree.pubID, tree.timeStamp.tv_sec,
		tree.photoURL, tree.photoCaption);

	enqueue("Nature", &tree);
	enqueue("Nature", &tree);
	enqueue("Nature", &tree);
	enqueue("Nature", &tree);
	enqueue("Nature", &tree);
	enqueue("Nature", &tree);
	for(int i = 0; i < 5; i++)
	{
		printf("%s:%d - %s\n", registry[0]->topic, registry[0]->entries,
			registry[0]->buffer[i].photoURL);
	}

	atexit(exitStat);
	return EXIT_SUCCESS;
}
