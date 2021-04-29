#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
void error_handling(char *message);
int main(int argc, char *argv)
{
	int sock;
	char message[999];
	int str_len;
	struct sockaddr_un serv_adr;
	
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0,sizeof(serv_adr));
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path,"./sock_addr");

	if(connect(sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected................");
	while(1)
	{
		fputs("Inputs message(): ",stdout);
		fgets(message, 999, stdin);
		if( send(sock,message,999,0) == -1)
			error_handling("send() error");
		if(!strcmp(message,"\\quit\n"))
			break;

		//str_len = read(sock,message, 998);
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

