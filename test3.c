#include<stdio.h>
#include<stdint.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
// global
long cycle;
struct timespec now;
struct timespec timecheck, timecheck2;


//main of thread sample
void *currently_time(void *time)
{  

     char buff[100];
      clock_gettime(CLOCK_REALTIME,&now);
      // strftime(buff, sizeof buff, "%D %T", gmtime(&now.tv_sec));
      // printf("Current time: %s.%09ld \n", buff, now.tv_nsec);
      printf("%09ld.%09ld \n",now.tv_sec, now.tv_nsec);
  return NULL;
}

//main of thread input
void *check_time(void *time)
{ 
      long x = (*(long*)time);
      // struct timespec timecheck;
      FILE *file;
      file = fopen("freq.txt","r");
      char buff[100];
      fgets(buff,sizeof(buff),file);
      //convert from string to long
      char *eptr;
      long data;
      data = strtol(buff,&eptr,10);
      fclose(file);


    long checktime = data;
    if(checktime != x) //check parameters
    {
      FILE *file;
        file = fopen("freq.txt","w");
        fprintf(file,"%ld",x);
        fclose(file);
        cycle = x;
        timecheck.tv_nsec = data; // attach
    }
      else
      {
        return NULL;
      } 
        return NULL;
}

//main of thread logging
void *save_time(void *time)
{
    FILE *fp;
    fp = fopen("time_and_interval.txt","r");
    char buff[100];
    fgets(buff,sizeof(buff),fp);
    char *t_nsec;
    char *t_sec;
    //read to first element to "-" element
    const char s[2] = ".";
    t_sec = strtok(buff,s);
    t_nsec = strtok(NULL,s);
    fclose(fp);

    //convert to long 
     char *tin;
     long old_nsec = 0;
     long old_sec = 0;
     old_sec = strtol(t_sec,&tin,10);
     old_nsec = strtol(t_nsec,&tin,10);
    

    //   printf("old_value %ld. %ld\n",old_sec, old_nsec);
    //   printf("new_value %ld. %ld\n", now.tv_sec, now.tv_nsec);

    //save time
    FILE *file;
    FILE *file1;
    file1 = fopen("save_value.txt","a+"); // save all of offset values in file
    file = fopen("time_and_interval.txt","w+");
   
    long interval_sec = ((long)now.tv_sec) - old_sec ;
    long interval_nsec;

    if(now.tv_nsec > old_nsec)
      {
        interval_nsec = now.tv_nsec - old_nsec;
      }
    else 
      {
        interval_nsec = old_nsec - now.tv_nsec;
        interval_sec = interval_sec - 1;
      }
    fprintf(file,"%ld.",now.tv_sec);
    fprintf(file,"%ld",now.tv_nsec);
    fprintf(file1,"%ld,",interval_sec);
    fprintf(file1,"%ld \n",interval_nsec);
    fclose(file);
     
    fclose(file1);
}
int main(){
  FILE *file;
      file = fopen("freq.txt","r");
      char buff[100];
      fgets(buff,sizeof(buff),file);
      //convert from string to long
      char *eptr;
      long data;
      data = strtol(buff,&eptr,10);
     fclose(file);
    long j = 1;
    long i = data;
    int* ptr;
    pthread_t sample;
    pthread_t input;
    pthread_t logging;
   // pthread_mutex_init(&mutex, NULL);   
    while(1){
      
        if(nanosleep(&timecheck , &timecheck2) < 0 )   
      {
        printf("Nano sleep system call failed \n");
        return -1;
      }
      else{
     pthread_create(&input,NULL,check_time,&i);
      pthread_create(&sample, NULL, currently_time,&j);
      pthread_create(&logging,NULL,save_time,&i);
      pthread_join(input,(void**)&ptr);
      pthread_join(logging,(void**)&ptr);
      pthread_join(sample, (void**)&ptr);      
    }
        }
   return 0;
}