#include <stdio.h>
#include "mem.h"
#include <stdlib.h>


int main(void)
{
	printf("%d\n", sizeof(struct MEM_HEAD));
}

/*int main1(void)
{
	struct MEM_HEAD* fp = malloc(100);
	fp->wrptr = 0x2364787;
    fp->rdptr = 0x2364787;
    fp->front = 2;
    fp->rear = 8;
    fp->nframe = 8;
    fp->num = 9;
    int *p = (char *)fp+12;
    *p = 59;
    printf("%#x\n", fp);
    printf("%#x\n", fp+12); //²½³¤
	printf("%#x\n", p);
	printf("%d\n", *p);
}*/

