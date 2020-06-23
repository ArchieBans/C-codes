#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <string.h>
#include <semaphore.h> 
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>
pthread_mutex_t test_mutex;
  

#define MAX_POINTS 3700
#define MAX_SUMS 6843150
#define MAX_THREADS 1000000 
void getData(char *buff);
typedef struct point {
    float x_val;
    float y_val;
    int isDone;
    int isCurr;
    float residual;
} point;

typedef struct two_points{
    float x1_val;
    float y1_val;
    float x2_val;
    float y2_val;
    int index_i;
    int index_j;
} two_points;

point all_points[MAX_POINTS];
int counter=0;
float all_sums[MAX_SUMS][3];
int sum_counter = 0;
int thread_counter = 0;
char *fileName;  



void process_csv_file()
{
    //FILE *fp = fopen("stremflow_time_series.csv","r");
    FILE *fp = fopen(fileName,"r");   
    int count=0;
	do
	{
	 char buff[1024];
	 fgets(buff, 1024, (FILE*)fp);
	 count++;
	 if(count != 1)
	 {
      all_points[count-1].x_val = count-1;
      counter = count-1;  
      getData(buff);
	 }
	 getch();
	}while((getc(fp))!=EOF);
}
void getData(char buff[])
{
   char *token = strtok(buff,",");
   int mod_counter = 0;
   while( token != NULL ) 
   {       
    mod_counter++;
    token = strtok(NULL,",");
    if (mod_counter % 2 !=0)
    {
        all_points[counter].y_val = strtof(token, NULL);
        all_points[counter].isDone = 0;
    }
   }	  
}

void generate_points(float points[MAX_POINTS][2],int n_rows,int n_columns)
{
    
    for(int i = 0; i<n_rows;i++)
    {
        for(int j = 0;j<n_columns;j++)
        {
            if(j==0)
            {
                  all_points[counter].x_val = points[i][j];          
            }
            if(j==1)
            {
                  all_points[counter].y_val = points[i][j];          
            }
        }
        all_points[counter].isDone = 0;
        counter++;
    }
}


void *work_threads(void* arguments)
{
    
    struct two_points *args = arguments;
    
    float slope = (args->y2_val-args->y1_val)/(args->x2_val-args->x1_val);
    float intercept = (args->y1_val - (args->x1_val*slope));
    
    float sum_val;
    for(int i = 0;i<counter;i++)
    {
        if(i!=args->index_i && i!=args->index_j)
        {
            float y_val = fabs(all_points[i].y_val - all_points[i].x_val*slope - intercept);
            sum_val = sum_val + y_val;
            //printf("Y-val=%f,Sum-val=%f\n",y_val,sum_val);
        }
    }
    
    
    pthread_mutex_lock(&test_mutex);
    all_sums[sum_counter][0] = sum_val;
    all_sums[sum_counter][1] = slope;
    all_sums[sum_counter][2] = intercept;
    sum_counter++;
    pthread_mutex_unlock(&test_mutex);
}


bool almostEqual(float a, float b)
{
    return fabs(a - b) <= FLT_EPSILON;
}
int find_minimum_sum()
{
    float sum = 1000000000000;
    int save_index =0 ;
    float mantissa = 0.00000;
 for(int i=1;i<sum_counter;i++)
    {
        if(!almostEqual(all_sums[i][0],mantissa) && !almostEqual(all_sums[i][1],mantissa) && !almostEqual(all_sums[i][2],mantissa))
        {
         if(all_sums[i][0] < sum)
           {
            sum  = all_sums[i][0];
            save_index = i;
        }   
        }
        
    }
    return save_index;
}
void Process_Requested_Points(int val)
{
    float points[6][2] = {1,87.6,2,88.9,3,90.4,4,91.3,5,92.9,6,95.4};
    float points_10[10][2] = {1,87.6,2,88.9,3,90.4,4,91.3,5,92.9,6,95.4,7,97.8,8,100,9,102.8,10,104.7};
    float points_14[14][2] = {1,87.6,2,88.9,3,90.4,4,91.3,5,92.9,6,95.4,7,97.8,8,100,9,102.8,10,104.7,11,107,12,109.1,13,111.5,14,114.1};
    float points_2[18][2] = {1,87.6,2,88.9,3,90.4,4,91.3,5,92.9,6,95.4,7,97.8,8,100,9,102.8,10,104.7,11,107,12,109.1,13,111.5,14,114.1,15,114.4,16,116.5,17,119.9,18,121.7};
    int start_val = 0;
    if (val==0)
    {
        printf("Running the calculations for first 6 points\n");
        generate_points(points,sizeof(points)/sizeof(points[0]),2);
    }
    else if(val==1)
    {
        printf("Running the calculations for first 10 points\n");
        generate_points(points_10,sizeof(points_10)/sizeof(points_10[0]),2);
    }
    else if(val==2)
    {
        printf("Running the calculations for first 14 points\n");
        generate_points(points_14,sizeof(points_14)/sizeof(points_14[0]),2);
    }
    else if (val==3)
    {
        printf("Running the calculations for first 18 points\n");
        generate_points(points_2,sizeof(points_2)/sizeof(points_2[0]),2);
    }
    else if (val==4)
    {
        printf("Running the calculations for Year 2002.\n");
        fileName = "Year_2002.csv";
        process_csv_file();
        start_val = 1;
    }
    else if (val == 5)
    {
        printf("Running the calculations for 10 years. This will take time number of pairs are 6,666,726.\n");
        fileName = "stremflow_time_series.csv";
        process_csv_file();
        start_val = 1;
    }
    pthread_t threads[MAX_THREADS];
    pthread_mutex_init(&test_mutex, NULL);

    for(int i = start_val;i<counter;i++)
    {
        for(int j = i;j<counter;j++)
        {
            if(j!=i && all_points[i].isDone==0)
            {
            struct two_points test;
            
            test.index_i = i;
            test.index_j = j;
            
            test.x1_val = all_points[i].x_val;
            test.y1_val = all_points[i].y_val;
            test.x2_val = all_points[j].x_val;
            test.y2_val = all_points[j].y_val;
            
            pthread_create(&threads[thread_counter],NULL,work_threads,(void *)&test);
            pthread_join(threads[thread_counter],NULL);
            thread_counter++;
            if(thread_counter==MAX_THREADS){
                thread_counter = 0;
                }
            }
            
        }
        
        all_points[i].isDone = 1;
            
    }
    
    int val_index = find_minimum_sum();
    
    printf("Line with best L1 fit is, Sum : %f, Intercept: %f, Slope: %f\n",all_sums[val_index][0],all_sums[val_index][2],all_sums[val_index][1]);
    
    pthread_mutex_destroy(&test_mutex);
}
int main(int argc,char* argv[]) 
{ 
    if(argc==1)
    {
        printf("Please read the readme file for the usage of the program.\n");
        exit(0);
    }
    else if(argc==2)
    {
        int val = *argv[1] - '0';
        if(val>=0 && val<=5)
        {
               Process_Requested_Points(val);         
        }
    }
    else{
        printf("Please read the readme file for the usage of the program.\n");
        exit(0);
        
    }

    return 0; 
} 