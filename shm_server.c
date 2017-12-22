#include<sys/stat.h>
#include<pthread.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include <sys/mman.h>
#include<string.h>
#define SHM_NAME "neelavara_snatarajan"
int page_size;
pthread_mutex_t mutex;
pthread_mutexattr_t mutexAttribute;
typedef struct {
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
    int occupied;
} stats_t;
void *ptr;

void exit_handler(int sig){
	munmap(ptr,page_size);
        shm_unlink(SHM_NAME);
	exit(0);
}

int main(int argc, char *argv[]){
	int iteration = 1;
	page_size = getpagesize();
	int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
	if(fd == -1)
                exit(1);
	ftruncate(fd, page_size);
	struct sigaction act;
        act.sa_handler=exit_handler;
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTERM, &act, NULL);
	ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//pthread_mutex_t *mlock = (pthread_mutex_t *) ptr;	
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&mutex, &mutexAttribute);
	memcpy(ptr, &mutex, sizeof(pthread_mutex_t));
        stats_t *start_ptr = (stats_t *)ptr;
	start_ptr++;
	stats_t *temp;
	while(1){
	  int count = 1;
	  temp = start_ptr;
             while(count <= 63){
                if(temp->occupied == 1)
                    printf("%d, pid : %d, birth : %s, elapsed : %d s %.4lf ms, %s\n",iteration,temp->pid,temp->birth,temp->elapsed_sec,temp->elapsed_msec,temp->clientString);
                temp++;
		//printf("\n");
                count++;
       	 	}
		iteration++;
		printf("\n");
		sleep(1);
	}
   return 0;
}

