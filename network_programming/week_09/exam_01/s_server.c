#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>

void error_handling(char *message);
int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_un serv_adr , clnt_adr;
	socklen_t clnt_adr_sz;
	int str_len;
	char message[999];
	
	
	serv_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path, "./sock_addr");
	
	if( 0== access("./sock_addr",F_OK))
		unlink("./sock_addr");

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1 )
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("bind() error");
	clnt_adr_sz = sizeof(clnt_adr);

	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	if(clnt_sock == -1)
		error_handling("accept() error");
	else
		printf("Connected client\n");

	while(1)
	{
		str_len = recv(clnt_sock, message, 999, 0);
		if(!strcmp(message,"\\quit\n"))
		{
			printf( "quit message from client\n");
			printf( "close server_socket\n");
			break;
		}
		printf("message from client : %s", message);
		send(clnt_sock, message,str_len,0);
	}
	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
