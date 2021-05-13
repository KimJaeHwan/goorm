#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void handler(int sig);

int main(int argc, char* argv[])
{
	signal(SIGINT,handler);
	printf("SLeep begins!!\n");
	sleep(1000);
	printf("Wake up!");

	return 0;
}

void handler(int sig)
{
	if(sig == SIGINT)
		printf("Handler is called.\n");

	exit(1);
}
