#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>		// timeout안쓰면 삭제해도 됨
#include <sys/select.h>
#include <sys/shm.h>
#include <pthread.h>
#include <signal.h>

#define MAX_ROOM 4	// 대기실 1개 채팅룸 3개
#define MAX_USER 10
#define MAX_CAPA 5
#define KEY_NUM 1234

struct chatting_room{
	pthread_t thread_id;
	int room_num;		// 자신의 방번호
	int users[MAX_CAPA];	// 자신의 채팅방에 있는 사용자들
	int user_cnt;		// 자신의 채팅방에 있는 사용자들 수
};

void null(int sig);
void pntArr(int *arr, int size);
void delInd(int* arr, int* size, int ind);
void * handle_clnt(void * arg);
void send_msg(char * msg, struct chatting_room *chat,int users_num);
void error_handling(char * message);
int maxArr(int * arr, int size);

/* server room  */
int return_sock[MAX_USER];
int return_cnt = 0;
pthread_mutex_t mutx;

int main(int argc, char * argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	socklen_t clnt_adr_sz;
	pthread_t t_id;
	/* select value */
	int fd_max, str_len, fd_num, i,j;
	char message[BUFSIZ];
	char menu_list[200] = "<MENU>\n1.채팅방 목록보기\n2. 채팅방 참여하기(사용법 : 2 <채팅방 번호>)\n3. 프로그램종료\n(0을 입력하면 메뉴가 다시 표시됩니다)\n";

	struct chatting_room *chatp;
	int chat_room_num;

	struct sigaction act;			// return 되는 사용자들을 받기위한 시그널 변수

	if(argc!=2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	
	
	pthread_mutex_init(&mutx, NULL);

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0 , sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	/* 기본 chatting room 3개 생성 + 대기실 */
	chatp = (struct chatting_room *)calloc(sizeof(struct chatting_room),MAX_ROOM);

	for(i = 0;i<MAX_ROOM;i++){
		chatp[i].room_num = i;
		pthread_create(&t_id, NULL, handle_clnt,(void*)&chatp[i]);
		(chatp + i)->thread_id = t_id;
	}
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;
	while(1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 1;
		timeout.tv_usec = 1000;
		
		/* 채팅방에서 나온 사용자들 FD_SET해주는 과정 */
		pthread_mutex_lock(&mutx);
		for(i = 0; i < return_cnt; i++)
		{
			printf("return sock[%d] : %d\n",i,return_sock[i]); 
			FD_SET(return_sock[i],&reads);
			if(fd_max < return_sock[i])
				fd_max = return_sock[i];
		}
		return_cnt = 0;
		pthread_mutex_unlock(&mutx);

		if((fd_num = select(fd_max + 1, &cpy_reads, 0,0, &timeout)) == -1)	// error
			break;
		if(fd_num == 0)		// timeout
			continue;
		
		for(i = 0; i< fd_max + 1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				if(i == serv_sock)
				{
					clnt_adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
				//	chatp[0].users[chatp[0].user_cnt] = clnt_sock;
				//	chatp[0].user_cnt++;
					FD_SET(clnt_sock, &reads);
					if(fd_max < clnt_sock)
						fd_max = clnt_sock;

					printf("connected client: %d \n",clnt_sock);

					send(clnt_sock,menu_list,strlen(menu_list),0);
					printf("send menu\n");
				}
				else	// read message!
				{
					str_len = recv(i, message, BUFSIZ, 0);
					if(str_len == 0)	// close message
					{
						FD_CLR(i,&reads);
						close(i);
						printf("closed client: %d \n", i);
					}
					else
					{
						message[str_len] = 0;
						printf("MSG from client %d : %s\n",i,message);
						//send(i,message,str_len,0);
						switch(message[0]){
							case '0':
								send(i,menu_list,strlen(menu_list),0);
								printf("send menu\n");
								break;
							case '1':
								strcpy(message,"<ChatRoom info>\n");
								for(j = 0;j < MAX_ROOM;j++)
								{
									sprintf(message,"%s[ID: %d] Chatroom-%d (%d/%d)\n",message,j,j,chatp[j].user_cnt,MAX_CAPA);
								}
								send(i, message,strlen(message), 0);
								break;
							case '2':
								if(fd_max == i)		// 현재 fd_max값이 채팅방에 참여하고자 하는 clnt이면 값을 하나 내린다.
									fd_max--;

								chat_room_num = atoi(message + 2);
								sprintf(message,"<Ch. %d> Chatting room!!!",chat_room_num);
								send(i,message,strlen(message),0);

								FD_CLR(i,&reads);	// 채팅방에 참여하고자하는 clnt를 select 검사에서 삭제
								/* n번 채팅방 참여 시도 n번이 현재 존재하는 채팅방인지 체크하는 부분이 필요할듯*/						
								pthread_mutex_lock(&mutx);
								chatp[chat_room_num].users[chatp[chat_room_num].user_cnt++] = i;
								pthread_mutex_unlock(&mutx);
								pthread_kill(chatp[chat_room_num].thread_id,SIGUSR1);
								printf("chat_room[%d] thread_id : %d client_num : %d\n",chat_room_num,chatp[chat_room_num].thread_id, i);
									
								//pthread_create(&t_id, NULL, handle_clnt,(void*)&chatp[chat_room_num]);						
								
								break;
							case '3':
								printf("user[%d] exit\n",i);
								FD_CLR(i,&reads);
								close(i);
								break;
						}

					}
				}
			}
		}
	}
//	}
	close(serv_sock);
	return 0;
}
int maxArr(int * arr, int size)
{
	int i, max = -1;
	for( i = 0; i < size; i++) { if(max < arr[i]) max = arr[i]; }
	return max;
}
void * handle_clnt(void *arg)
{
	int str_len = 0;
	int chat_room_num;
	int i, fd_max = -1, fd_num;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	
	char message[BUFSIZ];
	struct chatting_room* chat;

	struct sigaction act;		// 채팅방에 참여하고자하는 사용자를 받기위한 시그널

	/* 시그널 등록 */
	act.sa_handler=null;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGUSR1,&act,0);	
	
	chat = (struct chatting_room *)arg;

	printf("Thread chatting room open [%d] thread_id : %d\n",chat->room_num,chat->thread_id);

	FD_ZERO(&reads);
	while(1){

	pthread_mutex_lock(&mutx);
	/* 주기 적으로 해당 채팅방의 users를 확인하여 FD_SET을 해준다. 시그널 등록을 해두었기에 시그널이 발생하면 바로 FD_SET을 한다*/
	for(i = 0;i < chat->user_cnt; i++)
	{
		FD_SET(chat->users[i],&reads);
		if( fd_max < chat->users[i]) 
			fd_max = chat->users[i];
		printf("[Ch. %d user : %d\n",chat->room_num,chat->users[i]);
	}
	pthread_mutex_unlock(&mutx);
	
	/* 타임 아웃을 설정하여 변경된 select클라이언트들을 갱신한다 */
	cpy_reads = reads;
	timeout.tv_sec = 10;
	timeout.tv_usec = 10000;
	if((fd_num = select(fd_max +1,&cpy_reads, 0 , 0, &timeout)) == -1){	// signal accept
		continue;
	}
	if(fd_num == 0)	// timeout
		continue;
	for( i = 0; i < chat->user_cnt; i++)
	{
		if(FD_ISSET(chat->users[i],&cpy_reads))
		{
			memset(message, 0, BUFSIZ);
			str_len=recv(chat->users[i],message,BUFSIZ,0);
			send_msg(message,chat,i);
			message[str_len] = 0;
			printf("[Ch. %d] message %s\n",chat->room_num, message);	
			pthread_mutex_lock(&mutx);
			if(!strcmp(message,"quit\n"))		// 'quit 와 같은 해제 문자 입력시 탈출
			{
				printf("[Ch. %d] client : %d quit\n",chat->room_num,chat->users[i]);
				FD_CLR(chat->users[i],&reads);
				
				return_sock[return_cnt++] = chat->users[i];
				pntArr(chat->users,chat->user_cnt);		// check users
				delInd(chat->users,&(chat->user_cnt), i);	// chat->users에서 i인덱스 사용자 제거
				pntArr(chat->users,chat->user_cnt);		// check users

				/* 서버 대기실에 추가 */
			}
			if(str_len == 0){		// 임시로 해놓은것임
				FD_CLR(chat->users[i],&reads);
				close(chat->users[i]);
				pntArr(chat->users,chat->user_cnt);		// check users
				delInd(chat->users,&(chat->user_cnt), i);	// chat->users에서 i인덱스 사용자 제거
				pntArr(chat->users,chat->user_cnt);		// check users
			}
			pthread_mutex_unlock(&mutx);
		}
	}

	}
}
void null(int sig)
{
	printf("NULL signal!!\n");
}
void delInd(int* arr, int* size, int ind)
{
	for(; ind < *size - 1; ind++)
	{
		arr[ind] = arr[ind + 1];
	}
	(*size)--;
}
void pntArr(int *arr, int size)
{
	int i;
	printf("pntArr : ");
	for(i = 0; i < size; i++)
	{
		printf("[%d] ",arr[i]);
	}
	printf("\n");
}
void send_msg(char * msg, struct chatting_room *chat,int users_num)
{
	int i;
	char * message;

	message = (char *)calloc(1,strlen(msg) + 5);
	pthread_mutex_lock(&mutx);
	for(i = 0; i < chat->user_cnt; i++){
		memset(message, 0 , strlen(msg + 5));
		if( i != users_num)	// 다른 사용자에게 보내는 메세지
		{
			sprintf(message,"[%d] %s",chat->users[i],msg);
			send(chat->users[i],message, strlen(message), 0);
			printf("send message to [%d] [%s]\n",chat->users[i],message);
		}else			// 나에게 보내는 메세지
		{
			sprintf(message,"[ME] %s",msg);
			send(chat->users[i], message, strlen(message), 0);
			printf("send message to [%d] [%s]\n",chat->users[i],message);
		}
	}
	pthread_mutex_unlock(&mutx);
	free(message);
}
void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
