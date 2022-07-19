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
struct timespec timecheck, timecheck2, request;
long freq = 100000;
u_int8_t check_loop = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
//main of thread sample
void *currently_time(void *time)
{  
  clock_gettime(CLOCK_REALTIME, &request);
  char buff[100];
    while(1)
    {
        clock_gettime(CLOCK_REALTIME,&now);  
        if(request.tv_nsec + freq > 1000000000)
        {
         long temp = request.tv_nsec;
          request.tv_sec +=1;
          request.tv_nsec =   temp  + freq - 1000000000; 
                 if(clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME, &request,NULL) != 0)
                  {
                      check_loop = 0;
                  }
                  else
                  {
                      check_loop = 1;
                  }
       }
        else{
            request.tv_nsec +=freq;
            if(clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME, &request,NULL) != 0)
              {
                  check_loop = 0;
              }
            else
              {
                  check_loop = 1;
              }
        }
  }
      return NULL;
}


//main of thread input
void *check_time(void *time)
{ 
  while(1)
  {
      pthread_mutex_lock(&mutex);
      long x = (*(long*)time);
      // struct timespec timecheck;
      FILE *file;
      file = fopen("freq1.txt","r");
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
        file = fopen("freq1.txt","w");
        fprintf(file,"%ld",x);
        fclose(file);
        cycle = x;
        timecheck.tv_nsec = data; // attach
        pthread_mutex_unlock(&mutex);
    }
      else
      {
        return NULL;
        pthread_mutex_unlock(&mutex);
      } 
        return NULL;
}
}

//main of thread logging
void *save_time(void *time)
{
  while(1){
    FILE *fp;
    fp = fopen("kaka.txt","r");
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
      char *str;
     long old_nsec = 0;
     long old_sec = 0;
     old_sec = strtol(t_sec,&str,10);
     old_nsec = strtol(t_nsec,&str,10);
    

      //printf("old_value %ld\n",old_sec);
      //printf("new_value %ld. %ld\n", now.tv_sec, now.tv_nsec);

    //save time
    FILE *file;
    FILE *file1;
    file1 = fopen("save_value.txt","a+"); // save all of offset values in file
    file = fopen("kaka.txt","w+");
   
    long interval_sec = ((long)now.tv_sec) - old_sec ;
    long interval_nsec;

    if(now.tv_nsec > old_nsec)
      {
        interval_nsec = now.tv_nsec - old_nsec;
      }
    else 
      {
        interval_nsec = 1000000000 + now.tv_nsec - old_nsec;
        interval_sec = interval_sec - 1;
      }
    fprintf(file,"%ld.",now.tv_sec);
    fprintf(file,"%ld",now.tv_nsec);
    //fprintf(file1,"%ld.",now.tv_sec);
    //fprintf(file1,"%ld  ",now.tv_nsec);
    fprintf(file1,"%ld,000",interval_sec);
    fprintf(file1,"%ld \n",interval_nsec);
    printf("%ld\n", interval_nsec);
    fclose(file);
    fclose(file1);
   // sleep(1);
  }
}
int main(){
  FILE *file;
      file = fopen("freq1.txt","r");
      char buff[100];
      fgets(buff,sizeof(buff),file);
      //convert from string to long
      char *eptr;
      long data;
      data = strtol(buff,&eptr,10);
     fclose(file);
    long j = 1;
    
    long i = data;
    //printf("%ld", i);
    int* ptr;
    pthread_t sample;
    pthread_t input;
    pthread_t logging;
   // pthread_mutex_init(&mutex, NULL);   
  //  while(1){
      
  //       if(clock_nanosleep(&timecheck , &timecheck2) < 0 )   
  //     {
  //       printf("Nano sleep system call failed \n");
  //       return -1;
  //     }
  //    else{
      pthread_create(&input,NULL,check_time,&i);
      pthread_create(&sample, NULL, currently_time,&i);
      pthread_create(&logging,NULL,save_time,&i);
      pthread_join(input,(void**)&ptr);
      pthread_join(logging,(void**)&ptr);
      pthread_join(sample, (void**)&ptr);  
  //     //sleep(1);    
  //  }
  //      }
   return 0;
}