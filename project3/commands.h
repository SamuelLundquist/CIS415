/****************************** commands.h *************************************
* Author: Samuel Lundquist
* Date: 11.26.2019
* Description: Header file for project 3
* Notes:
*******************************************************************************/

#ifndef COMMANDS
#define COMMANDS

/****************************** Constants *************************************/
#define URLSIZE 255
#define CAPSIZE 255
#define MAXTOPICS 5
#define MAXNAME 255
#define MAXENTRIES 5
#define MAXPUBS 3
#define MAXSUBS 3
/******************************************************************************/

/******************************* Structs **************************************/
typedef struct{
	int entryNum;
	clock_t t;
	int pubID;
	char photoURL[URLSIZE];
	char photoCaption[CAPSIZE];
}
topicEntry;

typedef struct{
	char topic[MAXNAME];
	int id, head, tail, entries;
	int length;
	topicEntry * buffer;
	pthread_mutex_t mutex;
}
TQ;

typedef struct{
	int id;
	char filename[MAXNAME];
}
threadargs;

/******************************************************************************/

/******************************* Macros ***************************************/
#define TQ_BLANK(var)                                                    \
	TQ var = {                                                           \
		.id = 0,                                                         \
		.head = 0,                                                       \
		.tail = 0,                                                       \
		.entries = 0,                                                    \
		.length = 0,                                                     \
		.mutex = PTHREAD_MUTEX_INITIALIZER                               \
	}

#define TQ_DEF(var, id, len)                                             \
	TQ var = {                                                           \
		.id = id,                                                        \
		.head = 0,                                                       \
		.tail = 0,                                                       \
		.entries = 0,                                                    \
		.length = len,                                                   \
		.mutex = PTHREAD_MUTEX_INITIALIZER                               \
	}

#define TE_DEF(var, ENTRYVAL, id)                                        \
	topicEntry var = {                                                   \
		.entryNum = ENTRYVAL,                                            \
		.pubID = id,                                                     \
	}

/******************************************************************************/

/************************** Function Definitions ******************************/

void exitStat(void);

TQ *findQueue(char *TQ_ID);

int enqueue(int id, topicEntry *TE);

int dequeue(int id);

int getEntry(int id, int lastEntry, topicEntry *TE);

void *publisher(void *args);

void *subscriber(void *args);

/******************************************************************************/
#endif
