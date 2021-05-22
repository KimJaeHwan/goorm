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

#define MAX_ROOM 4	// 대기실 1개 채팅룸 3개
#define MAX_USER 10
#define MAX_CAPA 5
#define KEY_NUM 1234

struct chatting_room{
	int room_num;
	int users[MAX_CAPA];
	int user_cnt;
};

void * handle_clnt(void * arg);
void send_msg(char *msg, int str_len, struct chatting_room *chat);
void error_handling(char * message);
int maxArr(int * arr, int size);

int test = 0;


/* thread vlaues */
int clnt_sock;
pthread_mutex_t mutx;

int main(int argc, char * argv[])
{
	int serv_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	socklen_t clnt_adr_sz;

	/* select value */
	int fd_max, str_len, fd_num, i,j;
	
	char message[BUFSIZ];
	char menu_list[200] = "<MENU>\n1.채팅방 목록보기\n2. 채팅방 참여하기(사용법 : 2 <채팅방 번호>)\n3. 프로그램종료\n(0을 입력하면 메뉴가 다시 표시됩니다)\n";


	//struct chatting_room chat[MAX_ROOM];
	struct chatting_room *chatp;
	
	/* thread values */
	pthread_t t_id;
	
	int chat_room_num;
	/* shared memory values
	int shm_id;
	void * shm_addr;
	int pid;
	*/

	if(argc!=2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	
	//int chat_room_num;
	
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


	/* 서버(대기실)의 상태 정보 초기화*/
	//chat_room_num = 0;
	//chatp[0].pid = 1111;
	chatp[0].room_num = 0;
	chatp[0].user_cnt = 0;

	for(i = 1;i<MAX_ROOM;i++){
		chatp[i].room_num = i;
		pthread_create(&t_id, NULL, handle_clnt,(void*)&chatp[i]);
	}
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;
	while(1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

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
					chatp[0].users[chatp[0].user_cnt] = clnt_sock;
					chatp[0].user_cnt++;
					FD_SET(clnt_sock, &reads);
					if(fd_max < clnt_sock)
						fd_max = clnt_sock;

					printf("connected client: %d \n",clnt_sock);

					send(clnt_sock,menu_list,strlen(menu_list),0);
					printf("send menu\n");
				}
				else
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
						
						switch(message[0]){
							case '0':
								send(clnt_sock,menu_list,strlen(menu_list),0);
								printf("send menu\n");
								break;
							case '1':
								strcpy(message,"<ChatRomm info>\n");
								for(i = 1;i<MAX_ROOM;i++)
								{
									sprintf(message,"%s[ID: %d] Chatroom-%d (%d/%d)\n",message,i,i,chatp[i].user_cnt,MAX_CAPA);
								}
								send(clnt_sock, message,strlen(message), 0);
								break;
							case '2':
								if(fd_max == clnt_sock)		// 현재 fd_max값이 채팅방에 참여하고자 하는 clnt이면 값을 하나 내린다.
									fd_max--;

								FD_CLR(clnt_sock,&reads);	// 채팅방에 참여하고자하는 clnt를 select 검사에서 삭제
								/* n번 채팅방 참여 시도 n번이 현재 존재하는 채팅방인지 체크하는 부분이 필요할듯*/						
								chat_room_num = atoi(message + 2) + 1;		// 0 번이 들어오면 1번 인덱스에 값을 넣게 되므로 1을 더해준다.
								pthread_mutex_lock(&mutx);

								printf("%d[[]]\n",chatp[chat_room_num].user_cnt);
								chatp[chat_room_num].users[chatp[chat_room_num].user_cnt++] = clnt_sock;
								test++;	// 이곳을 지나면 더이상 서버 메인 프로세스에서는 입력을 받지 못한다.		그리고 왜인지모르겠는데 chatp[chat_room_num].user_cnt가 2이다...
								printf("chat_room_num : %d %d]\n",chat_room_num,test);
								
								pthread_mutex_unlock(&mutx);

								//pthread_create(&t_id, NULL, handle_clnt,(void*)&chatp[chat_room_num]);						
								printf("채팅방 %d번입장 \n",chat_room_num);
								
								break;
							case '3':
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
	int * users;
	int user_cnt;
	//chat_room_num = *((int*) arg);
	char message[BUFSIZ];
	struct chatting_room* chat;

	chat = (struct chatting_room *)arg;

	printf("Thread chatting room open [%d]\n",chat->room_num);

	pthread_mutex_lock(&mutx);
	users = chat->users;
	user_cnt = chat->user_cnt;
	FD_ZERO(&reads);
	pthread_mutex_unlock(&mutx);

	while(1){
	pthread_mutex_lock(&mutx);
	printf("[Ch. %d user_cnt : %d\n",chat->room_num,chat->user_cnt);
	printf("Test : %d\n",test);
	for(i = 0;i < user_cnt; i++)
	{
		FD_SET(users[i],&reads);
		if( fd_max < users[i]) 
			fd_max = users[i];
		printf("[Ch. %d user : %d\n",chat->room_num,users[i]);
	}
	pthread_mutex_unlock(&mutx);
	
	cpy_reads = reads;
	timeout.tv_sec = 1;
	timeout.tv_usec = 1000;
	if((fd_num = select(fd_max +1,&cpy_reads, 0 , 0, &timeout)) == -1)
		break;
	if(fd_num == 0)	// timeout
		continue;
	for( i = 0; i < user_cnt; i++)
	{
		if(FD_ISSET(users[i],&cpy_reads))
		{
			while((str_len=recv(users[i], message,BUFSIZ,0))!= 0)
				send_msg(message,str_len, chat);
			message[str_len] = 0;
			printf("[Ch. %d] message %s\n",chat->room_num, message);	
			pthread_mutex_lock(&mutx);
			if(str_len == 0)		// 'quit 와 같은 해제 문자 입력시 탈출
			{
				FD_CLR(users[i],&reads);
				chat->users[chat->user_cnt++] = users[i];
				while(i++<user_cnt+1)	// 배열 요소에서 제거
					users[i] = users[i + 1];
				user_cnt--;
				chat->user_cnt = user_cnt;
			}
			pthread_mutex_unlock(&mutx);
		}
	}

	}
}

void send_msg(char * msg,int str_len, struct chatting_room *chat)
{
	int i;

	pthread_mutex_lock(&mutx);
	for(i = 0; i < chat->user_cnt; i++)
		send(chat->users[i], msg, str_len, 0);
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
