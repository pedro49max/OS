#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

// Global counters and synchronization primitives
int current_count = 0;          // Current number of clients in the disco
int waiting_vip = 0;            // Number of VIPs waiting to enter
int turnN=0;
int turnVIP=0;
sem_t capacity_sem;             // Semaphore to enforce club capacity
pthread_mutex_t mutex;          // Mutex for synchronizing access to shared data

void enter_normal_client(int id, int turn){
	pthread_mutex_lock(&mutex);

    // Wait for any VIPs to enter first
    while (turn != turnN || waiting_vip > 0 || current_count >= CAPACITY) {
        pthread_mutex_unlock(&mutex);
        usleep(100); // Slight delay to avoid busy waiting
        pthread_mutex_lock(&mutex);
    }

    current_count++;
    turnN++;
    printf("Client %2d (not vip) entered the disco. Current count: %d\n", id, current_count);

    pthread_mutex_unlock(&mutex);
    sem_post(&capacity_sem);
}

void enter_vip_client(int id, int turn){
	pthread_mutex_lock(&mutex);
    waiting_vip++; // Increment waiting VIP count

    // Wait until there is space in the club
    while (turn != turnVIP || current_count >= CAPACITY) {
        pthread_mutex_unlock(&mutex);
        usleep(100); // Slight delay to avoid busy waiting
        pthread_mutex_lock(&mutex);
    }

    waiting_vip--; // VIP is about to enter, decrement waiting count
    turnVIP++;
    current_count++;
    printf("Client %2d (vip) entered the disco. Current count: %d\n", id, current_count);

    pthread_mutex_unlock(&mutex);
    sem_post(&capacity_sem);
}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip){
	sem_wait(&capacity_sem);
    pthread_mutex_lock(&mutex);

    current_count--;
    printf("Client %2d (%s) exited the disco. Current count: %d\n", id, VIPSTR(isvip), current_count);

    pthread_mutex_unlock(&mutex);
}

void *client(void *arg){
	int id = ((int *)arg)[0];
    int isvip = ((int *)arg)[1];
    int turn = ((int *)arg)[2];
    
    if (isvip)
        enter_vip_client(id, turn);
    else
        enter_normal_client(id, turn);

    // Client dances in the disco
    dance(id, isvip);

    // Client exits the disco
    disco_exit(id, isvip);

    free(arg); // Free memory allocated for arguments
    return NULL;
}

int main(int argc, char *argv[]){
	if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    int M; // Number of clients
    fscanf(file, "%d", &M);

    pthread_t threads[M];
    int id, isvip;

    // Initialize synchronization primitives
    sem_init(&capacity_sem, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    int numNormal = 0;
    int numVIP=0;
    for (int i = 0; i < M; i++) {
        fscanf(file, "%d", &isvip);

        int *arg = malloc(3 * sizeof(int));
        arg[0] = i + 1;   // Client ID
        arg[1] = isvip;   // VIP status
        if(isvip){
            arg[2] = numVIP;
            numVIP +=1;
        }
        else{
            arg[2]= numNormal;
            numNormal +=1;
        }

        pthread_create(&threads[i], NULL, client, arg);
    }

    fclose(file);

    // Wait for all threads to complete
    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    sem_destroy(&capacity_sem);
    pthread_mutex_destroy(&mutex);

    return 0;
}
