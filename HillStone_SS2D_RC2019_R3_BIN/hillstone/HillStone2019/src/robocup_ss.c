#include<stdio.h>
#include <stdlib.h>
int main(void)
{
	FILE *wri,*rea;
	char str[256];
	int sum,reading,ret;
	if( (rea= (fopen("reader.dat","r"))) ==NULL)
	{
		printf("read error！\n");
		exit(EXIT_FAILURE);
	}
	
	while(ret= (fscanf(rea,"%d",&reading)!=EOF))
	{
	
		sum = reading+1;
		wri = fopen("reader.dat","w");
		fprintf(wri,"%d",sum);
	}
	fclose(rea);
	fclose(wri);			
	return 0;
}
