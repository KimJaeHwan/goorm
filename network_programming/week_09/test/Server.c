#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BUFF_SIZE 1024
#define FILE_SERVER "./socket_addr"

void error_handling(char*message);

int main(void)
{
	int serv_sock, clnt_sock;
	int clnt_adr_sz;
	int option;

	struct sockaddr_un serv_adr, clnt_adr;

	char buff_rcv[BUFF_SIZE+5];
	char buff_snd[BUFF_SIZE+5];

	if ( 0 == access( FILE_SERVER,F_OK))
		unlink( FILE_SERVER );
	serv_sock = socket(PF_FILE, SOCK_STREAM, 0);

	if( -1 == serv_sock)
		error_handling("socket() error");

	memset(&serv_adr, 0 ,sizeof(serv_adr));
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path, FILE_SERVER);

	if( bind(serv_sock, (struct sockaddr*)&serv_adr,sizeof(serv_adr)) == -1)
		error_handling("bind() erro");

	while(1)
	{
		if(listen(serv_sock,5) == -1)
			error_handling("listen() error");

		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);

		if( -1 == clnt_sock)
			error_handling("accept() error");

		read( clnt_sock, buff_rcv, BUFF_SIZE);
		printf( "receive: %s\n", buff_rcv);

		sprintf( buff_snd , "%d : %s ", strlen( buff_rcv),buff_rcv);
		write( clnt_sock, buff_snd, strlen( buff_snd) +1);
		close( clnt_sock);
	}
	return 0;
}

void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
