#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define	MODEL_STATUS_UNKNOWN	0
#define	MODEL_STATUS_EST	1
#define	MODEL_STATUS_ESTC	2
#define	MODEL_STATUS_INF	3
#define MODEL_STATUS_PREPROCESS 4
#define MODEL_STATUS_RANKING 5
#define MODEL_STATUS_CLASSIFIER 6
#define MODEL_STATUS_SERVER 7
#define N_RANKING 300
#define N_DISP 12

#define PORT 6000
#define LISTENQ 20
#define BUFFSIZE 4096
#define FILE_NAME_MAX_SIZE 512
#define WORD_MAX_SIZE 256

struct MovieData {
	char name[100];
	char year[10];
	char length[10];
	char director[30];
	char cast[500];
	char pic[200];
	char number[20];
	char content[10000];
	char wiki[10000];
};

#endif

