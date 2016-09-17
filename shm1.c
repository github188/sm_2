#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include "mem.h"

int main()
{
  int running = 1;
  void *shared_memory_start = (void *)0;
  void *shared_memory_end = (void *)0;
  struct MEM_HEAD *head = NULL;
  struct FRAME_PACKET *packet = NULL;
  int shmid;
  unsigned int cnt = 0;
  struct INDEX_INFO index;

  unsigned int shm = sizeof(struct MEM_HEAD)+sizeof(struct FRAME_PACKET)*FRAME_NUM;
  shmid = shmget((key_t)1234, shm, 0666 | IPC_CREAT);
  if (shmid == -1) 
  {
    fprintf(stderr, "shmget failed\n");
    exit(EXIT_FAILURE);
  }
  shared_memory_start = shmat(shmid, (void *)0, 0);
  if (shared_memory_start == (void *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
  shared_memory_end = shared_memory_start + shm;
  printf("Memory attached at %X\n", (int)shared_memory_start);
  printf("Memory end at %X\n", (int)shared_memory_end);
  
  head = (struct MEM_HEAD *)shared_memory_start;
  packet = (struct FRAME_PACKET *)(shared_memory_start + sizeof(head));
  FILE *fp = fopen("test.264", "w");
  FILE *fp2 = fopen("test.index", "w");
  struct FRAME_PACKET rev_pac;
  unsigned int frolen = 0;
  unsigned int curlen = 0;
  while(running) 
  {
	if (head->front == head->rear) /*共享内存空*/
	{
		usleep(10000);
	}
    else
	{	
		memset(&rev_pac, 0, sizeof(struct FRAME_PACKET)); 
		memset(&index, 0, sizeof(struct INDEX_INFO)); 

 		rev_pac.head.IFrameType = packet->head.IFrameType;
  		rev_pac.head.IFrameLen = packet->head.IFrameLen;
  		rev_pac.head.ISreamExam = packet->head.ISreamExam;
  		rev_pac.head.IExtendLen = packet->head.IExtendLen;
  		rev_pac.head.IExtendCount = packet->head.IExtendCount;
  		rev_pac.rtc.stuInfoTYpe.LInfoType = packet->rtc.stuInfoTYpe.LInfoType;
  		rev_pac.rtc.stuInfoTYpe.LInfoLength = packet->rtc.stuInfoTYpe.LInfoLength ;
  		rev_pac.rtc.stuRtcTime.cYear = packet->rtc.stuRtcTime.cYear;
  		rev_pac.rtc.stuRtcTime.cMonth = packet->rtc.stuRtcTime.cMonth;
  		rev_pac.rtc.stuRtcTime.cDay = packet->rtc.stuRtcTime.cDay;
  		rev_pac.rtc.stuRtcTime.cHour = packet->rtc.stuRtcTime.cHour;
  		rev_pac.rtc.stuRtcTime.cMinute = packet->rtc.stuRtcTime.cMinute;
  		rev_pac.rtc.stuRtcTime.cSecond = packet->rtc.stuRtcTime.cSecond;
  		rev_pac.rtc.stuRtcTime.usMilliSecond = packet->rtc.stuRtcTime.usMilliSecond;
  		rev_pac.rtc.stuRtcTime.usWeek = packet->rtc.stuRtcTime.usWeek;
  		rev_pac.rtc.stuRtcTime.usReserved = packet->rtc.stuRtcTime.usReserved;
  		rev_pac.rtc.stuRtcTime.usMilliValidate = packet->rtc.stuRtcTime.usMilliValidate;

  		memcpy(rev_pac.frame, packet->frame, packet->head.IFrameLen);
  		frolen+=curlen; /*偏移量*/
  		curlen = 12+rev_pac.head.IExtendLen+rev_pac.head.IFrameLen;
		fwrite(&rev_pac, curlen ,1,fp);

		index.cHour = packet->rtc.stuRtcTime.cHour;
		index.cMinute = packet->rtc.stuRtcTime.cMinute;
		index.cSecond = packet->rtc.stuRtcTime.cSecond;
		index.offset = frolen;
		fwrite(&index, sizeof(index) ,1,fp2);

		head->front = (head->front+1)%FRAME_NUM;
		head->nread ++;		
		printf("have frame num :%d ~ already read num:%d\n", head->nread , ++cnt);
		if (head->nread == (FRAME_NUM))	/*回环*/
		{
			head->nread = 0;
			packet = (struct FRAME_PACKET *)(shared_memory_start + sizeof(head));
		}
		else
		{
			packet = (struct FRAME_PACKET *)((char *)packet + sizeof(struct FRAME_PACKET));
		}	
	}
  }
  
  if (shmdt(shared_memory_start) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid, IPC_RMID, 0) == -1) 
  {
    fprintf(stderr, "shmctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

