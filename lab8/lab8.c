/*
* Description: Lab 8
*
* Author: Samuel Lundquist
*
* Notes:
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXQUEUES 4
#define MAXNAME 256

typedef struct{
	int ticketNum;
	char *dish;
}mealTicket;

typedef struct{
	char name[MAXNAME];
	int head;
	int tail;
	const int length;
	mealTicket * const buffer;
}MTQ;

MTQ *registry[MAXQUEUES];

MTQ initQueue(char* name, int length)
{
	mealTicket buf[length];
	for(int i = 0; i < length; i++)
	{
		mealTicket nul = {.ticketNum = 0, .dish = NULL};
		buf[i] = nul;
	}
	buf[length-1].ticketNum = -1;

	MTQ queue = { .head = 0, .tail = 0, .length = length, .buffer = buf };
	strcpy(queue.name, name);
	//printf("%d %d %d %d\n", queue.buffer[0].ticketNum, queue.buffer[1].ticketNum, queue.buffer[2].ticketNum, queue.buffer[3].ticketNum);
	return queue;
}

int enqueue(char *MTQ_ID, mealTicket *MT)
{
	printf("WENT TO ENQUEUE\n");
	MTQ *queue;
	int last = MAXQUEUES-1;
	int i;
	for(i = 0; i < MAXQUEUES; i++)
	{
		queue = registry[i];
		if(strcmp(queue->name, MTQ_ID) == 0)
		{
			break;
		}
		if(i == last)
		{
			printf("Could not find queue with name: %s\n", MTQ_ID);
			return 0;
		}
	}
	int t = queue->tail;
	printf("Tail: %d | ticketNum: %d\n", t, queue->buffer[t].ticketNum);
	if(queue->buffer[t].ticketNum == -1)
	{
		return 0;
	}
	else
	{
		MT->ticketNum = t;
		queue->buffer[t].ticketNum = t;
		queue->buffer[t].dish = MT->dish;
		t++;
		if(t == queue->length)
		{
			queue->tail = 0;
		}
		else
		{
			queue->tail = t;
		}
	}
	return 1;
}
/*
int dequeue(char *MTQ_ID, int ticketNum, mealTicket *MT)
{
	printf("WENT TO DEQUEUE\n");
	MTQ *queue;
	int last = MAXQUEUES-1;
	int i;
	for(i = 0; i < MAXQUEUES; i++)
	{
		queue = registry[i];
		if(strcmp(queue->name, MTQ_ID) == 0)
		{
			break;
		}
		if(i == last)
		{
			printf("Could not find queue with name: %s\n", MTQ_ID);
			return 0;
		}
	}
	int h = queue->head;
	int t = queue->tail;
	if(queue->buffer[h].ticketNum <= 0)
	{
		return 0;
	}
	else
	{
		*MT = queue->buffer[h];
		mealTicket nul = { .ticketNum = 0, .dish = NULL };
		queue->buffer[t] = nul;
		nul.ticketNum = -1;
		queue->buffer[h] = nul;
		if(h == queue->length-1)
		{
			queue->head = 0;
		}
		else
		{
			queue->head = h+1;
		}
	}
	return 1;
}
*/
int main()
{
	MTQ bkfst = initQueue("Breakfast", 4);
	registry[0] = &bkfst;
	printf("%d %d %d %d\n", registry[0]->buffer[0].ticketNum, registry[0]->buffer[1].ticketNum, registry[0]->buffer[2].ticketNum, registry[0]->buffer[3].ticketNum);

	MTQ lnch = initQueue("Lunch", 4);
	registry[1] = &lnch;
	printf("%d %d %d %d\n", registry[1]->buffer[0].ticketNum, registry[1]->buffer[1].ticketNum, registry[1]->buffer[2].ticketNum, registry[1]->buffer[3].ticketNum);
	MTQ dinner = initQueue("Dinner", 4);
	registry[2] = &dinner;
	printf("%d %d %d %d\n", registry[2]->buffer[0].ticketNum, registry[2]->buffer[1].ticketNum, registry[2]->buffer[2].ticketNum, registry[2]->buffer[3].ticketNum);
	MTQ bar = initQueue("Bar", 4);
	registry[3] = &bar;

	mealTicket eggs = {.dish = "Eggs Benedict"};
	mealTicket bacon = {.dish = "Bacon"};
	mealTicket pancakes = {.dish = "Pancakes"};

	printf("%d %d %d %d\n", registry[0]->buffer[0].ticketNum, registry[0]->buffer[1].ticketNum, registry[0]->buffer[2].ticketNum, registry[0]->buffer[3].ticketNum);
	printf("%d %d %d %d\n", registry[1]->buffer[0].ticketNum, registry[1]->buffer[1].ticketNum, registry[1]->buffer[2].ticketNum, registry[1]->buffer[3].ticketNum);
	printf("%d %d %d %d\n", registry[2]->buffer[0].ticketNum, registry[2]->buffer[1].ticketNum, registry[2]->buffer[2].ticketNum, registry[2]->buffer[3].ticketNum);

	printf("Enqueue: %d\n", enqueue("Breakfast", &eggs));
	printf("Enqueue: %d\n", enqueue("Breakfast", &bacon));
	printf("Enqueue: %d\n", enqueue("Breakfast", &pancakes));
	printf("Enqueue: %d\n", enqueue("Breakfast", &eggs));
	printf("Enqueue: %d\n", enqueue("Breakfast", &bacon));

	printf("%s: %s,\n", bkfst.name, bkfst.buffer[0].dish);
	printf("%s: %s,\n", bkfst.name, bkfst.buffer[1].dish);
	printf("%s: %s,\n", bkfst.name, bkfst.buffer[2].dish);
	printf("%s: %s,\n", bkfst.name, bkfst.buffer[3].dish);

}
