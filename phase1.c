#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define GLOBAL 22
#define GLOBAL1 20

void break_chunk(char process[],int size,char* outfile,char* function);
void sort(char solved[][GLOBAL],int size);
void forward(char buff[],char* outfile,int track);
void backward(char*infile,char* outfile);
void dobackward(char process[],char* outfile,int size);

int compare_function(const void *a,const void *b) 
{
return (strcmp((char *)a,(char *)b));
}

int main(int argc,char* argv[])
{
	
	char* function = argv[1];
	char* infile = argv[3];
	char* outfile = argv[5];
	int i;
	char process[GLOBAL];
	int track=0;
	char t;
	
	if(strcmp(function,"--forward")==0)
	{
		
		FILE *input_file;
		input_file = fopen(infile,"r");
		unsigned char a = 0xab;
		unsigned char b = 0xba;
		unsigned char c = 0xbe;
		unsigned char d = 0xef;
		unsigned int e = 0b10100;
		
		FILE *fp;
		fp = fopen(outfile, "w");
		fwrite(&a, sizeof(char), 1, fp);
		fwrite(&b, sizeof(char), 1, fp);
		fwrite(&c, sizeof(char), 1, fp);
		fwrite(&d, sizeof(char), 1, fp);
		fwrite(&e, sizeof(int), 1, fp);
		fclose(fp);
		if(!input_file)
		return 1;
		while((t = fgetc(input_file))!= EOF)
		{
		
			if(track<20)
			{
			
				process[track++] = t;
				process[track] = '\x03';
				
			}
		
			else 
			{
			track++;
			forward(process,outfile,track);
			track=0;
			
			for(i=0;i<GLOBAL;i++)
			{
			process[i] = '\0';
			}
			process[track] = t;
			track++;
			}
		}
			printf("%s",process);
		forward(process,outfile,track);	
	
	}
	else if(strcmp(function,"--backward")==0)
	{
		backward(infile,outfile);
	}
}
	
void forward(char buff[],char* outfile, int track)
{	
	
	buff[track+1] = '\0';
	int i,j,k,l,size = strlen(buff);
	char done;
	char process[GLOBAL];
	strcpy(process,buff);
	char rotated[strlen(buff)][GLOBAL];
	FILE *fp;
	fp = fopen(outfile, "a");
	for(i=0;i<strlen(buff);i++)
	{
		if(!(i==strlen(buff)))
		{
			strcpy(rotated[i],process);
		}
	
	
		for(j=i+1,l=0;j<strlen(buff);j++,l++)
			{	 	
				process[l] = buff[j];
			}
		for(k=0;k<=i;k++)
			{ 
			process[l] = buff[k];
			l++;
			}
	}
	qsort(rotated,size,GLOBAL,compare_function);
	for(i=0;i<size;i++)
	  {
		done = rotated[i][size-1];
		fwrite(&done, sizeof(char), 1, fp);
	  }
	  fclose(fp);
}
void backward(char* infile,char* outfile)
{
	char process[GLOBAL];
	FILE* input_text;
	input_text = fopen(infile,"r");
	int i;
	int track = 0;
	int ch;
	for(i=0;i<GLOBAL;i++)
		{	
	  process[i] = '\0';
		}
	
	fseek(input_text, 8, SEEK_SET);
	int size  = strlen(process);
	while((track = fread(process,sizeof(char),21,input_text))>0)
	{
		dobackward(process,outfile,track);
		for(i=0;i<GLOBAL;i++)
		{	
	  process[i] = '\0';
		}
	}
		
}	

void dobackward(char process[],char* outfile,int size)
{
	int found = 0;
	int i,k,l=0;
	FILE* output_file;
	output_file = fopen(outfile,"a");
	int ch;
	char unsolved[size][GLOBAL];
	char solved[size][GLOBAL];
	
	for(i=0;i<size;i++)
	{
		unsolved[i][0] = process[i];
		
	}
	for(k=1;k<size;k++)
	{
	
		for(i=0;i<size;i++)
		{
			strcpy(solved[i],unsolved[i]);
		}
		qsort(solved,size,GLOBAL,compare_function);
		for(i=0;i<size;i++)
		{	
			unsolved[i][k] = solved[i][l];
		}
	l++;
	}
		for(i=0;i<size;i++)
	{
		if(unsolved[i][size-1]=='\x03')
		{
			found = i;
		}
	}
	for(i=0;i<size-1;i++)
	{
		ch = unsolved[found][i];
		fwrite(&ch, sizeof(char), 1, output_file);
	}
	fclose(output_file);
	
}	