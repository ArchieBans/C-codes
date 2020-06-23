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

//This test is to show that making a new file in sub directory in the disk works. 
// Goal  2.3 for the assignment.

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
    printf("First we will try to go the directory named test_direc_1.\n");
    refresh_current_directory("test_direc_1");
    printf("Making new directory -  test_direc_1.\n");
    make_new_directory("test_direc_1");
    
    printf("Now the directory is made we will try to go again.\n");
    refresh_current_directory("test_direc_1");
    printCWD();
    
    printf("Now we will write the file test_1026.txt to the test_direc\n");
    
    write_files_beta("test_1026.txt");
    go_one_directory_up();
    printCWD();
    printf("Going back to root directory and trying to find the file.\n");
    
    readFile("test_1026.txt","../apps/new_file_2.txt");
    printf("Now we will change the current directory to test_direc where we wrote the file.\n");
    refresh_current_directory("test_direc_1");
    readFile("test_1026.txt","../apps/new_file_2.txt");
    printf("File found and written as a new_file_2.txt. You can use this 'diff -w new_file_2.txt test_1026.txt' to test the validity.\n");
    // print_all_inodes();
    // checkDirectory();
    fclose(fp);
    return 0;
}

