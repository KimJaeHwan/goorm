#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main()
{
	srand(time(NULL));
	int random = 0,i;
	for (i = 0;i<10;i++)
	{
		random = rand()%100;
		printf("%d\n",random);
		if(random < 30)
			printf("30퍼센트의 확률을 뚫음 !!!\n");
		else
			printf("어림도 없다 !!!!!\n");
	}
}
