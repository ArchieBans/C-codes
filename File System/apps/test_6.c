#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>
#include <sys/types.h>

#include "../io/file.h"
FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);

//This test is to show that system is robust in the disk works. 
// Goal  3 for the assignment.

int main()
{
    FILE *fp = fopen("../disk/vdisk","r");
    
    if(fp==NULL)
    {
        printf("No disk found. Initializing the disk.\n");
        initializeDisk();
    }
    printf("System robustness test is done with a special function, that is on purpose going to exit the program.\n");
    printf("As said in the readme file if the system is exit in the middle of the process the gsync is not done.\n ");
    printf("Now we will write the file with special function.\n");
    printf("Please also note that this file_system_check is run everytime when a test runs.We are running now to show that nothing is wrong with the system.\n");
    file_system_check();
    
    printf("Invoking the bad function.\n");
    robustTest("test_bad.txt");
    file_system_check();
    printf("Hence the system is robust in terms of crashes.\n");
    fclose(fp);
    return 0;
}

