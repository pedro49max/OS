#include <stdlib.h>//gcc savage.c -o savage -lrt -lpthread
#include <stdio.h>//for i in {1..5} do ./savage & done
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define NUMITER 3
#define SHM_NAME "/cauldron"

sem_t *empty, *full, *mutex;
int *cauldron;

int getServingsFromPot(void){ 
    if (*cauldron > 0) {
        (*cauldron)--; // Take a serving.
        printf("Savage %lu took a serving. Servings left: %d\n", (unsigned long)getpid(), *cauldron);
    }

    int servings_left = *cauldron;

    sem_post(mutex);  // Unlock access to shared memory.
    if (servings_left == 0) {
        sem_post(empty); // Signal the cook to refill.
        sem_wait(full);   // Wait until the cauldron has servings.
        sem_wait(mutex);  // Lock access to shared memory.
    }

    return servings_left;
}

void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void savages(void){
	for (int i = 0; i < NUMITER; i++) {
        if (getServingsFromPot() >= 0) {
            eat();
        }
    }
}

int main(int argc, char *argv[]){
	// Open shared memory.
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        fprintf(stderr, "Error: Please start the cook program first.\n");
        return 1;
    }

    cauldron = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Open semaphores.
    empty = sem_open("/sem_empty", 0);
    if(empty == SEM_FAILED){
        perror("error creating semaphore 'empty'");
        exit(1);
    }
    full = sem_open("/sem_full", 0);
    if(full == SEM_FAILED){
        perror("error creating semaphore 'full'");
        exit(1);
    }
    mutex = sem_open("/sem_mutex", 0);
    if(mutex == SEM_FAILED){
        perror("error creating semaphore 'mutex'");
        exit(1);
    }
    printf("Cook: Semaphores initialized successfully.\n");


    savages();

	return 0;
}
