#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define N_CLIENT 3
void error_handling(char * message);
int maxArr(int * arr, int size);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock[N_CLIENT];

	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr[N_CLIENT];

	fd_set read;
	socklen_t clnt_adr_sz[N_CLIENT];
	int fd_max, str_len, fd_num, i;

	char message[BUFSIZ];

	if(argc != 2)
	{
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	memset(&serv_adr, 0 , sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	
	for(i = 0; i < N_CLIENT; i++){
		if((clnt_sock[i] = accept(serv_sock, (struct sockaddr*)(clnt_adr + i), clnt_adr_sz + i)) == -1){
			error_handling("accept() error");	
		}
		else
			printf("Client #%d connected. %d\n",i+1,clnt_sock[i]);
	}
	
	while(1)
	{
		FD_ZERO(&read);
		for( i = 0; i<N_CLIENT; i++)
			FD_SET(clnt_sock[i], &read);
		printf("waiting at select ... \n");
		fd_num = select(maxArr(clnt_sock, N_CLIENT) + 1, &read,NULL,NULL,NULL);
		switch(fd_num){
			case -1:
				error_handling("select() error");
			case 0:
				printf("select returns : 0\n");
				break;
			default:
				i = 0;
				while( fd_num > 0) {
					if( FD_ISSET(clnt_sock[i], &read)){
						memset(message,0,BUFSIZ);
						if(recv(clnt_sock[i], message, BUFSIZ, 0 ) == -1)
							error_handling("recv() error");
						fd_num--;
						printf("MSG from client %d: %s\n", i , message);
							
					}else;
					i++;
				}
				break;
		}
	}
}
void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

int maxArr(int * arr, int size){
	int max = -1;
	int i;
	for ( i = 0; i < size; i++){
		if(max < arr[i])
			max = arr[i];
	}
	return max;
}
