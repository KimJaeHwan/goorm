#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define UNIX_PATH "./unix_socket_addr"

void error_handling(char * message);

int main( int argc, char * argv[])
{
	/* unix socket values */
	int unix_serv_sock, unix_clnt_sock;
	struct sockaddr_un unix_serv_adr, unix_clnt_adr;
	socklen_t unix_clnt_adr_sz;

	/* inet socket values */
	int inet_serv_sock;
	struct sockaddr_in inet_serv_adr;

	int str_len;
	char message[BUF_SIZE];

	if(argc != 3)
	{
		printf("Usage : %s <IP> <port>\n",argv[0]);
		exit(1);
	}

	/* unix socket open */
	unix_serv_sock = socket(AF_UNIX,SOCK_STREAM, 0);
	if(unix_serv_sock == -1)
		error_handling("unix] socket() error");

	memset(&unix_serv_adr, 0 , sizeof(unix_serv_adr));
	unix_serv_adr.sun_family = AF_UNIX;
	strcpy(unix_serv_adr. sun_path, UNIX_PATH);

	if(0 == access(UNIX_PATH,F_OK))
		unlink(UNIX_PATH);

	if(bind(unix_serv_sock, (struct sockaddr*)&unix_serv_adr, sizeof(unix_serv_adr)) == -1)
		error_handling("unix] bind() error");

	if(listen(unix_serv_sock, 5) == -1)
		error_handling("unix] listen() error");

	unix_clnt_adr_sz = sizeof(unix_clnt_adr);

	unix_clnt_sock = accept(unix_serv_sock , (struct sockaddr*)&unix_clnt_adr, &unix_clnt_adr_sz);
	if(unix_clnt_sock == -1)
		error_handling("unix] accept() error");
	else
		printf("unix] Connected client\n");
	/* end unix socket open */

	/* inet socket open */
	
	inet_serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(inet_serv_sock == -1)
		error_handling("inet] socket() error");

	memset(&inet_serv_adr, 0 ,sizeof(inet_serv_adr));
	inet_serv_adr.sin_family = AF_INET;
	inet_serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	inet_serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(inet_serv_sock ,(struct sockaddr*)&inet_serv_adr, sizeof(inet_serv_adr)) == -1)
		error_handling("inet] connect() error");
	else
		puts("inet] Connected..........");
	
	/* end inet socket open */

	fcntl(unix_clnt_sock, F_SETFL, fcntl(unix_clnt_sock, F_GETFL, 0) | O_NONBLOCK);
	fcntl(inet_serv_sock, F_SETFL, fcntl(inet_serv_sock, F_GETFL, 0) | O_NONBLOCK);
	/* recv from input message */
	while(1){
		//while((str_len = recv(unix_clnt_sock,message, BUF_SIZE -1,0)) < 0 | ) { usleep(3000); }
		while(1)
		{
			str_len = recv(unix_clnt_sock,message, BUF_SIZE -1, 0);
			if(str_len > 0)
			{
				message[str_len] = 0;
				printf("[ME] : %s",message);
				send(inet_serv_sock, message, strlen(message), 0);
			}
			str_len = recv(inet_serv_sock , message, BUF_SIZE -1, 0);
			if(str_len > 0)
			{
				message[str_len] = 0;
				printf("[YOU] : %s",message);
			}
			usleep(3000);
		}
	}
	/* end recv from input message  */
	
	close(inet_serv_sock);
	close(unix_clnt_sock);
	close(unix_serv_sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
