#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>		// timeout안쓰면 삭제해도 됨
#include <sys/select.h>

void error_handling(char * message);

#define MAX_ROOM 3
#define MAX_USER 10
#define MAX_CAPA 5

struct chatting_room{
	int pid;
	int users[MAX_CAPA];
	int user_cnt;
};

int main(int argc, char * argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	socklen_t clnt_adr_sz;

	int fd_max, str_len, fd_num, i,j;
	char message[BUFSIZ];
	char menu_list[200] = "<MENU>\n1.채팅방 목록보기\n2. 채팅방 참여하기(사용법 : 2 <채팅방 번호>)\n3. 프로그램종료\n(0을 입력하면 메뉴가 다시 표시됩니다)\n";


	struct chatting_room chat[MAX_ROOM];
	int fork_pid;

	if(argc!=2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0 , sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;
	for( i = 0;i<MAX_ROOM;i++){			// 3개의 자식 프로세스 생성
		if(chat[i].pid = fork())	// 포크된 결과가 자식이면 그만
			break;
	}
	if(fork_pid == 0){	// 자식 프로세스
		

	}
	else{			// 부모 프로세스
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
						
						/* broad cast other user


						for(j = 4; j < fd_max +1; j++)
						{
							send(j,message,str_len,0);
							printf("send client %d\n",j);
						}
						
						printf("fd_max : %d\n", fd_max);
						send(i,message,str_len,0);

						*/

						switch(message[0]){
							case '0':
								send(clnt_sock,menu_list,strlen(menu_list),0);
								printf("send menu\n");
								break;
							case '1':
								strcpy(message,"<ChatRomm info>\n");
								for(i = 0;i<MAX_ROOM;i++)
								{
									sprintf(message,"%s[ID: %d] Chatroom-%d (%d/%d)\n",message,i,i,chat[i].user_cnt,MAX_CAPA);
								}
								send(clnt_sock, message,strlen(message), 0);
								break;
							case '2':
								break;
							case '3':
								break;
						}

					}
				}
			}
		}
	}
	}
	close(serv_sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
