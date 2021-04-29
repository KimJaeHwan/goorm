#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char * message);

int main( int argc, char * argv[])
{
	int sock;
	char message[BUF_SIZE];
	char in_mess[BUF_SIZE];
	int str_len,i;
	struct sockaddr_in serv_adr;

	if(argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0 ,sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected..............");
	
	//for(i = 0; i < 5; i++)
	while(1)
	{
		fputs("INput message : ",stdout);
		fgets(in_mess, BUF_SIZE, stdin);
		send(sock, in_mess,strlen(in_mess),0);
		if(!strcmp(in_mess,"\\quit\n")){
			fputs("close socket\n",stdout);
			break;
		}
		recv(sock, message,BUF_SIZE,0);
		if(!strcmp(message,"\\quit\n")){
			fputs("Receive quit message!!\n",stdout);
			fputs("close socket\n",stdout);
			break;
		}
		printf("[you] %s",message);  
	}

	close(sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
