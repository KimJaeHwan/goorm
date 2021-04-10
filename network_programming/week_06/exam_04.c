#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

struct channel {
	int sendFrameID;
	int ackFrameID;
	int sendFrameError;
	int ackFrameError;
};
const int const NULL_FRAME_ID = -1;
const int const INITIAL_FRAME_ID = 0;
const int const TIME_STEP = 2;
const int const ITER_MAX = 5;

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

void *sender_routine(void *arg){
	struct channel * chan = (struct channel *)arg;
	int frameToSend = INITIAL_FRAME_ID;
	int i;
	for (i = 0;i< ITER_MAX;i++){
		printf("(S) sending frame #%d\n",frameToSend);	// 프레임 송신 출력문
		chan->sendFrameID = frameToSend;		// 프레임 송신 작업
		sleep((int)(TIME_STEP));
		if(rand_per(40)||chan->ackFrameID != frameToSend)		// 40퍼의 확률로 에러 발생 또는 ack값의 변조
		{
			printf("(S) receving ack ERROR\n");	// 에러인 경우
			continue;				// 재반복
		}else
		{
			printf("(S) receiving ack SUCCESS #%d\n",chan->ackFrameID); //정상적인 경우
		}
		
		frameToSend++;
	}
	pthread_exit(0);
}

void *receiver_routine(void * arg){
	srand(time(NULL));
	struct channel *chan = (struct channel *) arg;
	int frameToReceive = INITIAL_FRAME_ID;
	int i;
	sleep((int)(TIME_STEP / 2));		// 동기를 맟추기위한 sleep
	for(i = 0; i < ITER_MAX; i++){
		frameToReceive = chan->sendFrameID;		// 받은 프레임 저장
		if(rand_per(40))				// 40퍼센트 확률로 에러 발생
		{
			printf("(R) receiving frame ERROR\n");
						// 에러 발생으로 ackFrameID에 frameToReceive를 저장하지않음
						// 이경우 이전에 받았던것을 다시 보낸다.
			printf("(R) sending ack #%d\n",chan->ackFrameID);	// 제대로 받지 못했음을 알림
			sleep((int)(TIME_STEP));	// 동기를 맟추기 위함
			continue;			// 재반복
		}else
		{
			printf("(R) receiving frame SUCCESS #%d\n",frameToReceive);	// 정상적으로 받는경우 출력문
			chan->ackFrameID = frameToReceive;			// 정상적으로 받고 ack를 보낸다.
			printf("(R) sending ack #%d\n",chan->ackFrameID);	// 정상적인send ack
		}
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
