#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char * message);

int main(int argc, char * argv[])
{
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	char in_mess[BUF_SIZE];
	int str_len, i;

	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;

	if(argc != 2){
		printf("Usage : %s <port> \n", argv[0]);
		exit(1);
	}

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_adr_sz = sizeof(clnt_adr);

	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	if(clnt_sock == -1)
		error_handling("accept() error");
	else
		printf("Connected client\n");

	//for( i = 0; i < 5; i++){	// 임시로 5번 반복
	while(1){
		str_len = recv(clnt_sock, message, BUF_SIZE,0);
		if(!strcmp(message,"\\quit\n"))
		{
			fputs("Receive quit message!!\n",stdout);
			fputs("close socket\n",stdout);
			break;
		}
		printf("[You] %s",message);
		printf("Input message : ");
		fgets(in_mess,BUF_SIZE,stdin);
		send(clnt_sock, in_mess, BUF_SIZE,0);
		if(!strcmp(in_mess,"\\quit\n")){
			fputs("close socket\n",stdout);
			break;
		}
	}
	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
