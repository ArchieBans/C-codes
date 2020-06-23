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

//This test is to show that making a new deletion of file and directory in the disk works. 
// Goal  2.4 for the assignment.

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
    printf("First we will try to go the directory named test_direc_2.\n");
    refresh_current_directory("test_direc_2");
    printf("Making new directory -  test_direc_2.\n");
    make_new_directory("test_direc_2");
    
    printf("Now the directory is made we will try to go again.\n");
    refresh_current_directory("test_direc_2");
    printCWD();
    
    printf("Now we will go one directory up.\n");
    go_one_directory_up();
    printCWD();
    printf("Now we will remove the newly made directory - test_direc_2.\n");
    remove_directory("test_direc_2");
    printf("Trying to go to the directory again.\n");
    refresh_current_directory("test_direc_2");
    printf("Hence directory got removed successfully.\n");
    
    printf("Now we are writing a file to root directory named test_3072.txt.\n");
    write_files_beta("test_3072.txt");
    printf("Checking if the file exists now.\n");
    find_mentioned_directory("/root/test_3072.txt");
    
    printf("Now removing the file from the root directory.\n");
    remove_file("test_3072.txt");
    
    printf("Checking if the file exists now.\n");
    find_mentioned_directory("/root/test_3072.txt");
    printf("Hence the file got removed as well.\n");
    fclose(fp);
    return 0;
}

