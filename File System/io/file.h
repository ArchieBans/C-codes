#ifndef FILE_H
#define FILE_H

#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <string.h>

#define BLOCK_SIZE 512
#define Group_Start 6144
#define Inode_Start 1024
#define SuperBlock_Start 0
#define Free_Block_Vector_Start 512
#define Directory_Start 4096
#define DataBlock_Start 15872

#define Inode_Checkpoint_Start 8192
#define Directory_Checkpoint_Start 11264
#define Group_Checkpoint_Start 13312
#define FBV_Checpoint 15360



//Function declarations;
void refresh_current_directory(char *directory);

void InitLLFS();

int calculateHowMuchToAdd(int bitpos);


void setFreeBlockBitInUse(int bitVal);

void setUsedBlockBitFree(int bitVal);

int findFreeBitInReadByte(int valread);

int printBitInReadByte(int valread);
int findFreeBlock();

void initializeFreeBlockVector();

void readSuperBlock();

short read_given_file_name(char *filename);

void readGroup(int groupnum);

void updateSuperBlock();

void printDiskStats();

void struct_check_for_inodes();

void print_all_inodes();

void print_freeBlockVector();

void checkgroups();

void checkDirectory();

void makeGroupsForInodes();

void initializeDirectoryForRoot();

short checkWhichInodeToUse();

short checkWhichDirectoryEntryToUse();

void remakeFileThroughInode(short inode_num,char *target);

short checkParentId(short inode_id);

short check_if_directoryExists(char *filename);

void find_mentioned_directory(char *file_path);

void set_currFileName(char *file_path);

void initalizeRootInode_Struct();

void initializeAllInodes_Struct();


void file_system_check();

void makeCheckPoints();


void refresh_current_directory(char *directory);

void go_one_directory_up();

void write_files_beta(char *given_file_name);


void write_files_alpha(char *given_file_name);
void make_new_directory(char *directory_name);

void remove_directory(char *directory_name);

void remove_file(char *directory_name);

void save_existing_file(char *given_file_name);

void readFile(char *filename,char *target);

void printCWD();

void robustTest(char *given_file_name);

void initializeDisk();







#endif
