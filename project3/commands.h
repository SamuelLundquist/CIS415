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
	int head, tail, entries;
	const int length;
	topicEntry * const buffer;
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
#define TQ_DEF(var, id)                                                  \
	topicEntry var##_buf[MAXENTRIES + 1] = {[MAXENTRIES].entryNum = -1}; \
	TQ var = {                                                           \
		.topic = id,                                                     \
		.head = 0,                                                       \
		.tail = 0,                                                       \
		.entries = 0,                                                    \
		.length = MAXENTRIES,                                            \
		.buffer = var##_buf,                                             \
		.mutex = PTHREAD_MUTEX_INITIALIZER                               \
	}

#define TE_DEF(var, photo_url, photo_caption)                            \
	topicEntry var = {                                                   \
		.entryNum = 0,                                                   \
		.pubID = 0,                                                      \
		.photoURL = photo_url,                                           \
		.photoCaption = photo_caption                                    \
	}

#define ARGS_DEF(var, id, filename)                                      \
	threadargs arg = {                                                   \
		.id = id,                                                        \
		.filename = filename                                             \
	}
/******************************************************************************/

/************************** Function Definitions ******************************/

void exitStat(void);

TQ *findQueue(char *TQ_ID);

int enqueue(char *TQ_ID, topicEntry *TE);

int dequeue(char *TQ_ID);

int getEntry(char *TQ_ID, int lastEntry, topicEntry *TE);

void *publisher(void *args);

void *subscriber(void *args);

/******************************************************************************/
#endif
