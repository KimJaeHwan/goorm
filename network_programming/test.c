#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int rand_per(int per)
{
	int rand_num = 0;
	rand_num = rand()%100;
	if(rand_num < per)
		return 1;
	else
		return 0;

	return -1;
}

int main()
{

	int i;
	srand(time(NULL));
	for(i = 0;i<10;i++){
		if(rand_per(30))
			printf("[%d] 30퍼센트의 확률을 뚫음 !!\n",i);
		else
			printf("[%d] 어림도 없다 !!!!!\n",i);
	}
}

