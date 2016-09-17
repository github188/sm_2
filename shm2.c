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
  head->front = 0;
  head->rear = 0;
  head->nwrite = 0;
  head->nread = 0;

  packet = (struct FRAME_PACKET *)(shared_memory_start + sizeof(head));

  while(running) 
  {
  	if ((head->front == (head->rear+1)%FRAME_NUM)) /*共享内存满*/
	{
		usleep(10000);
    }
	else
	{
		printf("%x ", (int)packet);
		packet->head.IFrameType = 0x63643200;
  		packet->head.IFrameLen = 0x00000A;
  		packet->head.ISreamExam = 0xD6;
  		packet->head.IExtendLen = 0x00000C;
  		packet->head.IExtendCount = 0x01;
  		packet->rtc.stuInfoTYpe.LInfoType = 0x02;
  		packet->rtc.stuInfoTYpe.LInfoLength = 0x00000C;
  		//packet->rtc.stuRtcTime = 0x0800072808100810;
  		packet->rtc.stuRtcTime.cYear = 0x10;
  		packet->rtc.stuRtcTime.cMonth = 0x08;
  		packet->rtc.stuRtcTime.cDay = 0x10;
  		packet->rtc.stuRtcTime.cHour = 0x08;
  		packet->rtc.stuRtcTime.cMinute = 0x28;
  		packet->rtc.stuRtcTime.cSecond = 0x07;
  		packet->rtc.stuRtcTime.usMilliSecond = 0;
  		packet->rtc.stuRtcTime.usWeek = 2;
  		packet->rtc.stuRtcTime.usReserved = 0;
  		packet->rtc.stuRtcTime.usMilliValidate = 0;
  		strncpy(packet->frame, "abcdefghij", 10);
    
		head->nwrite ++;
		printf("have frame num :%d ~ already write num:%d\n", head->nwrite , ++cnt);
		if (head->nwrite == (FRAME_NUM)) /*回环*/
		{
			head->nwrite = 0;
			packet = (struct FRAME_PACKET *)(shared_memory_start + sizeof(head));
		}
		else
		{
			 packet = (struct FRAME_PACKET *)((char *)packet + sizeof(struct FRAME_PACKET));
		}
		head->rear = (head->rear+1)%FRAME_NUM;
	}
  }

  if (shmdt(shared_memory_start) == -1) {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
