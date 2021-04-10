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

int rand_per(int per)
{
	int rand_num = 0;
	rand_num = rand()%100;
	if(rand_num <per)
		return 1;	// 확률 통과
	else
		return 0;	// 확률 불통과
	return -1;		// 있을수 없는 일
}

void *sender_routine(void *arg){
	struct channel * chan = (struct channel *)arg;
	int frameToSend = INITIAL_FRAME_ID;
	int i;
	for (i = 0;i< ITER_MAX;i++){
		printf("(S) sending frame #%d\n",frameToSend);	// 보낸 프레임 출력
		chan->sendFrameID = frameToSend;		// 프레임 송신 과정
		sleep((int)(TIME_STEP));			// 순서를 맟추기 위한 sleep
		if(chan->ackFrameID < -1){			// recevier 가 프레임 수신을 제대로 못한경우
			i--;
			frameToSend = chan->sendFrameID;	// 에러 발생시 에러 발생 지점부터
			continue;				// 에러 발생시 재전송
		}
		if(rand_per(40)){				// 수신시 일정 확률로 수신 데이터 변조
			chan->ackFrameID += -99;
		}
		if(chan->ackFrameID != frameToSend){		// 수신 데이터 비교
			printf("(S) receiving ack ERROR\n");
			i--;					// 에러 발생시 0부터
			frameToSend = chan->sendFrameID;		// 에러 발생시 처음 부터
			continue;				// 에러 발생시 재전송
		}	// 보낸 프레임과 ack프레임이 다른경우
		else	
			printf("(S) receiving ack #%d\n",chan->ackFrameID);	// 받은 ack출력
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
		if(rand_per(40)){
			frameToReceive = chan->sendFrameID - 99;	// 프레임 수신과정
		}else
			frameToReceive = chan->sendFrameID;

		if(frameToReceive < 0)			// 수신된 데이터에 에러가 있다면
		{
			printf("(R) receving frame ERROR\n");
			i--;					// 에러 발생시 처음부터 다시
			chan->ackFrameID = frameToReceive;	// 변조된 프레임 전송 
			frameToReceive = INITIAL_FRAME_ID;
			sleep((int)(TIME_STEP));		// sender가 다시 보내지도않았는데 먼저 출력을 방지
			continue;
		}
		printf("(R) receiving frame #%d\n",frameToReceive);	// 받은 프레임 출력
		chan->ackFrameID = frameToReceive;			// ack 프레임 송신과정
		printf("(R) sending ack #%d\n",chan->ackFrameID);	// 송신 프레임 출력
		sleep((int)(TIME_STEP));				// 순서를 맟추기위한 sleep
	}
	pthread_exit(0);
}

int main(void)
{
	srand(time(NULL));
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
