#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <string.h>
#include "file.h"

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

char curr_file_name[30];
int g_num_blocks;
int g_num_inodes;
int g_used_blocks;
int g_free_blocks;
int g_free_inodes;
int g_num_direc;
int g_num_groups;
short g_group_id;
short g_group_nums[10];
char current_directory[30] = "root";
short inode_current = 1;
int g_sync=0;
int directory_read_given_file_name = 0;
//////////////////////////////////////All the Structs are here. ///////////////////////////////////////////////
struct inodeBlock
{
    short inode_num;
    short is_used;
    short type_of;
    time_t created;
    time_t last_modified;
    int size_of_file;
    short group_pointer;
    short indirect_pointer;
    short directory_inode;
}__attribute__((packed));

struct directory_block
{
    short inode_id;
    char filename[30];
}__attribute__((packed));


struct group_block
{
    short g_id;
    short g_nums[10];
}__attribute__((packed));


struct SuperBlock
{
    unsigned int MagicNumber;
    int numBlocks;
    int numInodes;
    int usedBlocks;
    int numDirectory;
    int freeBlocks;
    int freeInodes;
    int initialValues;
    int isSync;
    int end_super_block;
    int numGroups;
    char unusedBuffer[468];
}__attribute__((packed));

void InitLLFS()
{
    int num_bytes = 2*(1024*1024-1);
    struct SuperBlock sblock;
    sblock.MagicNumber = 0xd0410c0f;
    sblock.numBlocks = 4096;
    sblock.numInodes = 96;
    sblock.usedBlocks = 31;
    sblock.numDirectory = 64;
    sblock.freeBlocks = 4065;
    sblock.freeInodes = 95;
    sblock.end_super_block = 2098;
    sblock.isSync = 0;
    sblock.unusedBuffer[0] = '\0';
    sblock.numGroups = 96;
    int initial_values = 0;
    FILE *fp = fopen("../disk/vdisk","w");
    fseek(fp,num_bytes,SEEK_SET);
    fwrite(&initial_values,512,1,fp);
    fseek(fp,0,SEEK_SET);
    fwrite(&sblock,sizeof(sblock),1,fp);
    fclose(fp);
    
}




//////////////////////////////////////Bit Operations. ///////////////////////////////////////////////


int calculateHowMuchToAdd(int bitpos)
{
    if(bitpos==0)
    {
        return 128;
    }
    if(bitpos==1)
    {
        return 64;
    }
    if(bitpos==2)
    {
        return 32;
    }
    if(bitpos==3)
    {
        return 16;
    }
    if(bitpos==4)
    {
        return 8;
    }
    if(bitpos==5)
    {
        return 4;
    }
    if(bitpos==6)
    {
        return 2;
    }
    if(bitpos==7)
    {
        return 1;
    }
    return 0;
}
void setFreeBlockBitInUse(int bitVal)
{
    FILE *fp = fopen("../disk/vdisk","r+");
    int getRow = ceil(bitVal / 8); //Each row is a byte.
    int getBitPosition = bitVal % 8;
    fseek(fp,512+getRow,SEEK_SET); //Set cursor to the row postion.
    int valread;
    fread(&valread,1,1,fp);
    fseek(fp,-1,SEEK_CUR); //Set cursor back to the row after the read.
    int add_val = calculateHowMuchToAdd(getBitPosition);
    unsigned char new_val = valread+add_val;
    fwrite(&new_val,1,1,fp);
    fclose(fp);
}
void setUsedBlockBitFree(int bitVal)
{
    FILE *fp = fopen("../disk/vdisk","r+");
    int getRow = ceil(bitVal/8);
    int getBitPosition = bitVal % 8;
    fseek(fp,512+getRow,SEEK_SET);
    int valread;
    fread(&valread,1,1,fp);
    fseek(fp,-1,SEEK_CUR);
    int negate_val = calculateHowMuchToAdd(getBitPosition);
    unsigned char new_val = valread - negate_val;
    fwrite(&new_val,1,1,fp);
    fclose(fp);
}
int findFreeBitInReadByte(int valread)
{
    
    int input, index;
    char inputchar;
    unsigned int mask;
    int bits[CHAR_BIT];
    input = valread;

    /* Check for errors. In C, we must always check for errors */
    if (input == EOF) {
        printf("No character read\n");
        exit(1);
    }

    inputchar = input; //Make an int to a char.

    mask = 1 << (CHAR_BIT - 1); //Mask for reading in the bits of a char.

    for (index = 0; index < CHAR_BIT; ++index) { //Loop for most significant to least significant
        
        bits[index] = (inputchar & mask) ? 1 : 0;
        mask = mask >> 1; //Masking reads the bits one by one.
    }

    for (index = 0; index < CHAR_BIT; ++index) {
        
       // printf("%d", bits[index]);
        if(bits[index]==0)//Check for a free bit.
        {
            return index;
        }
    }
    
    return -1;
}
int printBitInReadByte(int valread)
{
    
    int input, index;
    char inputchar;
    unsigned int mask;
    int bits[CHAR_BIT];
    input = valread;

    /* Check for errors. In C, we must always check for errors */
    if (input == EOF) {
        printf("No character read\n");
        exit(1);
    }

    inputchar = input; //Make an int to a char.

    mask = 1 << (CHAR_BIT - 1); //Mask for reading in the bits of a char.

    for (index = 0; index < CHAR_BIT; ++index) { //Loop for most significant to least significant
        
        bits[index] = (inputchar & mask) ? 1 : 0;
        mask = mask >> 1; //Masking reads the bits one by one.
    }

    for (index = 0; index < CHAR_BIT; ++index) {
        
       printf("%d", bits[index]);
    }
    
    return 0;
}
int findFreeBlock()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,512,SEEK_SET);
    for(int i=0;i<512;i++)
    {
        int valread;
        fread(&valread,1,1,fp);
        int free_bit_index = findFreeBitInReadByte(valread);
        if(free_bit_index!=-1)
        {
            return (i*8)+free_bit_index;
        }
    }
    printf("No free block has been found. Please free some memory by deleting stuff.\n");
    fclose(fp);
    return 0;
}
void initializeFreeBlockVector()
{
    FILE *fp = fopen("../disk/vdisk","r+");
    fseek(fp,Free_Block_Vector_Start,SEEK_SET);
    for(int i=0;i<512;i++)
    {
        if(i<4)
        {
        unsigned char value = 255;
        fwrite(&value,sizeof(value),1,fp);   
        }
        else
        {
        unsigned char value = 0;
        fwrite(&value,sizeof(value),1,fp);     
        }
    }
    
    fclose(fp);
}



//////////////////////////////////////All the reads. ///////////////////////////////////////////////




void readSuperBlock()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,0,SEEK_SET);
    struct SuperBlock sblock;
    fread(&sblock,sizeof(sblock),1,fp);
    
    
    g_num_blocks = sblock.numBlocks;
    g_num_inodes = sblock.numInodes;
    g_used_blocks = sblock.usedBlocks;
    g_free_blocks = sblock.freeBlocks;
    g_free_inodes = sblock.freeInodes;
    g_sync = sblock.isSync;
    g_num_direc = sblock.numDirectory;
    g_num_groups = sblock.numGroups;
    // printf("Disk parameters:\n");
    // printf("Magic num - %d\n",sblock.MagicNumber);
    // printf("Num block - %d\n",g_num_blocks);
    // printf("Num inode - %d\n",g_num_inodes);
    // printf("Used blocks - %d\n",g_used_blocks);
    // printf("Free block - %d\n",g_free_blocks);
    // printf("Free inode - %d\n",g_free_inodes);
    // printf("Number directory - %d\n",g_num_direc);
    // printf("Num groups - %d\n",g_num_groups);
    fclose(fp);
}

short read_given_file_name(char *filename)
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Directory_Start,SEEK_SET);
    readSuperBlock();
    for(short i=0;i<64;i++)
    {
        struct directory_block dblock;
        fread(&dblock,sizeof(dblock),1,fp);
        if(strcmp(filename,dblock.filename)==0)
        {
            if(dblock.inode_id==1)
            {
                return dblock.inode_id;
            }
            else
            {
                fseek(fp,(dblock.inode_id-1)*32+Inode_Start,SEEK_SET);
                struct inodeBlock inode;
                fread(&inode,sizeof(inode),1,fp);
                if(inode.directory_inode==inode_current)
                {
                    //remakeFileThroughInode(dblock.inode_id);
                    directory_read_given_file_name = i;
                    return dblock.inode_id;
                }
                fseek(fp,i*32+Directory_Start,SEEK_SET);
                
            }
        }
    }
    fclose(fp);
    return 0;
}

void readGroup(int groupnum)
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Group_Start+((groupnum-1)*22),SEEK_SET);
    struct group_block gblock;
    fread(&gblock,sizeof(gblock),1,fp);
    g_group_id = gblock.g_id;
    for(int i=0;i<sizeof(gblock.g_nums)/sizeof(gblock.g_nums[0]);i++)
    {
        g_group_nums[i] = gblock.g_nums[i];
    }
    fclose(fp);
}

void updateSuperBlock()
{
    
    FILE *fp = fopen("../disk/vdisk","r+");
    struct SuperBlock sblock;
    sblock.MagicNumber = 0xd0410c0f;
    sblock.numBlocks = 4096;
    sblock.numInodes = 96;
    sblock.usedBlocks = g_used_blocks;
    sblock.numDirectory = 64;
    sblock.freeBlocks = g_free_blocks;
    sblock.freeInodes = g_free_inodes;
    sblock.initialValues = 0;
    sblock.isSync = g_sync;
    sblock.end_super_block = 2098;
    sblock.unusedBuffer[0] = '\0';
    sblock.numGroups = g_num_groups;
    
    fseek(fp,0,SEEK_SET);
    fwrite(&sblock,sizeof(sblock),1,fp);
    fclose(fp);
}




//////////////////////////////////////////////All the prints./////////////////////////////////////////////

void printDiskStats()
{
    printf("First 31 blocks are not for data usage.\n");
    printf("After super block and free block vector next 6 are inodes.Giving 96 inodes 32 bytes each.\n");
    printf("Next 4 blocks are for directories.Giving 64 directory entries.\n");
    printf("Next 4 are groups for the inodes. Please read more about this in the readme file.");
    printf("Next 6 are for checkpoint Inodes.\n");
    printf("Next 4 are directory checkpoints.\n");
    printf("Next 4 are groups checkpoint.\n");
    printf("Last one is free block vector checkpoints.\n");
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,1030,SEEK_SET);
    time_t read_date;
    fread(&read_date,8,1,fp);
    printf("Root inode last modified : %s\n", ctime(&read_date));
    fclose(fp);
}

void struct_check_for_inodes()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,1024,SEEK_SET);
    readSuperBlock();
    for(short i=0;i<5;i++)
    {
        struct inodeBlock iblock;
        fread(&iblock,sizeof(iblock),1,fp);
        printf("Inode Id - %d\n",iblock.inode_num);
        
    }
    
}

void print_all_inodes()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Inode_Start,SEEK_SET);
    readSuperBlock();
    printf("G - num - inode - %d\n",g_num_inodes);
    for(int i=0;i<g_num_inodes;i++)
    {
        struct inodeBlock inode;
        fread(&inode,sizeof(inode),1,fp);
        printf("Inode num - %d, Inode is used - %d, Parent directory - %d\n",inode.inode_num,inode.is_used,inode.directory_inode);
    }
}

void print_freeBlockVector()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Free_Block_Vector_Start,SEEK_SET);
    for(int i=0;i<20;i++)
    {
        int val;
        fread(&val,1,1,fp);
        if(i>=0)
        {
         printBitInReadByte(val);
           
        }
        // printf("value - %d\n",val);    
    }
    printf("\n");
    fclose(fp);
}

void checkgroups()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Group_Start,SEEK_SET);
    readSuperBlock();
    for(short i=0;i<96;i++)
    {
        struct group_block group;
        fread(&group,sizeof(group),1,fp);
        printf("group id -> %d\n",group.g_id);
        printf("group val -> %d\n",group.g_nums[0]);
    }
    
}
void checkDirectory()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Directory_Start,SEEK_SET);
    for(short i=0;i<64;i++)
    {
        struct directory_block directory;
        fread(&directory,sizeof(directory),1,fp);
        printf("Inode  - %d\n",directory.inode_id);
        printf("File name - %s\n",directory.filename);
    }
}



/////////////////////////////////////////////Utitlity Functions.///////////////////////////////////////////////

void makeGroupsForInodes()
{
    FILE *fp = fopen("../disk/vdisk","r+");
    fseek(fp,Group_Start,SEEK_SET); //Skip first 12 blocks.
    for(short i=1;i<97;i++)
    {
        struct group_block groups;
        groups.g_id = i;
        fwrite(&groups,sizeof(groups),1,fp);
        for(int i=0;i<10;i++)
        {
            groups.g_nums[i] = 0;
        }
        //fwrite(&i,2,1,fp);
        //fseek(fp,20,SEEK_CUR);
    }
    fclose(fp);
}
void initializeDirectoryForRoot()
{
    FILE *fp = fopen("../disk/vdisk","r+");
    fseek(fp,Directory_Start,SEEK_SET);
    struct directory_block dblock;
    dblock.inode_id = 1;
    strcpy(dblock.filename,"root");
    fwrite(&dblock,sizeof(dblock),1,fp);
    fclose(fp);
}

short checkWhichInodeToUse()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Inode_Start+32,SEEK_SET);
    readSuperBlock();
    if(g_free_inodes==0)
    {
       printf("There are no more inodes in the system. Please delete something to free up inodes.\n");
       exit(1);
    }
    //short what_seek = 1056;
    
    for(int i=2;i<g_num_inodes+1;i++)
    {
        struct inodeBlock inode;
        fread(&inode,sizeof(inode),1,fp);
        if(inode.is_used==0)
        {
            return inode.inode_num;
        }
    }
    fclose(fp);
    return 0;
}
short checkWhichDirectoryEntryToUse()
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Directory_Start,SEEK_SET); //Set head for finding a free directory
    
    for(short i=1;i<65;i++)
    {
        struct directory_block dblock;
        fread(&dblock,sizeof(dblock),1,fp);
        if(dblock.inode_id==0)
        {
            return i;
        }
        else 
        {
            //printf("Inode Id - %d,Filename - %s\n",dblock.inode_id,dblock.filename);
        }
    }
    return 0;
}

void remakeFileThroughInode(short inode_num,char *target)
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Inode_Start,SEEK_SET); //Go to inode head.
    readSuperBlock();
    FILE *fp_new = fopen(target,"w");
    for(short i=0;i<g_num_inodes;i++)
    {
        //short inode_val;
        struct inodeBlock iblock;
        fread(&iblock,sizeof(iblock),1,fp);
        
        if(inode_num==iblock.inode_num)
        {
            
            readGroup(iblock.group_pointer);
            printf("Group pointer %d\n",iblock.group_pointer);
            for(int i=0;i<sizeof(g_group_nums)/sizeof(g_group_nums[0]);i++)
            {
                
                if(g_group_nums[i]!=0)
                {
                //printf("DBlocks - %d\n",g_group_nums[i]);
                fseek(fp,0,SEEK_SET);
                int datablock = g_group_nums[i];
                fseek(fp,(datablock*512)+DataBlock_Start,SEEK_SET);
                char bufferRead[512];
                fread(&bufferRead,512,1,fp);
                for(int i=0;i<sizeof(bufferRead)/sizeof(bufferRead[0]);i++)
                {
                    if(bufferRead[i]!=0)
                    {
                        fwrite(&bufferRead[i],1,1,fp_new); 
                    }   
                }
                //printf("%s\n",bufferRead);
                //fwrite(&bufferRead,sizeof(bufferRead),1,fp_new);  
                }
            
            }
           
        }        
    }
    
    fclose(fp);
}

short checkParentId(short inode_id)
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Inode_Start,SEEK_SET);
    readSuperBlock();
    
    for(short i=0;i<g_num_inodes;i++)
    {
        struct inodeBlock inode;
        fread(&inode,sizeof(inode),1,fp);
        if(inode.inode_num==inode_id)
        {
            return inode.directory_inode;
        }
    }
    fclose(fp);
    return 0;
    
}
short check_if_directoryExists(char *filename)
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Directory_Start,SEEK_SET);
    
    for(short i=0;i<64;i++)
    {
        struct directory_block directory;
        fread(&directory,sizeof(directory),1,fp);
        if(strcmp(directory.filename,filename)==0)
        {
            short current_inode_id = read_given_file_name(current_directory);
            short directory_inode = checkParentId(directory.inode_id);
            if(directory_inode == current_inode_id)
            {
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;    
}

void find_mentioned_directory(char *file_path)
{
    char file_position[160];
    strcpy(file_position,file_path);
    char *token = strtok(file_position,"/");
    while(token!=NULL)
    {
        
        int inode_found = read_given_file_name(token);
        if(inode_found==0)
        {
            printf("File or directory - %s not found in the system.\n",token);
            return;
        }
        else if(inode_found==1)
        {
            printf("Directory - %s, At Inode - %d\n",token,inode_found);
        }
        else
        {
            FILE *fp = fopen("../disk/vdisk","r");
            fseek(fp,(inode_found-1)*32+Inode_Start,SEEK_SET);
            struct inodeBlock inode;
            fread(&inode,sizeof(inode),1,fp);
            if(inode.directory_inode!=inode_current)
            {
                printf("File or directory - %s not found in the system.\n",token);
                return;
            }
            else
            {
                if(inode.type_of==1){refresh_current_directory(token);printf("Current directory - %s,Current inode - %d\n",current_directory,inode_current);}
                printf("Directory - %s, At Inode - %d\n",token,inode_found);         
            }
            fclose(fp);
        }
        token = strtok(NULL,"/");
    }
}

void set_currFileName(char *file_path)
{
    char file_position[160];
    char save_token[30];
    strcpy(file_position,file_path);
    char *token = strtok(file_position,"/");
    while(token!=NULL)
    { 
        strcpy(save_token,token);
        
        token = strtok(NULL,"/");
    }
    strcpy(curr_file_name,save_token);
}
///////////////////////////////////////////Initialization Functions./////////////////////////////////////////////
void initalizeRootInode_Struct()
{
    struct inodeBlock inode;
    struct group_block gblock;
    struct directory_block dblock;
    printf("Size of %ld\n",sizeof(inode));
    printf("Size of %ld\n",sizeof(gblock));
    printf("Size of %ld\n",sizeof(dblock));
}

void initializeAllInodes_Struct()
{
    readSuperBlock();
    FILE *fp = fopen("../disk/vdisk","r+");
    fseek(fp,Inode_Start,SEEK_SET);
    for(short i=1;i<g_num_inodes+1;i++)
    {
        struct inodeBlock inode;
        inode.inode_num = i;
        inode.is_used = 0;
        if(i==1)
        {
         inode.type_of = 1;
         inode.is_used = 1;   
        }
        else
        {
            inode.type_of = 0;
        }
        time_t created;
        time(&created);
        inode.created = created;    
        time_t last_modified;//4 bytes
        time(&last_modified);
        inode.last_modified = last_modified;
        inode.size_of_file = 0;
        inode.group_pointer = i;
        inode.indirect_pointer = 0;
        inode.directory_inode=0;
        fwrite(&inode,sizeof(inode),1,fp);
    }
}

/////////////////////////////////////System Robustness.//////////////////////////////////////////////////

void file_system_check()
{
    readSuperBlock();
    
    if(g_sync==1)
    {
        printf("Disk was not written properly in the last write. Restoring the data to last known checkpoint.\n");
        FILE *fp = fopen("../disk/vdisk","r+");
        readSuperBlock();
        
        
        struct inodeBlock inodes[g_num_inodes];
        printf("Restoring inodes.\n");
        fseek(fp,Inode_Checkpoint_Start,SEEK_SET);
        printf("Reading inode checkpoints.\n");
        for(int i=0;i<g_num_inodes;i++)
        {
            fread(&inodes[i],sizeof(inodes[i]),1,fp);
        }
        printf("Done.\n");
        fseek(fp,Inode_Start,SEEK_SET);
        for(int i=0;i<g_num_inodes;i++)
        {
            fwrite(&inodes[i],sizeof(inodes[i]),1,fp);
        }
        printf("Restoring inodes done.\n");
        
        struct directory_block dblocks[g_num_direc];
        printf("Restoring directory entries.\n");
        fseek(fp,Directory_Checkpoint_Start,SEEK_SET);
        printf("Reading directory checkpoints.\n");
        for(int i=0;i<g_num_direc;i++)
        {
            fread(&dblocks[i],sizeof(dblocks[i]),1,fp);
        }
        printf("Done.\n");
        fseek(fp,Directory_Start,SEEK_SET);
        for(int i=0;i<g_num_direc;i++)
        {
            fwrite(&dblocks[i],sizeof(dblocks[i]),1,fp);
        }
        printf("Resotring directory done.\n");
        
        
        struct group_block gblocks[g_num_blocks];
        printf("Restoring groups\n");
        fseek(fp,Group_Checkpoint_Start,SEEK_SET);
        printf("Reading group blocks checkpoint.\n");
        for(int i=0;i<g_num_blocks;i++)
        {
            fread(&gblocks[i],sizeof(gblocks[i]),1,fp);
        }
        printf("Done.\n");
        fseek(fp,Group_Start,SEEK_SET);
        for(int i=0;i<g_num_groups;i++)
        {
            fwrite(&gblocks[i],sizeof(gblocks[i]),1,fp);
        }
        printf("Resotring groups done.\n");
        
        char freeBlockVector[512];
        printf("Restoring free block vector;\n");
        fseek(fp,FBV_Checpoint,SEEK_SET);
        fread(&freeBlockVector,sizeof(freeBlockVector),1,fp);
        fseek(fp,Free_Block_Vector_Start,SEEK_SET);
        fwrite(&freeBlockVector,sizeof(freeBlockVector),1,fp);
        g_sync = 0;
        updateSuperBlock();
        printf("Restoring complete of the disk. The disk is now ready to use.\n");
        fclose(fp);
    }
}

void makeCheckPoints()
{
    FILE *fp = fopen("../disk/vdisk","r+");
    readSuperBlock();
    
    struct inodeBlock inodes[g_num_inodes];
    fseek(fp,Inode_Start,SEEK_SET);
    for(int i=0;i<g_num_inodes;i++)
    {
        fread(&inodes[i],sizeof(inodes[i]),1,fp);
    }
    fseek(fp,Inode_Checkpoint_Start,SEEK_SET);
    for(int i=0;i<g_num_inodes;i++)
    {
        fwrite(&inodes[i],sizeof(inodes[i]),1,fp);
    }
    
    struct directory_block dblocks[g_num_direc];
    fseek(fp,Directory_Start,SEEK_SET);
    for(int i=0;i<g_num_direc;i++)
    {
        fread(&dblocks[i],sizeof(dblocks[i]),1,fp);
    }
    fseek(fp,Directory_Checkpoint_Start,SEEK_SET);
    for(int i=0;i<g_num_direc;i++)
    {
        fwrite(&dblocks[i],sizeof(dblocks[i]),1,fp);
    }
    
    struct group_block gblocks[g_num_groups];
    fseek(fp,Group_Start,SEEK_SET);
    for(int i=0;i<g_num_groups;i++)
    {
        fread(&gblocks[i],sizeof(gblocks[i]),1,fp);
    }
    fseek(fp,Group_Checkpoint_Start,SEEK_SET);
    for(int i=0;i<g_num_groups;i++)
    {
        fwrite(&gblocks[i],sizeof(gblocks[i]),1,fp);
    }
    
    char freeBlockVector[512];
    fseek(fp,Free_Block_Vector_Start,SEEK_SET);
    fread(&freeBlockVector,sizeof(freeBlockVector),1,fp);
    fseek(fp,FBV_Checpoint,SEEK_SET);
    fwrite(&freeBlockVector,sizeof(freeBlockVector),1,fp);
    
    fclose(fp);
    
}



///////////////////////////////////////////Assignment Requirements.//////////////////////////////////////////////

void refresh_current_directory(char *directory)
{
    FILE *fp = fopen("../disk/vdisk","r");
    fseek(fp,Directory_Start,SEEK_SET);
    if(check_if_directoryExists(directory)==1)
    {
        for(short i=0;i<64;i++)
        {
            struct directory_block dblock;
            fread(&dblock,sizeof(dblock),1,fp);
            if(strcmp(dblock.filename,directory)==0)
            {
                
                short parent_direc = checkParentId(dblock.inode_id);
                if(parent_direc==inode_current)
                {
                    
                    strcpy(current_directory,dblock.filename);
                    inode_current = dblock.inode_id;
                    fclose(fp);
                    return;
                }
            }
        }
    }
    else{
        printf("Requested directory not found. Please check the path.\n");
    }
    fclose(fp);
}

void go_one_directory_up()
{
   FILE *fp = fopen("../disk/vdisk","r");
   fseek(fp,Inode_Start,SEEK_SET);
   readSuperBlock(); 
    if(inode_current==1)
    {
        printf("Already at the root directory.\n");
    }
    else
    {
     
        for(short i=0;i<96;i++)
        {
            struct inodeBlock inode;
            fread(&inode,sizeof(inode),1,fp);
            if(inode.inode_num==inode_current)
            {
                fseek(fp,(inode.directory_inode-1)*32+Inode_Start,SEEK_SET);
                
                struct inodeBlock newnode;
                fread(&newnode,sizeof(newnode),1,fp);
                fseek(fp,Directory_Start,SEEK_SET);
                for(int j=0;j<64;j++)
                {
                        struct directory_block dblock;
                        fread(&dblock,sizeof(dblock),1,fp);
                        if(dblock.inode_id==newnode.inode_num)
                        {
                            
                                strcpy(current_directory,dblock.filename);
                                inode_current = dblock.inode_id;
                                fclose(fp);
                                return;

                        }
                    
                }
                
                
                
                
                }
            }
        
    }
    
    fclose(fp);
}

void write_files_beta(char *given_file_name)
{
    readSuperBlock();
    makeCheckPoints();
    g_sync = 1;
    updateSuperBlock();
    
    if(check_if_directoryExists(given_file_name)==0)
    {
     FILE *fp = fopen("../disk/vdisk","r+");
    FILE *fp_file = fopen(given_file_name,"r");
    set_currFileName(given_file_name);
    //Get the size of the file.
    fseek(fp_file,0L,SEEK_END);
    float file_size = ftell(fp_file);
    int file_size_int = ftell(fp_file);
    fseek(fp_file,0,SEEK_SET);
    fseek(fp,0,SEEK_SET);
    
    //Calculate how many blocks we need to store the file.
    printf("File size - %d\n",file_size_int);
    float test_how_many = (file_size / 512);
    int how_many_blocks = ceil(test_how_many);    
    //printf("\n%d\n",how_many_blocks);
    readSuperBlock();//Update the global variables.
    
    //Choose which inode to use
    short curr_inode = checkWhichInodeToUse();
    if(curr_inode==0)
    {
        printf("Please free some space on the disk to write new files and directories.\n");
        exit(1);
    }
    
    //Prepare the inode to write the file, update all parameters for inode.
    short byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
    fseek(fp,byte_pos,SEEK_SET);
    struct inodeBlock read_inode;
    fread(&read_inode,sizeof(read_inode),1,fp);
    
    
    //Directory entry for the file name.
    char final_name[30];
    strcpy(final_name,curr_file_name);
    short which_directory_to_Write = checkWhichDirectoryEntryToUse();    
    if(which_directory_to_Write==0)
    {
        printf("No free directory found in the system. This design only supports 64 directory entries. Please free some space before adding new stuff.\n");
        exit(1);
    }
    fseek(fp,0,SEEK_SET);
    short directory_bytes_seek = (which_directory_to_Write-1)*32+Directory_Start;
    fseek(fp,directory_bytes_seek,SEEK_SET);
    struct directory_block dblock;
    fread(&dblock,sizeof(dblock),1,fp);
    if(dblock.inode_id!=0)
    {
        printf("Seek not correct. Check the seek calculations.\n");
        exit(1);
    }
    fseek(fp,-32,SEEK_CUR);
    dblock.inode_id = read_inode.inode_num;
    strcpy(dblock.filename,final_name);
    fwrite(&dblock,sizeof(dblock),1,fp);
    
    //Find the group where blocks will be used.
    short read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
    fseek(fp,0,SEEK_SET);
    fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
    struct group_block gblock;
    fread(&gblock,sizeof(gblock),1,fp);
    if(gblock.g_id!=read_inode.group_pointer)
    {
        printf("Wrong group found for the given inode. Check calculations for either curr_inode or group.\n");
        exit(1);
    }
    printf("Inode written on - %d\n",curr_inode);
    printf("How many blocks - %d\n",how_many_blocks);
    if(how_many_blocks <=10)
    {
            char c;
            int num_bytes = 0;
            int i =0;
            fseek(fp,0,SEEK_SET);
            short block_num_to_use = findFreeBlock();
            setFreeBlockBitInUse(block_num_to_use);
            int locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
            fseek(fp,locate_block_bytes,SEEK_SET);
            gblock.g_nums[i] = block_num_to_use;
            while((c=fgetc(fp_file))!=EOF)
            {
                
                //printf("%c",c);
                fwrite(&c,1,1,fp);
                if(num_bytes==512)
                {
                    i++;
                    block_num_to_use = findFreeBlock();
                    setFreeBlockBitInUse(block_num_to_use);
                    locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
                    fseek(fp,locate_block_bytes,SEEK_SET);
                    gblock.g_nums[i] = block_num_to_use;
                    num_bytes = 0;                  
                }
                 num_bytes++;
            }   
    }
    
    
    read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
    fseek(fp,0,SEEK_SET);
    fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
    fwrite(&gblock,sizeof(gblock),1,fp); //Updated the group.
    
    //Update the inode.
    byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
    fseek(fp,byte_pos,SEEK_SET);
    time_t last_modified;//4 bytes
    time(&last_modified);
    read_inode.is_used = 1;
    read_inode.type_of = 0;
    read_inode.last_modified = last_modified;
    read_inode.size_of_file = file_size_int;
    read_inode.directory_inode = inode_current;
    fwrite(&read_inode,sizeof(read_inode),1,fp);
    fclose(fp);
    fclose(fp_file);
    g_free_inodes--;
       
    }
    else
    {
        printf("Error! File already exists in the directory.\n");
    }
    g_sync = 0;
    updateSuperBlock();

}

void write_files_alpha(char *given_file_name)
{
    readSuperBlock();
    makeCheckPoints();
    g_sync = 1;
    
    updateSuperBlock();
    
    
    FILE *fp = fopen("../disk/vdisk","r+");
    FILE *fp_file = fopen(given_file_name,"r");
    set_currFileName(given_file_name);
    //Get the size of the file.
    fseek(fp_file,0L,SEEK_END);
    float file_size = ftell(fp_file);
    int file_size_int = ftell(fp_file);
    fseek(fp_file,0,SEEK_SET);
    fseek(fp,0,SEEK_SET);
    
    //Calculate how many blocks we need to store the file.
    printf("File size - %d\n",file_size_int);
    float test_how_many = (file_size / 512);
    int how_many_blocks = ceil(test_how_many);    
    //printf("\n%d\n",how_many_blocks);
    readSuperBlock();//Update the global variables.
    
    
    if(how_many_blocks<=10)
    {
            //Choose which inode to use
    short curr_inode = checkWhichInodeToUse();
    if(curr_inode==0)
    {
        printf("Please free some space on the disk to write new files and directories.\n");
        exit(1);
    }
    
    //Prepare the inode to write the file, update all parameters for inode.
    short byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
    fseek(fp,byte_pos,SEEK_SET);
    struct inodeBlock read_inode;
    fread(&read_inode,sizeof(read_inode),1,fp);
    
    
    //Directory entry for the file name.
    char final_name[30];
    strcpy(final_name,curr_file_name);
    short which_directory_to_Write = checkWhichDirectoryEntryToUse();    
    if(which_directory_to_Write==0)
    {
        printf("No free directory found in the system. This design only supports 64 directory entries. Please free some space before adding new stuff.\n");
        exit(1);
    }
    fseek(fp,0,SEEK_SET);
    short directory_bytes_seek = (which_directory_to_Write-1)*32+Directory_Start;
    fseek(fp,directory_bytes_seek,SEEK_SET);
    struct directory_block dblock;
    fread(&dblock,sizeof(dblock),1,fp);
    if(dblock.inode_id!=0)
    {
        printf("Seek not correct. Check the seek calculations.\n");
        exit(1);
    }
    fseek(fp,-32,SEEK_CUR);
    dblock.inode_id = read_inode.inode_num;
    strcpy(dblock.filename,final_name);
    fwrite(&dblock,sizeof(dblock),1,fp);
    
    //Find the group where blocks will be used.
    short read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
    fseek(fp,0,SEEK_SET);
    fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
    struct group_block gblock;
    fread(&gblock,sizeof(gblock),1,fp);
    if(gblock.g_id!=read_inode.group_pointer)
    {
        printf("Wrong group found for the given inode. Check calculations for either curr_inode or group.\n");
        exit(1);
    }
    printf("Inode written on - %d\n",curr_inode);
    printf("How many blocks - %d\n",how_many_blocks);
    if(how_many_blocks <=10)
    {
            char c;
            int num_bytes = 0;
            int i =0;
            fseek(fp,0,SEEK_SET);
            short block_num_to_use = findFreeBlock();
            setFreeBlockBitInUse(block_num_to_use);
            int locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
            fseek(fp,locate_block_bytes,SEEK_SET);
            gblock.g_nums[i] = block_num_to_use;
            while((c=fgetc(fp_file))!=EOF)
            {
                
                //printf("%c",c);
                fwrite(&c,1,1,fp);
                if(num_bytes==512)
                {
                    i++;
                    block_num_to_use = findFreeBlock();
                    setFreeBlockBitInUse(block_num_to_use);
                    locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
                    fseek(fp,locate_block_bytes,SEEK_SET);
                    gblock.g_nums[i] = block_num_to_use;
                    num_bytes = 0;                  
                }
                 num_bytes++;
            }   
    }
    
    
    read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
    fseek(fp,0,SEEK_SET);
    fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
    fwrite(&gblock,sizeof(gblock),1,fp); //Updated the group.
    
    //Update the inode.
    byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
    fseek(fp,byte_pos,SEEK_SET);
    time_t last_modified;//4 bytes
    time(&last_modified);
    read_inode.is_used = 1;
    read_inode.type_of = 0;
    read_inode.last_modified = last_modified;
    read_inode.size_of_file = file_size_int;
    read_inode.directory_inode = inode_current;
    fwrite(&read_inode,sizeof(read_inode),1,fp);

    
    }
    
    
    if(how_many_blocks>10 && how_many_blocks <=20)
    {
        int num_blocks_written = 0;
        short curr_inode = checkWhichInodeToUse();
        int inode_for_saving_indirect=curr_inode;
        int indirect_inode = 0;
        if(curr_inode==0)
        {
            printf("Please free some space on the disk to write new files and directories.\n");
            exit(1);
        }
        while(num_blocks_written<20)
        {
            
            if(num_blocks_written==10)
            {
                
                curr_inode = checkWhichInodeToUse();
                if(curr_inode==0)
                {
                    printf("Please free some space on the disk to write new files and directories.\n");
                    exit(1);
                }
                indirect_inode = curr_inode;
                
                //Prepare the inode to write the file, update all parameters for inode.
                short byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
                fseek(fp,byte_pos,SEEK_SET);
                struct inodeBlock read_inode;
                fread(&read_inode,sizeof(read_inode),1,fp);
                
                
                
                
                //Find the group where blocks will be used.
                short read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
                fseek(fp,0,SEEK_SET);
                fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
                struct group_block gblock;
                fread(&gblock,sizeof(gblock),1,fp);
                if(gblock.g_id!=read_inode.group_pointer)
                {
                    printf("Wrong group found for the given inode. Check calculations for either curr_inode or group.\n");
                    exit(1);
                }
                printf("Inode written on - %d\n",curr_inode);
                char c;
                int num_bytes = 0;
                int i =0;
                fseek(fp,0,SEEK_SET);
                short block_num_to_use = findFreeBlock();
                setFreeBlockBitInUse(block_num_to_use);
                int locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
                fseek(fp,locate_block_bytes,SEEK_SET);
                gblock.g_nums[i] = block_num_to_use;
                while((c=fgetc(fp_file))!=EOF)
                {
                    fwrite(&c,1,1,fp);
                    if(num_bytes==512)
                    {
                        i++;
                        block_num_to_use = findFreeBlock();
                        setFreeBlockBitInUse(block_num_to_use);
                        locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
                        fseek(fp,locate_block_bytes,SEEK_SET);
                        gblock.g_nums[i] = block_num_to_use;
                        num_bytes = 0;  
                        num_blocks_written++;    
                    }
                     num_bytes++;
                }
                
                read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
                fseek(fp,0,SEEK_SET);
                fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
                fwrite(&gblock,sizeof(gblock),1,fp); //Updated the group.
                
                //Update the inode.
                byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
                fseek(fp,byte_pos,SEEK_SET);
                time_t last_modified;//4 bytes
                time(&last_modified);
                read_inode.is_used = 1;
                read_inode.type_of = 0;
                read_inode.last_modified = last_modified;
                read_inode.size_of_file = file_size_int;
                read_inode.directory_inode = inode_current;
                fwrite(&read_inode,sizeof(read_inode),1,fp);
                
                //Point the indirect pointer.
                byte_pos = (inode_for_saving_indirect-1)*32+Inode_Start;
                fseek(fp,byte_pos,SEEK_SET);
                struct inodeBlock indirectInode;
                fread(&indirectInode,sizeof(indirectInode),1,fp);
                indirectInode.indirect_pointer = indirect_inode;
                fseek(fp,byte_pos,SEEK_SET);
                fwrite(&indirectInode,sizeof(indirectInode),1,fp);
            }
            if(num_blocks_written<10)
            {
                //Prepare the inode to write the file, update all parameters for inode.
                short byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
                fseek(fp,byte_pos,SEEK_SET);
                struct inodeBlock read_inode;
                fread(&read_inode,sizeof(read_inode),1,fp);
                
                
                //Directory entry for the file name.
                char final_name[30];
                strcpy(final_name,curr_file_name);
                short which_directory_to_Write = checkWhichDirectoryEntryToUse();    
                if(which_directory_to_Write==0)
                {
                    printf("No free directory found in the system. This design only supports 64 directory entries. Please free some space before adding new stuff.\n");
                    exit(1);
                }
                fseek(fp,0,SEEK_SET);
                short directory_bytes_seek = (which_directory_to_Write-1)*32+Directory_Start;
                fseek(fp,directory_bytes_seek,SEEK_SET);
                struct directory_block dblock;
                fread(&dblock,sizeof(dblock),1,fp);
                if(dblock.inode_id!=0)
                {
                    printf("Seek not correct. Check the seek calculations.\n");
                    exit(1);
                }
                fseek(fp,-32,SEEK_CUR);
                dblock.inode_id = read_inode.inode_num;
                strcpy(dblock.filename,final_name);
                fwrite(&dblock,sizeof(dblock),1,fp);
                
                    //Find the group where blocks will be used.
    short read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
    fseek(fp,0,SEEK_SET);
    fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
    struct group_block gblock;
    fread(&gblock,sizeof(gblock),1,fp);
    if(gblock.g_id!=read_inode.group_pointer)
    {
        printf("Wrong group found for the given inode. Check calculations for either curr_inode or group.\n");
        exit(1);
    }
    printf("Inode written on - %d\n",curr_inode);
    printf("How many blocks - %d\n",how_many_blocks);
    if(how_many_blocks <=10)
    {
        char c;
        int num_bytes = 0;
        int i =0;
        fseek(fp,0,SEEK_SET);
        short block_num_to_use = findFreeBlock();
        setFreeBlockBitInUse(block_num_to_use);
        int locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
        fseek(fp,locate_block_bytes,SEEK_SET);
        gblock.g_nums[i] = block_num_to_use;
        while(num_blocks_written<10)
        {
            c=fgetc(fp_file);
            //printf("%c",c);
            fwrite(&c,1,1,fp);
            if(num_bytes==512)
            {
                i++;
                block_num_to_use = findFreeBlock();
                setFreeBlockBitInUse(block_num_to_use);
                locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
                fseek(fp,locate_block_bytes,SEEK_SET);
                gblock.g_nums[i] = block_num_to_use;
                num_bytes = 0;
                num_blocks_written++;    
            }
             num_bytes++;
        }   
   }


        read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
        fseek(fp,0,SEEK_SET);
        fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
        fwrite(&gblock,sizeof(gblock),1,fp); //Updated the group.

        //Update the inode.
        byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
        fseek(fp,byte_pos,SEEK_SET);
        time_t last_modified;//4 bytes
        time(&last_modified);
        read_inode.is_used = 1;
        read_inode.type_of = 0;
        read_inode.last_modified = last_modified;
        read_inode.size_of_file = file_size_int;
        read_inode.directory_inode = inode_current;
        fwrite(&read_inode,sizeof(read_inode),1,fp);
                
    }
    
        }
        
    }    
       
   

    fclose(fp);
    fclose(fp_file);
    g_sync = 0;
    updateSuperBlock();

}

void make_new_directory(char *directory_name)
{
    readSuperBlock();
    g_sync = 1;
    updateSuperBlock(); 
    makeCheckPoints();    
    if(check_if_directoryExists(directory_name)==0)
    {
        FILE *fp = fopen("../disk/vdisk","r+");
        int curr_inode = checkWhichInodeToUse();
        //short inode_id_directory = read_given_file_name(current_directory);    


        struct inodeBlock inode;
        inode.inode_num = curr_inode;
        inode.is_used = 1;
        inode.type_of = 1;
        time_t created;
        time(&created);
        inode.created = created;    
        time_t last_modified;//4 bytes
        time(&last_modified);
        inode.last_modified = last_modified;
        inode.directory_inode = inode_current;
        inode.group_pointer = curr_inode;
        fseek(fp,((curr_inode-1)*32)+Inode_Start,SEEK_SET);
        fwrite(&inode,sizeof(inode),1,fp);
        g_free_inodes--;
        int directory_to_use = checkWhichDirectoryEntryToUse();
        struct directory_block directory;
        directory.inode_id = curr_inode;
        strcpy(directory.filename,directory_name);
        fseek(fp,((directory_to_use-1)*32)+Directory_Start,SEEK_SET);
        fwrite(&directory,sizeof(directory),1,fp);
        fclose(fp);   
    }
    else
    {
        printf("Error! The directory already exists. Please check the name of the mentioned directory.\n");
    }
    
    
    g_sync = 0;
    updateSuperBlock();
}




void remove_directory(char *directory_name)
{
    readSuperBlock();
    g_sync = 1;
    updateSuperBlock();
    // makeCheckPoints();
    
    FILE *fp = fopen("../disk/vdisk","r+");
    fseek(fp,Directory_Start,SEEK_SET);
    short which_inode_to_free;
    short which_directory_to_free;
    for(short i=0;i<64;i++)
    {
        struct directory_block dblock;
        fread(&dblock,sizeof(dblock),1,fp);
        
        if(strcmp(directory_name,dblock.filename)==0)
        {
            which_inode_to_free = dblock.inode_id;
            which_directory_to_free = i;
            break;
        }
    }
    
   struct inodeBlock inode; 
   fseek(fp,((which_inode_to_free-1)*32)+Inode_Start,SEEK_SET);
   fread(&inode,sizeof(inode),1,fp);
   if(inode.type_of==1)
   {
        if(inode.directory_inode==inode_current)
        {
              fseek(fp,((which_inode_to_free-1)*32)+Inode_Start,SEEK_SET);
            inode.is_used = 0;
            fwrite(&inode,sizeof(inode),1,fp);

            struct directory_block directory;
            fseek(fp,((which_directory_to_free)*32+Directory_Start),SEEK_SET);
            directory.inode_id = 0;
            fwrite(&directory,sizeof(directory),1,fp);
          
        }
        else
        {
            printf("Error! Directory %s is not in the current directory.\n",directory_name);
        }
   }
   else
   {
       printf("Error! %s is not a directory that is tried to removed.\n",directory_name);
   }
   fclose(fp);
   
   g_sync = 1;
   updateSuperBlock();
    
}

void remove_file(char *directory_name)
{
    readSuperBlock();
    g_sync = 1;
    makeCheckPoints();
    updateSuperBlock();
    
    FILE *fp = fopen("../disk/vdisk","r+");
    fseek(fp,Directory_Start,SEEK_SET);
    short which_inode_to_free;
    short which_directory_to_free;
    for(short i=0;i<64;i++)
    {
        struct directory_block dblock;
        fread(&dblock,sizeof(dblock),1,fp);
        if(strcmp(directory_name,dblock.filename)==0)
        {
            which_inode_to_free = dblock.inode_id;
            which_directory_to_free = i;
            break;
        }
    }
    
   struct inodeBlock inode; 
   fseek(fp,((which_inode_to_free-1)*32)+Inode_Start,SEEK_SET);
   fread(&inode,sizeof(inode),1,fp);
   if(inode.type_of==0)
   {
        fseek(fp,((which_inode_to_free-1)*32)+Inode_Start,SEEK_SET);
        inode.is_used = 0;
        if(inode.directory_inode==inode_current)
        {
            int group_pointer = inode.group_pointer;
            fwrite(&inode,sizeof(inode),1,fp);

            readGroup(group_pointer);

            for(int i=0;i<sizeof(g_group_nums);i++)
            {
            //printf("%d",g_group_nums[i]);

            if(g_group_nums[i]!=0)
            {
                setUsedBlockBitFree(g_group_nums[i]);
                // printBitInReadByte(g_group_nums[i]);  
                // printf("\n");    
            }

            }
            printf("\n");
            struct directory_block directory;
            fseek(fp,((which_directory_to_free)*32+Directory_Start),SEEK_SET);
            directory.inode_id = 0;
            fwrite(&directory,sizeof(directory),1,fp);    
        }
        else
        {
            printf("Error! %s not found in the current directory.\n",directory_name);
        }
        
        
   }
   else
   {
       printf("Error! It is not a file that is tried to unlink.\n");
   }
   fclose(fp);  
   g_sync = 0;
   updateSuperBlock();
   
}


void save_existing_file(char *given_file_name)
{
    remove_file(given_file_name);
    write_files_beta(given_file_name);
}

void readFile(char *filename,char *target)
{
    int inode = read_given_file_name(filename);
    if(inode==0)
    {
        printf("Error! File not found in the current directory.\n");
    }
    else
    {
            remakeFileThroughInode(inode,target);    
    }
}

void printCWD()
{
    printf("Current directory : %s\n",current_directory);
}

void robustTest(char *given_file_name)
{
    readSuperBlock();
    makeCheckPoints();
    g_sync = 1;
    updateSuperBlock();
    
    if(check_if_directoryExists(given_file_name)==0)
    {
     FILE *fp = fopen("../disk/vdisk","r+");
    FILE *fp_file = fopen(given_file_name,"r");
    set_currFileName(given_file_name);
    //Get the size of the file.
    fseek(fp_file,0L,SEEK_END);
    float file_size = ftell(fp_file);
    int file_size_int = ftell(fp_file);
    fseek(fp_file,0,SEEK_SET);
    fseek(fp,0,SEEK_SET);
    
    //Calculate how many blocks we need to store the file.
    printf("File size - %d\n",file_size_int);
    float test_how_many = (file_size / 512);
    int how_many_blocks = ceil(test_how_many);    
    //printf("\n%d\n",how_many_blocks);
    readSuperBlock();//Update the global variables.
    
    //Choose which inode to use
    short curr_inode = checkWhichInodeToUse();
    if(curr_inode==0)
    {
        printf("Please free some space on the disk to write new files and directories.\n");
        exit(1);
    }
    
    //Prepare the inode to write the file, update all parameters for inode.
    short byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
    fseek(fp,byte_pos,SEEK_SET);
    struct inodeBlock read_inode;
    fread(&read_inode,sizeof(read_inode),1,fp);
    
    
    //Directory entry for the file name.
    char final_name[30];
    strcpy(final_name,curr_file_name);
    short which_directory_to_Write = checkWhichDirectoryEntryToUse();    
    if(which_directory_to_Write==0)
    {
        printf("No free directory found in the system. This design only supports 64 directory entries. Please free some space before adding new stuff.\n");
        exit(1);
    }
    fseek(fp,0,SEEK_SET);
    short directory_bytes_seek = (which_directory_to_Write-1)*32+Directory_Start;
    fseek(fp,directory_bytes_seek,SEEK_SET);
    struct directory_block dblock;
    fread(&dblock,sizeof(dblock),1,fp);
    if(dblock.inode_id!=0)
    {
        printf("Seek not correct. Check the seek calculations.\n");
        exit(1);
    }
    fseek(fp,-32,SEEK_CUR);
    dblock.inode_id = read_inode.inode_num;
    strcpy(dblock.filename,final_name);
    fwrite(&dblock,sizeof(dblock),1,fp);
    
    //Find the group where blocks will be used.
    short read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
    fseek(fp,0,SEEK_SET);
    fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
    struct group_block gblock;
    fread(&gblock,sizeof(gblock),1,fp);
    if(gblock.g_id!=read_inode.group_pointer)
    {
        printf("Wrong group found for the given inode. Check calculations for either curr_inode or group.\n");
        exit(1);
    }
    printf("Inode written on - %d\n",curr_inode);
    printf("How many blocks - %d\n",how_many_blocks);
    if(how_many_blocks <=10)
    {
            char c;
            int num_bytes = 0;
            int i =0;
            fseek(fp,0,SEEK_SET);
            short block_num_to_use = findFreeBlock();
            setFreeBlockBitInUse(block_num_to_use);
            int locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
            fseek(fp,locate_block_bytes,SEEK_SET);
            gblock.g_nums[i] = block_num_to_use;
            while((c=fgetc(fp_file))!=EOF)
            {
                
                //printf("%c",c);
                fwrite(&c,1,1,fp);
                if(num_bytes==512)
                {
                    i++;
                    block_num_to_use = findFreeBlock();
                    setFreeBlockBitInUse(block_num_to_use);
                    locate_block_bytes = (block_num_to_use)*512 + DataBlock_Start;
                    fseek(fp,locate_block_bytes,SEEK_SET);
                    gblock.g_nums[i] = block_num_to_use;
                    num_bytes = 0;                  
                }
                 num_bytes++;
            }   
    }
    
    
    read_bytes_group = (read_inode.group_pointer-1)*22 + Group_Start; //Go to position for the group of the given inode.
    fseek(fp,0,SEEK_SET);
    fseek(fp,read_bytes_group,SEEK_SET); //Set to head for groups.
    fwrite(&gblock,sizeof(gblock),1,fp); //Updated the group.
    
    //Update the inode.
    byte_pos = (curr_inode-1)*32+Inode_Start; //Calculate at what location does the given inode is located.
    fseek(fp,byte_pos,SEEK_SET);
    time_t last_modified;//4 bytes
    time(&last_modified);
    read_inode.is_used = 1;
    read_inode.type_of = 0;
    
    printf("Exiting now in the middle of the write because this is the bad function.\n");
    
    
    
    read_inode.last_modified = last_modified;
    read_inode.size_of_file = file_size_int;
    read_inode.directory_inode = inode_current;
    fwrite(&read_inode,sizeof(read_inode),1,fp);
    fclose(fp);
    fclose(fp_file);
    g_free_inodes--;
       
    }
    else
    {
        printf("Error! File already exists in the directory.\n");
    }

}
void initializeDisk()
{
     InitLLFS();
     readSuperBlock();
     initializeFreeBlockVector();
     initializeAllInodes_Struct();
     initializeDirectoryForRoot();
     makeGroupsForInodes();
     printDiskStats();      
}


