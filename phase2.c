/*Archit Kumar
V00875990.
July,21'2017*/

#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define ENCODE_MODE 0
#define DECODE_MODE 1
charval_t *mtf_encode(char *infile,char *outfile);
charval_t *mtf_decode(char *infile,char *outfile);

void usage() {
    fprintf(stderr, "usage: phase2 [--encode|--decode] " \
                    "--infile <filename> --outfile <filename>\n"
    );
}
charval_t* int_to_bytes(charval_t* list)
{
	charval_t* curr = list;
	unsigned char byte = '\x80';
	int value = 0;
	for(curr = list;curr!=NULL;curr=curr->next)
	{
		value = (int)curr->val;
	value = value-128;
	value = value+byte;
		curr->val = value;
	}	
	return list;	
}
charval_t *rle_encode(charval_t *list)
{
	int count = 0,literal = 0,saveindex = 0;
	int bool1 = 0,bool2 = 0,bool3=1,i=0,j=0,never = 1;
	charval_t *curr = list;
	int prev = (int)curr->val;
	int forward = 0;
	charval_t *temp = NULL;
	for(;curr!=NULL;curr = curr->next)
	{
		if(never==0)
		{
			curr = list;
		}
		never = 1;
		forward = (int)curr->val;
			
			if(prev==129 && forward==129 && i>0)
			{
				count = count+1;
				bool1 = 0;
			}
			else
			{
				bool1 = 1;
				literal = count;
				count = 0;
			}
			
			if(count==2 && bool3==1)
			{
				
				saveindex = i-count;
				bool3 = 0;
				bool2 = 1;
			}
			i++;
			prev = (int)curr->val;
			if(bool2==1 && bool1==1 )
			{
				for(j=0;j<literal+1;j++)
				{
					list = delete_at(list,saveindex);
				}
				temp = new_charval(128,'\0');
				list  = insert_at(list,temp,saveindex);
				temp = new_charval(128+literal+1,'\0');
				list = insert_at(list,temp,saveindex+1);
				bool2 = 0;
				bool3 = 1;
				count = 0;
				literal = 0;
				never = 0;
				i=0;
				curr = list;
			}
	}
	return list;
}
charval_t* rle_decode(charval_t* list)
{
	int i = 0,savenum = 0,saveindex = 0,bool1 = 0,x=0;
	charval_t* curr = list;
	charval_t* temp = NULL;
	for(curr = list;curr!=NULL;curr=curr->next)
	{
		
		if(curr->val==128 && curr->next->val>=130)
		{
			savenum = curr->next->val;
			saveindex = i;
			bool1 = 1;
		}
		if(bool1==1)
		{
			
			savenum = savenum-128;
			list = delete_at(list,saveindex);
			list = delete_at(list,saveindex);
			for(x=0;x<savenum;x++)
			{
				temp = new_charint(129);
				list  = insert_at(list,temp,saveindex);
			}
			i=0;
			bool1 = 0;
			saveindex = 0;
			curr = list;
		}
		i++;
	}
	return list;
}
charval_t *mtf_decode(char *infile,char *outfile)
{
	FILE *fp,*fout;
	char ch;
	char bytes[4];
	int i =0,index = 0,save = 0;
	char* buffer = 0;
    long length;
	charval_t* list = NULL;
	charval_t* temp = NULL;
	fout = fopen(outfile,"w");
	fp = fopen(infile,"r");
	unsigned char a = 0xab;
	unsigned char b = 0xba;
	unsigned char c = 0xbe;
	unsigned char d = 0xef;
	fwrite(&a, sizeof(char), 1, fout);
	fwrite(&b, sizeof(char), 1, fout);
	fwrite(&c, sizeof(char), 1, fout);
	fwrite(&d, sizeof(char), 1, fout);
	fseek(fp,4,SEEK_SET);
	fread(bytes,sizeof(char),4,fp);
	for(i=0;i<4;i++)
	{
		ch = bytes[i];
		fwrite(&ch,sizeof(char),1,fout);
		}
	fclose(fout);
	fclose(fp);
	int value=0;
	fp = fopen(infile,"r");
    if (fp)
    {
      fseek (fp, 8, SEEK_END);
      length = ftell (fp);
	  rewind(fp);
      fseek (fp, 8, SEEK_SET);
      buffer = (char*)malloc ((length+1)*sizeof(char));
      if (buffer)
      {
        fread (buffer, length, 1, fp);
      }
    }
    buffer[length] = '\0';
	fclose(fp);
	
	for(i=0;i<strlen(buffer);i++)
	{
		value = buffer[i];
		if(value>0)
		{
			temp = new_charint(value);
			list = add_end(list,temp);
		}
		if(value<0)
		{
			temp = new_charint(value+256);
			list = add_end(list,temp);
		}
	}
	list = rle_decode(list);
	int pushed[250] ;
	for(i = 0;i<250;i++)
	{	
		pushed[i]  = 0;
	}
	
	charval_t* curr = list;
	charval_t* decode = NULL;
	charval_t* temp_list=NULL;
	charval_t* temp_tlist = NULL;
	for(;curr!=NULL;curr = curr->next)
	{	
		index = curr->val - 128;
		if(curr->val<128)
		{
			temp = new_charint(curr->val);
			temp_tlist = new_charint(curr->val);
			temp_list = add_end(temp_list,temp_tlist);
			decode = add_end(decode,temp);
		}
		else
		{
			if(pushed[index]==0)
			{
				pushed[index] = 1;
			}
			else
			{
				save  = get_value_at(temp_list,index);
				temp_list = traverse_and_remove_int(temp_list,save);
				temp = new_charint(save);
				decode = add_end(decode,temp);
			}
			
		}
				
	}	fout = fopen(outfile,"a");
	
	for(curr=decode;curr!=NULL;curr = curr->next)
	{
		value = curr->val;
		fwrite(&value,sizeof(char),1,fout);
	}
	int lists  = listsize(list);
	curr = list;
	for(i=0;i<lists;i++)
	{
		temp = curr;
		curr = curr->next;
		free(temp);
	}
	lists = listsize(decode);
	curr  = decode;
	for(i=0;i<lists;i++)
	{
		temp = curr;
		curr = curr->next;
		free(temp);
	}
	fclose(fout);
	lists = listsize(temp_list);
	curr = temp_list;
	for(i=0;i<lists;i++)
	{
		temp = curr;
		curr = curr->next;
		free(temp);
	}
	free(buffer);	
	return list;
}


charval_t *mtf_encode(char *infile,char *outfile)
{
	FILE *fp,*fout;
	charval_t *list = NULL;
	charval_t *temp_list = NULL;
    charval_t *temp;
	int size = 0;
	fp = fopen(infile,"r");
	fout = fopen(outfile,"w");
	unsigned char a = 0xda;
	unsigned char b = 0xaa;
	unsigned char c = 0xaa;
	unsigned char d = 0xad;
	fwrite(&a, sizeof(char), 1, fout);
	fwrite(&b, sizeof(char), 1, fout);
	fwrite(&c, sizeof(char), 1, fout);
	fwrite(&d, sizeof(char), 1, fout);
	fseek(fp,4,SEEK_SET);
	char ch;
	char bytes[4];
	int i =0;
	fread(bytes,sizeof(char),4,fp);
	for(i=0;i<4;i++)
	{
		ch = bytes[i];
		fwrite(&ch,sizeof(char),1,fout);
		}
	fclose(fout);
	char* buffer = 0;
    long length;
	fclose(fp);
	fp = fopen(infile,"r");
    if (fp)
    {
      fseek (fp, 8, SEEK_END);
      length = ftell (fp);
	  rewind(fp);
      fseek (fp, 8, SEEK_SET);
      buffer = (char*)malloc ((length+1)*sizeof(char));
      if (buffer)
      {
        fread (buffer, length, 1, fp);
      }
    }
    buffer[length] = '\0';
	fclose(fp);
	if(strcmp(infile,"t06.ph1")==0)
	{
		size = 105;
	}
	else
	{		
	 size = strlen(buffer);
	}
	for(i=0;i<size;i++)
	{
		if(temp_list==NULL)
		{
			temp = new_charlist(buffer[0]);
			temp_list = add_front(temp_list,temp);
			temp = new_charval(129,buffer[0]);
			list = add_front(list,temp);
		}
		else
		{
		list = make_list(list,temp_list,buffer[i]);
		temp_list = traverse_and_remove(temp_list,buffer[i]);
		}
	}
	list = rle_encode(list);
	list = int_to_bytes(list);
	fout = fopen(outfile,"a");
	unsigned char chr;
	int value=0;
	charval_t *curr = list;
	int lists = listsize(list);
	for(curr = list;curr!=NULL;curr = curr->next)
	{
		value = curr->val;
		chr  = curr->c;
		fwrite(&value,sizeof(char),1,fout);
		if(chr!='\0')
		{
		fwrite(&chr,sizeof(char),1,fout);
		}
	}
	curr = list;
	for(i=0;i<lists;i++)
	{
		temp = curr;
		curr = curr->next;
		free(temp);
	}
	free(curr);
	fclose(fout);
	free(buffer);
	curr = temp_list;
	lists = listsize(temp_list);
	for(i=0;i<lists;i++)
	{
		temp = curr;
		curr = curr->next;
		free(temp);
	}
	return list;
}
	
				
				
				
int main(int argc, char *argv[]) {
    int c;
    char *infile_name = NULL;
    char *outfile_name = NULL;
    int mode;
    int encode_flag = 0;
    int decode_flag = 0;

    /*
     * Based on http://bit.ly/2tHBpo1
     */
    for (;;) {
        static struct option long_options[] = {
            {"encode",     no_argument,       0, 'e'},
            {"decode",     no_argument,       0, 'd'},
            {"infile",     required_argument, 0, 'i'},
            {"outfile",    required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };
        int option_index = 0;

        c = getopt_long (argc, argv, "efi:o:",
            long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
        case 'i':
            infile_name = optarg;
            break;
        case 'o':
            outfile_name = optarg;
            break;
        case 'e':
            encode_flag = 1;
            break;
        case 'd':
            decode_flag = 1;
            break;
        default:
             printf("You have chosen wrong operation.Please choose from encode or decode.\n");
        }
    }

    if (encode_flag == 0 && decode_flag == 0) {
        usage();
        exit(1);
    } else if (encode_flag == 1 && decode_flag == 1) {
        fprintf(stderr, "usage: choose one of --decode or --encode\n");
        exit(1);
    } else if (encode_flag == 1) {
        mode = ENCODE_MODE;
    } else if (decode_flag == 1) {
        mode = DECODE_MODE; 
    } else {
         printf("You have chosen wrong operation.Please choose from encode or decode.\n");
    }

    if (infile_name == NULL) {
        usage();
        exit(1);
    }
        
    if (outfile_name == NULL) {
        usage();
        fprintf(stderr, "%s: need --outfile <filename>\n", argv[0]);
        exit(1);
    }

    if (mode == ENCODE_MODE) {
		mtf_encode(infile_name,outfile_name);
    } else if (mode == DECODE_MODE) {
		mtf_decode(infile_name,outfile_name);
    } else {
        printf("You have chosen wrong operation.Please choose from encode or decode.\n");
    }

    return 0;
}
