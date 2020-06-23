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

//This test is to show that making a new directory in the disk works. 
// Goal  2.2 for the assignment.

int main()
{
    FILE *fp = fopen("../disk/vdisk","r");
    
    if(fp==NULL)
    {
        printf("No disk found. Initializing the disk.\n");
        initializeDisk();
    }
    file_system_check();
    printCWD();
    printf("First we will try to go the directory named test_direc.\n");
    refresh_current_directory("test_direc");
    printf("Making new directory -  test_direc.\n");
    make_new_directory("test_direc");
    printf("Now the directory is made we will try to go again.\n");
    refresh_current_directory("test_direc");
    printCWD();
    // print_all_inodes();
    // checkDirectory();
    fclose(fp);
    return 0;
}

