#include <stdio.h>


#define BUF_SIZE 1024

int main()
{
	FILE * src = fopen("Book.txt","rb");
	FILE * des = fopen("copy.txt","wb");
	char buf[BUF_SIZE];
	int readCnt;
	
	if(src== NULL || des == NULL) {
		puts("fail to file open");
		return -1;
	}

	while(1)
	{
		readCnt = fread((void*)buf,1,BUF_SIZE,src);

		if(readCnt < BUF_SIZE)
		{
			if(feof(src) != 0)
			{
				fwrite((void*)buf, 1,readCnt,des);
				puts("파일 복사 완료 !!\n");
				break;
			}
			else
				puts("파일복사 실패\n");
			break;
		}
		fwrite((void*)buf,1,BUF_SIZE,des);
	}
	fclose(src);
	fclose(des);

	return 0;

}
