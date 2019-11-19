/*
* Description: Lab 8
*
* Author: Samuel Lundquist
*
* Notes:
*/

#define MAXQUEUES = 1;
#define MAXNAME = 256;

typedef struct{
	int ticketNum;
	char *dish;
}mealTicket;

typedef struct{
	char name[MAXNAME]
	mealTicket *const buffer;
	int head;
	int tail;
	const int length;
}MTQ;

MTQ registry[MAXQUEUES];

int enqueue(char *MTQ_ID, mealTicket *MT)
{
	return 1;
}

int dequeue(char *MTQ_ID, int ticketNum, malTicket *MT)
{
	return 1;
}

int main()
{

}
