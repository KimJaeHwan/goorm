#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/select.h>
#include <sys/un.h>

void error_handling(char * message);

int main(int argc, char * argv[])
{
	/* inet socket values */
	int inet_sock;
	struct sockaddr_in serv_adr;

	/* unix socket values */
	int unix_serv_sock, unix_clnt_sock;
	struct sockaddr_un unix_serv_adr, unix_clnt_adr;
	socklen_t unix_clnt_adr_sz;
	char unix_path[20];			// malloc으로 바꾸면 좋을듯

	/* select values */
	fd_set reads;
	int fd_max, fd_num, i, str_len;
	char message[BUFSIZ];
	
	if(argc != 4)
	{
		printf("Usage : %s <IP> <port> <unix_file>\n",argv[0]);
		exit(1);
	}
	

	/* connect to chatting server  */
	inet_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(inet_sock == -1)
		error_handling("ient] socket() error");

	memset(&serv_adr, 0 , sizeof(serv_adr));

	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(inet_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("inet] connect() error");
	else
		puts("inet] Connected..............");

	/* end connect to chtting server */

	/* connect to input prompt */
	unix_serv_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(unix_serv_sock == -1)
		error_handling("unix] socket() error");

	memset(&unix_serv_adr, 0 , sizeof(unix_serv_adr));
	unix_serv_adr.sun_family = AF_UNIX;
	strcpy(unix_path,argv[3]);
	printf("%s\n",unix_path);

	strcpy(unix_serv_adr.sun_path, argv[3]);

	if( 0 == access(argv[3], F_OK))
		unlink(argv[3]);

	if(bind(unix_serv_sock, (struct sockaddr*)&unix_serv_adr, sizeof(unix_serv_adr)) == -1)
		error_handling("unix] bind() error");

	if(listen(unix_serv_sock, 5) == -1)
		error_handling("unix] listen() error");
	
	unix_clnt_adr_sz = sizeof(unix_clnt_adr);

	unix_clnt_sock = accept(unix_serv_sock, (struct sockaddr*)&unix_clnt_adr, &unix_clnt_adr_sz);
	if(unix_clnt_sock == -1)
		error_handling("unix] accept() error");
	else
		printf("unix] Connected client\n");
	/* end connect to input prompt */

	fd_max = unix_clnt_sock > inet_sock ? unix_clnt_sock : inet_sock;
	
	while(1){
		FD_ZERO(&reads);
		FD_SET(unix_clnt_sock, &reads);
		FD_SET(inet_sock, &reads);
		
		if((fd_num = select(fd_max + 1,&reads, 0 ,0, 0)) == -1)
			break;

		for(i = 0; i < fd_max + 1;i++)
		{
			if(FD_ISSET(i,&reads))
			{
				if((str_len = recv(i, message, BUFSIZ, 0)) == 0)
					close(i);
				if(i == inet_sock){	// receive from server
					
					message[str_len] = 0;

					/* print message from server */
					printf("%s\n",message);
				}
				else			// receive from input message
				{
					send(inet_sock,message,str_len,0);
				}
			}
		}
	}
	close(inet_sock);
	close(unix_serv_sock);
	close(unix_clnt_sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
