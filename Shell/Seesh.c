
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>
#include <sys/types.h>



int putenv(char *string);
int setenv(const char *name,const char *value, int overwrite);
void unsetenv(const char* name);
char *strdup(const char *s);

#define PRMTSIZ 512
#define MAXARGS 63
#define HISTORYCOUNT 50

int history_print(char *hist[], int current)
{
        int i = current;
        printf("Commands executed are : \n");
		do {
                
				if (hist[i]) {
                        printf("->%s\n",hist[i]);                        
                }

                i = (i + 1) % HISTORYCOUNT;

        } while (i != current);

        return 0;
}
void execute_read_file()
{
	char read_value[1000]={0x0};
	FILE *fptr;
    char *homedir = getenv("HOME");
    if(homedir!=NULL)
    {
        strcat(homedir,"/.SEEshrc");
        printf("%s\n",homedir);
    }
    if ((fptr = fopen(homedir, "r")) == NULL) {
        printf("Error! opening file. Please check where .SEEshrc file is located. The program is set up for looking in the home directory.");
    }
    else
    {
          //reads text until newline is encountered
    while(fgets(read_value,100,(FILE*)fptr)!=NULL)
	{
		char *pointer_head = read_value;
		char *command_values[MAXARGS+1]={NULL};	
		printf("%s",read_value);
        for(int i=0;i<sizeof(command_values) && *pointer_head;pointer_head++)
		{
		if(*pointer_head==' ')continue;
		if(*pointer_head=='\n')break;
		for(command_values[i++] = pointer_head;*pointer_head&&*pointer_head!=' '&& *pointer_head!='\n';pointer_head++){}		
			*pointer_head = '\0';
		}
	 if(fork()==0) 
		{
		signal(SIGINT, SIG_DFL);
		exit(execvp(command_values[0],command_values));
		wait(NULL);
		}
	
	read_value[0] = '\0';	
	}	 	
	fclose(fptr);
  
    }
}
int main(int argc, char *argv[], char * envp[])
{
	

	
	execute_read_file();
	 
	char *history[HISTORYCOUNT];
	int his_count = 0;
	for(int i=0;i<HISTORYCOUNT;i++)
	{
		history[i] = NULL;
	}
	while(1)
	{
	
		signal(SIGINT, SIG_IGN);
		char input[PRMTSIZ+1] = {0x0};		
		char *pointer = input;
		char *entered_arguments[MAXARGS+1] = {NULL};
		
		//CD variables.
		char *directory;
		char *gedir;
		char *to_directory;
		char *home_directory = getenv("HOME");
		char  buffer[PRMTSIZ];
		
		
		//pwd variables
		char current_working_directory[PRMTSIZ];
		printf("\n? ");
		
		if(fgets(input,PRMTSIZ,stdin)==NULL)
		{
			printf("\n");
            exit(1);
		}
		
		if (input[strlen(input) - 1] == '\n'){
			input[strlen(input) - 1] = '\0';		
		}
        history[his_count] = strdup(input);
		his_count = (his_count+1) % HISTORYCOUNT;
		
	for(int i=0;i<sizeof(entered_arguments) && *pointer;pointer++)
	{
		if(*pointer==' ')continue;
		if(*pointer=='\n')break;
		for(entered_arguments[i++] = pointer;*pointer&&*pointer!=' '&& *pointer!='\n';pointer++){}		
			*pointer = '\0';
	}
	if(entered_arguments[0]==NULL)
	{
		continue;
	}
	if(strchr(entered_arguments[0],'!')!=NULL)
	{
		
		printf("Feature not implemented.\n");
		continue;		
			
	}
	if(!strcmp(entered_arguments[0], "history"))
	{
		history_print(history,his_count);
		continue;
	}
	if (!strcmp(entered_arguments[0], "cd")){

            gedir = getcwd(buffer, sizeof(buffer));
            directory = strcat(gedir, "/");
			if(entered_arguments[1]!=NULL)
			{
					to_directory = strcat(directory, entered_arguments[1]);
			}
            else
			{
				to_directory = home_directory;
			}
            chdir(to_directory);
			continue;
        }            
	if(!strcmp(entered_arguments[0],"pwd"))
	{
		getcwd(current_working_directory,sizeof(current_working_directory));
		printf("Current working directory: %s\n",current_working_directory);
		continue;
	}
	
	if(!strcmp(entered_arguments[0],"help"))
	{
		printf("cd -> Change current working directory.\n");
		printf("pwd -> Prints current working directory.\n");
		printf("help -> Displays string bindings.\n");
		printf("set var [value] -> Set enviornment variables.\n");
		printf("unset var -> Removes the pointer address associated with mentioned var.\n");
		printf("checkvar var -> Returns the value set for the mentioned enviornment variable.\n");
		printf("history -> Returns the list of all the commands executed till now.\n");
		printf("exit or ctrl-d -> Quit the shell.");
		continue;
	}
	if(!strcmp(entered_arguments[0],"set") && entered_arguments[1]==NULL)
	{
		int i; 
		for (i = 0; envp[i] != NULL; i++) 
		{
			printf("\n%s", envp[i]); 	
			continue;	
		}    
	}
	if(!strcmp(entered_arguments[0],"set") && entered_arguments[1]!=NULL)
	{
			if(entered_arguments[1]!=NULL)
			{
				char *token = strtok(entered_arguments[1]," ");
				char *prev = token;
			
				setenv(prev,entered_arguments[2],1);
				
				continue;
				
			}

	}
	if(!strcmp(entered_arguments[0],"exit"))
	{
		exit(1);
	}
	if(!strcmp(entered_arguments[0],"checkvar"))
	{
		char* pPath;
		if(entered_arguments[1]!=NULL)
		{
		pPath = getenv(entered_arguments[1]);
		printf("\n Value for enviornment variable %s = %s",entered_arguments[1],pPath);		
		}
		else
		{
			printf("Please input a variable for query.\n");
		}
		continue;
				
	}
	if(!strcmp(entered_arguments[0],"unset"))
	{
		
		if(entered_arguments[1]!=NULL)
		{
		unsetenv(entered_arguments[1]);
		printf("\n Value for enviornment variable %s  has been deleted.",entered_arguments[1]);		
		}
		else
		{
			printf("Please input a variable for query.\n");
		}
		continue;
				
	}
	
	// if(fork()==0) {
		// signal(SIGINT, SIG_DFL);
		// exit(execvp(entered_arguments[0],entered_arguments));
		// wait(NULL);
		// }
		pid_t process_id;
		int status;

		process_id = fork();
			if (process_id == 0) {
			// Child process
                signal(SIGINT, SIG_DFL);
				if (execvp(entered_arguments[0], entered_arguments) == -1) {
					perror("Error :");
				}
			exit(EXIT_FAILURE);
			}	 
			else if (process_id < 0) {
				// Error forking
				perror("Error :");
				} 
			else {
			// Parent process
			do {
				 waitpid(process_id, &status, WUNTRACED);
				} 
			while (!WIFEXITED(status) && !WIFSIGNALED(status));
			}
		
	
	}
	return 0;
}	