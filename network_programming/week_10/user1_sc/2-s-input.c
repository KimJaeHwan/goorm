#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define BUF_SIZE 1024
#define UNIX_PATH "./unix_socket_addr"

void error_handling(char * message);
int main(int argc, char * argv[])
{
	int sock;
	struct sockaddr_un serv_adr;
	
	char message[BUF_SIZE];
	int str_len;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0 , sizeof(serv_adr));
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path,UNIX_PATH);

	if(connect(sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected.........");
	while(1)
	{
		fputs("> Enter: ",stdout);
		fgets(message,BUF_SIZE -1,stdin);
	
		send(sock, message, strlen(message), 0);
		/* \\quit */
		if(!strcmp(message,"quit\n"))
			break;
	/* send message to server */
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
