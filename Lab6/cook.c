#include <stdlib.h>//gcc cook.c -o cook -lrt -lpthread
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define M 10
#define SHM_NAME "/cauldron"

int finish = 0;
sem_t *empty, *full, *mutex;
int *cauldron;

void putServingsInPot(int servings){
	sem_wait(empty); // Wait for the savages to signal that the cauldron is empty.
    sem_wait(mutex); // Lock access to shared memory.

    *cauldron = servings; // Refill the cauldron.
    printf("Cook: Refilled the cauldron with %d servings.\n", servings);

    sem_post(mutex); // Unlock access to shared memory.
    sem_post(full);  // Signal that the cauldron is ready for use.
}

void cook(void)
{
	while(!finish) {
		putServingsInPot(M);
	}
}

void handler(int signo){
	finish = 1;

	// Clean up shared memory and semaphores.
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    sem_unlink("/sem_empty");
    sem_unlink("/sem_full");
    sem_unlink("/sem_mutex");
    shm_unlink(SHM_NAME);

    printf("Cook: Cleaned up resources and exiting.\n");
    exit(0);
}

int main(int argc, char *argv[]){
	signal(SIGINT, handler);
    signal(SIGTERM, handler);

    // Initialize shared memory.
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1){
        perror("error creating shared memory");
        exit(1);
    }
    ftruncate(shm_fd, sizeof(int));
    cauldron = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(cauldron == MAP_FAILED){
        perror("error mapping shared memory");
        exit(1);
    }
    *cauldron = 0;

    printf("Cook: Shared memory initialized successfully.\n");

    // Initialize semaphores.
    empty = sem_open("/sem_empty", O_CREAT, 0666, 0);
    if(empty == SEM_FAILED){
        perror("error creating semaphore 'empty'");
        exit(1);
    }
    full = sem_open("/sem_full", O_CREAT, 0666, 0);
    if(full == SEM_FAILED){
        perror("error creating semaphore 'full'");
        exit(1);
    }
    mutex = sem_open("/sem_mutex", O_CREAT, 0666, 1);
    if(mutex == SEM_FAILED){
        perror("error creating semaphore 'mutex'");
        exit(1);
    }

    printf("Cook: Semaphores initialized successfully.\n");

    cook();
	
	return 0;
}
