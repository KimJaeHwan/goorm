#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY_NUM 9999


void error_handling(char * message);
int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	char message[BUFSIZ];
	
	struct sockaddr_in serv_adr,clnt_adr;
	socklen_t clnt_adr_sz;
	int str_len;
	
	int pid;
	int shm_id;
	void *shm_addr;
	
	if(argc != 2)
	{
		printf("Usage : %s <port> \n",argv[0]);
		exit(1);
	}

	serv_sock = socket(AF_INET, SOCK_STREAM,0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_adr_sz = sizeof(clnt_adr);
	
	shm_id = shmget( (key_t)KEY_NUM, sizeof(int), IPC_CREAT | 0666);
	shm_addr = shmat( shm_id,(void *)0, 0);

	
	*((int *)shm_addr) = 0;
	while(1){
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		*((int *)shm_addr) = *((int *)shm_addr) + 1;
		if(clnt_sock == -1)
			error_handling("accept() error");
		else
			printf("Number of service  client : %d \n",*((int *)shm_addr));
		pid = fork();
		switch(pid){
			case 0:
				close(serv_sock);
				while((str_len = recv(clnt_sock, message, BUFSIZ,0)) != 0){
					message[str_len] = 0;
					if(!strcmp(message, "quit\n")){
						*((int *)shm_addr) = *((int *)shm_addr) - 1;
						printf("Number of service  client : %d \n",*((int*)shm_addr));
						close(clnt_sock);
						exit(0);
						break;
					}
					printf("Recv from client : %s\n",message);
					send(clnt_sock, message, str_len, 0);
				}

		}
		close(clnt_sock);
	}
	shmdt(shm_addr);
	shmctl(shm_id,IPC_RMID,0);
	close(clnt_sock);
	close(serv_sock);
	return 0;
}
void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
