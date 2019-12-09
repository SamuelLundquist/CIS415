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
#define MAXTOPICS 10
#define MAXNAME 255
#define MAXENTRIES 15
#define MAXPUBS 5
#define MAXSUBS 5
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
#define TQ_DEF(var, id, len)                                             \
	TQ var = {                                                           \
		.id = id,                                                        \
		.head = 0,                                                       \
		.tail = 0,                                                       \
		.entries = 0,                                                    \
		.length = len,                                                   \
		.mutex = PTHREAD_MUTEX_INITIALIZER                               \
	}

#define TE_DEF(var, num, id)                                             \
	topicEntry var = {                                                   \
		.entryNum = num,                                                 \
		.pubID = id,                                                     \
	}

/******************************************************************************/

/************************** Function Definitions ******************************/

void exitStat(void);

void menuStart(char *filename);

void menuRun();

int checkArgv(char *argv[]);

TQ *findQueueFromID(int id);

int enqueue(int id, topicEntry *TE);

int dequeue(int id);

int getEntry(int id, int lastEntry, topicEntry *TE);

int makeThread(int type, int index, char *filename);

void joinThreads(int numPubs, int numSubs, pthread_t p[], pthread_t s[]);

void *publisher(void *args);

void *subscriber(void *args);

void *cleanup();

void alarmHandler(int signal);

/******************************************************************************/
#endif
