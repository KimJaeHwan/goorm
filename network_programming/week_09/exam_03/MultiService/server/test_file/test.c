#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *in;
	int ch;

	if( (in = fopen("Book.txt","rb")) == NULL){
		fputs("fopen() error",stderr);
		exit(1);
	}
	while( (ch = fgetc(in)) != EOF)
	{
		printf("%c",ch);
	}

	fclose(in);
	return 0;
}
