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

static MTQ *registry[MAXQUEUES];

MTQ initQueue(char* name, int length)
{
	mealTicket *buf = malloc(length * sizeof(mealTicket));
	mealTicket nul = {.ticketNum = 0, .dish = NULL};
	for(int i = 0; i < length; i++)
	{
		buf[i] = nul;
	}
	buf[length-1].ticketNum = -1;

	MTQ queue = { .head = 0, .tail = 0, .length = length, .buffer = buf };
	strcpy(queue.name, name);
	return queue;
}

int enqueue(char *MTQ_ID, mealTicket *MT)
{
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
			printf("Enqueue error: Could not find queue with name: %s\n", MTQ_ID);
			return 0;
		}
	}

	int t = queue->tail;
	if(queue->buffer[t].ticketNum == -1)
	{
		return 0;
	}
	else
	{
		MT->ticketNum = t+1;
		queue->buffer[t] = *MT;
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

int dequeue(char *MTQ_ID, int ticketNum, mealTicket *MT)
{
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
			printf("Dequeue error: Could not find queue with name: %s\n", MTQ_ID);
			return 0;
		}
	}
	int h = queue->head;
	int t = queue->tail;
	int ticket = queue->buffer[h].ticketNum;

	if(ticket <= 0)
	{
		*MT = queue->buffer[h];
		return 0;
	}
	else if (ticket == ticketNum)
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
	else
	{
		printf("%d %d\n", ticketNum, ticket);
		printf("Dequeue error: given ticketNum was not at the head of the queue\n");
		return 0;
	}
	return 1;
}

int main()
{
	MTQ bkfst = initQueue("Breakfast", 4);
	registry[0] = &bkfst;

	MTQ lnch = initQueue("Lunch", 4);
	registry[1] = &lnch;

	MTQ dinner = initQueue("Dinner", 4);
	registry[2] = &dinner;

	MTQ bar = initQueue("Bar", 4);
	registry[3] = &bar;

	mealTicket breakfast_1 = {.ticketNum = -2, .dish = "Eggs Benedict"};
	mealTicket breakfast_2 = {.ticketNum = -2, .dish = "Bacon"};
	mealTicket breakfast_3 = {.ticketNum = -2, .dish = "Pancakes"};
	enqueue("Breakfast", &breakfast_1);
	enqueue("Breakfast", &breakfast_2);
	enqueue("Breakfast", &breakfast_3);

	mealTicket lunch_1 = {.ticketNum = -2, .dish = "Sandwich"};
	mealTicket lunch_2 = {.ticketNum = -2, .dish = "Salad"};
	mealTicket lunch_3 = {.ticketNum = -2, .dish = "Sodapopski"};
	enqueue("Lunch", &lunch_1);
	enqueue("Lunch", &lunch_2);
	enqueue("Lunch", &lunch_3);

	mealTicket meal_1 = {.ticketNum = -2, .dish = "Spaghetti"};
	mealTicket meal_2 = {.ticketNum = -2, .dish = "Calzone"};
	mealTicket meal_3 = {.ticketNum = -2, .dish = "Lasagna"};
	enqueue("Dinner", &meal_1);
	enqueue("Dinner", &meal_1);
	enqueue("Dinner", &meal_1);

	mealTicket drink_1 = {.ticketNum = -2, .dish = "Gin and Tonic"};
	mealTicket drink_2 = {.ticketNum = -2, .dish = "Jack and Coke"};
	mealTicket drink_3 = {.ticketNum = -2, .dish = "Tequila"};
	enqueue("Bar", &drink_1);
	enqueue("Bar", &drink_2);
	enqueue("Bar", &drink_3);

	int i = 1;
	while(i < 5)
	{
		for(int j = 0; j < 4; j++)
		{
			mealTicket test;
			char *nm = registry[j]->name;
			dequeue(nm, i, &test);
			printf("Queue: %s - Ticket Number: %d - Dish: %s\n", nm, test.ticketNum, test.dish);
		}
		i++;
	}
	mealTicket test;
	if(dequeue(registry[0]->name, registry[0]->head+1, &test))
	{
		printf("Test Case: A - Result: Success\n");
	}
	else
	{
		printf("Test Case: A - Result: Fail\n");
	}

	enqueue("Breakfast", &breakfast_1);
	enqueue("Breakfast", &breakfast_2);
	enqueue("Breakfast", &breakfast_3);

	if(dequeue(registry[0]->name, registry[0]->head+1, &test))
	{
		printf("Test Case: B - Result: Success\n");
	}
	else
	{
		printf("Test Case: B - Result: Fail\n");
	}

	enqueue("Breakfast", &breakfast_1);

	if(enqueue("Breakfast", &breakfast_1))
	{
		printf("Test Case: C - Result: Success\n");
	}
	else
	{
		printf("Test Case: C - Result: Fail\n");
	}

	if(enqueue("Lunch", &lunch_1))
	{
		printf("Test Case: D - Result: Success\n");
	}
	else
	{
		printf("Test Case: D - Result: Fail\n");
	}
	free(registry[0]->buffer);
	free(registry[1]->buffer);
	free(registry[2]->buffer);
	free(registry[3]->buffer);

}
