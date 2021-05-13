#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
void error_handling(char * message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUFSIZ];
	int str_len;
	struct sockaddr_in serv_adr;

	if(argc !=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	sock=socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else
		puts("Connected.........");

	while(1)
	{
		fputs("Input message(quit) : ", stdout);
		fgets(message, BUFSIZ, stdin);

		send(sock,message, strlen(message),0);
		if(!strcmp(message,"quit\n"))
			break;
		str_len = recv(sock, message, BUFSIZ - 1,0);
		message[str_len] = 0;
		printf("[echo] : %s",message);
	}
	close(sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
