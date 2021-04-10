#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

struct channel {
	int sendFrameID;
	int ackFrameID;
};
const int const NULL_FRAME_ID = -1;
const int const INITIAL_FRAME_ID = 0;
const int const TIME_STEP = 2;
const int const ITER_MAX = 5;

void *sender_routine(void *arg){
	struct channel * chan = (struct channel *)arg;
	int frameToSend = INITIAL_FRAME_ID;
	int i;
	for (i = 0;i< ITER_MAX;i++){
		printf("(S) sending frame #%d\n",frameToSend);
		chan->sendFrameID = frameToSend;
		sleep((int)(TIME_STEP));
		printf("(S) receiving ack #%d\n",chan->ackFrameID);
		if(chan->ackFrameID != frameToSend)	// 보낸 프레임과 ack프레임이 다른경우
			printf("nop!!\n");
		
		frameToSend++;
	}
	pthread_exit(0);
}

void *receiver_routine(void * arg){
	struct channel *chan = (struct channel *) arg;
	int frameToReceive = INITIAL_FRAME_ID;
	int i;
	sleep((int)(TIME_STEP / 2));
	for(i = 0; i < ITER_MAX; i++){
		frameToReceive = chan->sendFrameID;
		printf("(R) receiving frame #%d\n",frameToReceive);
		chan->ackFrameID = frameToReceive;
		printf("(R) sending ack #%d\n",chan->ackFrameID);
		sleep((int)(TIME_STEP));
	}
	pthread_exit(0);
}

int main(void)
{
	pthread_t sender;
	pthread_t receiver;

	struct channel chan;
	chan.sendFrameID = NULL_FRAME_ID;
	chan.ackFrameID = NULL_FRAME_ID;

	pthread_create(&sender, NULL,sender_routine,(void*)&chan);
	pthread_create(&receiver, NULL,receiver_routine,(void*)&chan);

	pthread_join(sender,NULL);
	pthread_join(receiver,NULL);

	return 0;
}
