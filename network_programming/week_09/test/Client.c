#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>

#define BUFF_SIZE 1024
#define FILE_SERVER "./socket_addr"

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int clnt_sock;
	struct sockaddr_un serv_adr;
	char buff[BUFF_SIZE+5];

	clnt_sock = socket(PF_FILE, SOCK_STREAM, 0);
	if( -1 == clnt_sock)
		error_handling("socket() error");

	memset(&serv_adr, 0,sizeof(serv_adr));
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path,FILE_SERVER);

	if( connect(clnt_sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");

	write( clnt_sock, argv[1], strlen(argv[1]) + 1);
	read( clnt_sock,buff, BUFF_SIZE);
	printf("%s\n",buff);
	close(clnt_sock);

	return 0;
}

void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
