#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 2048
void error_handling(char * message);

int main(int argc, char * argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len,num;
	struct sockaddr_in serv_adr;
	char menu_service[20] ="\\service ";
	char menu_num[2];
	char temp;
	FILE * file;
	//char file_name[99];
	if(argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		fputs("Connected..........",stdout);


	str_len = recv(sock, message, BUF_SIZE,0);	// 서버 리스트 받아오는 recv
	message[str_len] = 0;				// recv함수로 받아오면 왜인지 모르게 뒤에 null이 없어지는듯
	printf("\n [Server] List]\n%s\n",message);	// 서버 리스트 출력
	fputs("Enter : ",stdout);
	fgets(menu_num,sizeof(menu_num),stdin);
	//scanf("%c%c",&menu_num,&temp);
	strcat(menu_service,menu_num);			// \\service <num>
	//printf("%s",menu_service);
	//menu_service[8] = menu_num;
	//printf("%s",menu_service);
	send(sock,menu_service,strlen(menu_service),0);	// \\service <num> 전송
	
	if(!strcmp(menu_num,"1")){
		//printf("%s",menu_num);	
		str_len = recv(sock, message, BUF_SIZE,0);    	// 시간 받아서 출력까지
		message[str_len] = 0;
		printf("\n%s\n",message);		// ..
	}else if(!strcmp(menu_num,"2")){
		str_len = recv(sock,message, BUF_SIZE,0);
		message[str_len] = 0;
		printf("\n%s\n",message);
		getchar();				// 버퍼 비우기용
		fputs("Enter : ",stderr);
		//fflush<stdin>;
		fgets(menu_num,sizeof(menu_num),stdin);
		//scanf("%c%c",&menu_num,&temp);	
		//printf("menu_num[%s]\n",menu_num);	// 테스트용 출력
		message[0] = menu_num[0];
		message[1] = 0;
		//printf("message[%s]\n",message);	// 테스트용 출력
		send(sock,message,strlen(message),0);	// 파일 번호 보내기
		
		str_len = recv(sock,message,BUF_SIZE,0);		// 파일 이름 수신
		message[str_len] = 0;
		printf("%s]",message);
		send(sock,message,str_len,0);				// 동기화를 위한 send
		
		file = fopen(message,"wb");
		
		if(file == NULL)
			error_handling("fopen() error");

		while(1)
		{
			str_len = recv(sock,message,BUF_SIZE,0);	// -1
			if(str_len < BUF_SIZE )
			{
				//printf("보이냐\n\n\n\n\n\n\n\n\n\n");
				fwrite((void*)message,1,str_len,file);	// -1
				puts("파일 수신 완료 !!\n");
				break;
			}else{
				send(sock,message,1,0);				//동기화를 위한 send
				//message[str_len] = 0;
				//printf("[%d]",str_len);
				fwrite((void*)message,1,BUF_SIZE,file);
			}
		}
		fclose(file);
		
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
