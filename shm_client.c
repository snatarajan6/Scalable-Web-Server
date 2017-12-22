#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/mman.h>
#include<string.h>
#include<signal.h>
#include<time.h>
#include<sys/time.h>
#include<pthread.h>
#define SHM_NAME "neelavara_snatarajan"

pthread_mutex_t *mutex;
struct timeval start, end;

typedef struct {
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
    int occupied; // 0 - Not Occupied and 1 - Occupied
} stats_t;

void *ptr;
//stats_t *client_details;
stats_t *temp;

void exit_handler(int sig) {
	pthread_mutex_lock(mutex);	
	//unallocate the memory
	temp->occupied = 0;
	pthread_mutex_unlock(mutex);
	exit(0);
}


int main(int argc, char *argv[]){
	int iterate = 0;
	int page_size = getpagesize();
	int fd = shm_open(SHM_NAME, O_RDWR, 0660);
	if(fd == -1){
		exit(1);
		}
	struct sigaction act	;
        act.sa_handler=exit_handler;
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTERM, &act, NULL);
	ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	mutex = (pthread_mutex_t*) ptr;
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	gettimeofday(&start, NULL);
	stats_t *start_ptr = (stats_t *) ptr;
	start_ptr++;
	stats_t *p;
	temp = (stats_t *)ptr;
	//pthread_mutex_lock(mutex);
	while(iterate <= 63){
			temp++;
                        iterate++;   
			if(temp->occupied == 0){
			 	pthread_mutex_lock(mutex);
				temp->occupied = 1;
				pthread_mutex_unlock(mutex);
				break;
			}
			//iterate++;
	}
	if(iterate > 63)
		exit(0);
		
	temp->pid = getpid();	
	time_t current_time;
        char *time_string;
        current_time = time(NULL);
        time_string = ctime(&current_time);
	//time_string[strlen(time_string)-1] = '\0';
	snprintf(temp->birth, 25, "%s", time_string);
	//strncpy(temp->birth,time_string, 24);
	if(strlen(argv[1]) > 9) exit(1); 
	strncpy(temp->clientString,argv[1], 9);

	while(1){
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	gettimeofday(&end, NULL);
	temp->elapsed_sec = (int)(end.tv_sec - start.tv_sec);
	temp->elapsed_msec = (double)(end.tv_usec - start.tv_usec)/1000.0f;
	sleep(1);
	int iterate = 1;
	p = (stats_t *) ptr;
	p++;
	printf("Active clients :");
	while(iterate <= 63){
		if(p->occupied == 1){
			printf(" %d",p->pid);
		}
		p++;
		iterate++;
	}	
	printf("\n");
	}
	return 0;
}

