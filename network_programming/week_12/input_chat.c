#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int sock;
	struct sockaddr_un serv_adr;

	char message[BUFSIZ];
	int str_len;

	if(argc != 2)
	{
		printf("Usage : %s <unix_file>",argv[0]);
		exit(1);
	}

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("sock() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path,argv[1]);

	if(connect(sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected......");

	while(1)
	{
		fputs("> Enter : ", stdout);
		fgets(message, BUFSIZ, stdin);

		send(sock, message, strlen(message), 0);
		if(atoi(message) == 3)
			break;
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
