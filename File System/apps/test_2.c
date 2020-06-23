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
//This test is to show that that reading and writing to root directory disk works. 
// Goal  2.1 for the assignment.

int main()
{
    char *diff = "diff -w new_file.txt test_512.txt";
    FILE *fp = fopen("../disk/vdisk","r");
    if(fp==NULL)
    {
        printf("No disk found. Initializing the disk.\n");
        initializeDisk();
    }
    file_system_check();
    printCWD();
    char *file_path = "../apps/test_512.txt";
    char *filename = "test_512.txt";
    printf("Writing the %s in the root directory.\n",filename);
    printf("Command executed diff new_file.txt test_512.txt\n");
    
    printf("Performing Diff before to show that no file name new_file.txt exists.\n");
    FILE *fp_n = popen(diff,"w");
    pclose(fp_n);
    
    write_files_beta(file_path);
    printf("Performing a read from the disk and making a new_file.txt with exact values as test_512.txt.\n");
    readFile(filename,"../apps/new_file.txt");
    printf("Now there is a new file in the folder called new_file.txt. You can perform this command '%s'to see that they are identical.\n",diff);
    fclose(fp);
    return 0;
}

