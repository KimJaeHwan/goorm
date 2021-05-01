#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define BUF_SIZE 2048
#define SERV_LIST "1. Get Current Time\n2. Download File\n3. Echo Server"
#define FILE_LIST "1. Book.txt\n2. HallymUniv.jpg\n3. Go back"
void error_handling(char * message);

int main(int argc, char * argv[])
{
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	char service_list[99];
	char *file_list[9] = {"Book.txt","HallymUniv.jpg"};
	int str_len, i;

	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	
	char temp;
	int num;
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	FILE * file;

	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1 )
		error_handling("bind() error");

	if(listen(serv_sock , 5) == -1)
		error_handling("listen() error");

	clnt_adr_sz = sizeof(clnt_adr);

	while(1){
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	if(clnt_sock == -1)
		error_handling("accept() error");
	else
		printf("Connected client\n");
	strcpy(message,SERV_LIST);
	send(clnt_sock,message, strlen(message),0);	// 서버 서비스 리트스 전송
	str_len = recv(clnt_sock,message,BUF_SIZE,0);		// 서비스 번호 받기
	message[str_len] = 0;
	//fputs(message,stdout);					// 테스트용 출력
	if(!strcmp(message,"\\service 1"))			// service 1 시간 반환
	{
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		//printf("%s", asctime(timeinfo));			// 테스트용 출력
		strcpy(message,"[Current Time]\n");
		strcat(message,asctime(timeinfo));
		send(clnt_sock,message,strlen(message),0);		// 시간 전송
	}else if(!strcmp(message,"\\service 2"))
	{
		strcpy(message,"[Available File List]\n");
		strcat(message,FILE_LIST);
		send(clnt_sock, message,strlen(message),0);
		printf("send file List\n");
		str_len = recv(clnt_sock, message, BUF_SIZE,0); 			//정수 번호받음
		message[str_len] = 0;
		//fputs(message,stdout);						// 리스트 번호받음
		//printf("%d",num);
		/*
		if(!strcmp(message,"1")){				// 리스트 번호별 다운로드
			strcpy(select_file,"Book.txt");
			printf("[%d]",atoi(message));
		}
		else if(!strcmp(message,"2"))
			strcpy(select_file,"HallymUniv.jpg");
		*/
		file = fopen(file_list[atoi(message) - 1],"rb");	// 입력한 파일 오픈
		printf("file open\n");					// 테스트용 출력
		i = atoi(message) -1;
		//printf("%d",i);
		printf("%s]\n",file_list[i]);
		strcpy(message,file_list[i]);
		printf("%s] %d\n",message,sizeof(message));
		//send(clnt_sock,file_list[atoi(message) -1],strlen(file_list[atoi(message) - 1]) , 0); // 파일이름 보내기

		send(clnt_sock,message,strlen(message),0);
		printf("send : %s\nsize : %d\n",message,strlen(message));
		recv(clnt_sock,&temp,1,0);		// 동기화를 위한 recv
		
				
		if(file == NULL)
			error_handling("fopen() error");
		while(1)
		{
			i = fread((void*)message,1,BUF_SIZE -1,file);	// -1
			
			message[i] = 0;
			//printf("[%s]",message);
			if(i < BUF_SIZE -1)		//-1
			{
			printf("[%s]",message);
			
				if(feof(file) != 0)
				{
					//usleep(1000);
					send(clnt_sock,message,i,0);	//-1
					puts("file send complete!!\n");
					break;
				}else
					puts("fail to send file\n");
			       break;	
			}
			//usleep(1000);
			send(clnt_sock, message,BUF_SIZE -1,0);	// -1
			recv(clnt_sock, &temp,1,0); 	//동기화를 위한  recv
			//message[i] = 0;
			//printf("[%s]",message);
		}
		
		fclose(file);
	}
	}

	close(clnt_sock);
	close(serv_sock);

}

void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
