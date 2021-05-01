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
void voidBuffer(int s);

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
	char list_num[9];
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
	while(1){
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
			recv(clnt_sock,message,BUF_SIZE,0);			// 시간 전송후 다음 메세지를 또보내지않기위한 recv
		}else if(!strcmp(message,"\\service 2"))		// Download file
		{
			while(1){
			strcpy(message,"[Available File List]\n");
			strcat(message,FILE_LIST);
			send(clnt_sock, message,strlen(message),0);			// 파일 리스트 보내기
			printf("send file List\n");
			str_len = recv(clnt_sock, message, BUF_SIZE,0);			// 버퍼 오류
			
			printf("[reciv : %s size : %d\n",message,str_len);				// 받는 데이터 확인	
			/*
			while(str_len != 0){
				str_len = recv(clnt_sock,message,BUF_SIZE,0);
				printf("[%d]",str_len);
			}
			*/
			message[str_len] = 0;
			printf("reciv : %s size : %d\n",message,strlen(message));				// 받는 데이터 확인	
			
			strcpy(list_num,message);
			
			if(atoi(list_num) == 3){				// 3 번들어오면 나가리
				printf("잘나가니??");
				break;
			}
			
			i = atoi(list_num) - 1;

			file = fopen(file_list[i],"rb");	// 입력한 파일 오픈
			printf("file[%s] open\n",file_list[i]);					// 테스트용 출력
			//printf("%d",i);
			printf("%s]\n",file_list[i]);

			strcpy(message,file_list[i]);
			printf("%s] %d\n",message,sizeof(message));
			//send(clnt_sock,file_list[atoi(message) -1],strlen(file_list[atoi(message) - 1]) , 0); // 파일이름 보내기

			send(clnt_sock,message,strlen(message),0);			//파일 명 전송
			printf("send : %s\nsize : %d\n",message,strlen(message));
			recv(clnt_sock,&temp,1,0);		// 파일 전송전 동기화를 위한 recv 
			
				
			if(file == NULL)
				error_handling("fopen() error");
			while(1)	// 파일 전송 과정
			{
				i = fread((void*)message,1,BUF_SIZE-1,file);	// -1
				
				message[i] = 0;
			//	printf("[%s]",message);
				if(i < BUF_SIZE-1)		//-1
				{
				//printf("[%s]",message);
				
					if(feof(file) != 0)
					{
						//usleep(1000);
						send(clnt_sock,message,i,0);	//-1
						//recv(clnt_sock, message,BUF_SIZE,0);	// 동기화를 위한  recv
						puts("file send complete!!\n");
						break;
					}else
						puts("fail to send file\n");

					break;	
				}
				//usleep(1000);
				send(clnt_sock, message,BUF_SIZE-1,0);	// -1
				recv(clnt_sock, &temp,BUF_SIZE,0); 	//동기화를 위한  recv
				//message[i] = 0;
				//printf("[%s]",message);
			}	
			fclose(file);
			//voidBuffer(clnt_sock);		// 버퍼 지우기
			
			recv(clnt_sock,&temp,BUF_SIZE,0);	// 파일 수신 완료후 동기화
			recv(clnt_sock,&temp,BUF_SIZE,0);
			};
		}else if(!strcmp(message,"\\service 3"))	// ECHO server 
		{

		}


	}			// while(1) 서비스 반복용
	}			// while(1) client테스트용
	close(clnt_sock);
	close(serv_sock);

}
/*
void voidBuffer(int s){
	u_long tmpl,i;
	char tmpc;
	ioctlsocket(s,FIONREAD,&tmpl);
	for(i = 0;i<tmpl;i++) recv(s,&tmpc,sizeof(char),0);
}
*/
void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
