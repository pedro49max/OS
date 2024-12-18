#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "teacher" : "student")

// Global counters and synchronization primitives
int current_count = 0;          // Current number of clients in the disco
int waiting_teacher = 0;            // Number of VIPs waiting to enter
int turnStudent=0;
int turnTeacher=0;
int vacationTime = 0;
sem_t capacity_sem;             // Semaphore to enforce club capacity
pthread_mutex_t mutex;          // Mutex for synchronizing access to shared data

void enter_student(int id, int turn){
	pthread_mutex_lock(&mutex);

    // Wait for any VIPs to enter first
    while (turn != turnStudent || waiting_teacher > 0 || current_count >= CAPACITY|| vacationTime%2==1) {
        pthread_mutex_unlock(&mutex);
        usleep(100); // Slight delay to avoid busy waiting
        pthread_mutex_lock(&mutex);
    }

    current_count++;
    turnStudent++;
    printf("User %2d (student) waiting on queue.\n", id);

    pthread_mutex_unlock(&mutex);
    sem_post(&capacity_sem);
}

void enter_teacher(int id, int turn){
	pthread_mutex_lock(&mutex);
    waiting_teacher++; // Increment waiting VIP count

    // Wait until there is space in the club
    while (turn != turnTeacher || current_count >= CAPACITY|| vacationTime%2==1) {
        pthread_mutex_unlock(&mutex);
        usleep(100); // Slight delay to avoid busy waiting
        pthread_mutex_lock(&mutex);
    }

    waiting_teacher--; // VIP is about to enter, decrement waiting count
    turnTeacher++;
    current_count++;
    printf("User %2d (teacher) waiting on queue.\n", id);

    pthread_mutex_unlock(&mutex);
    sem_post(&capacity_sem);
}

void reading(int id, int isvip)
{
	printf("User %2d (%s) is reading books for 2 seconds\n", id, VIPSTR(isvip));
	sleep(2);
}

void library_exit(int id, int isvip){
	sem_wait(&capacity_sem);
    pthread_mutex_lock(&mutex);

    current_count--;
    printf("User %2d (%s) leaves the library.\n", id, VIPSTR(isvip));

    pthread_mutex_unlock(&mutex);
}

void *client(void *arg){
	int id = ((int *)arg)[0];
    int isTeacher = ((int *)arg)[1];
    int turn = ((int *)arg)[2];
    
    if (isTeacher)
        enter_teacher(id, turn);
    else
        enter_student(id, turn);

    // Client dances in the disco
    reading(id, isTeacher);

    // Client exits the disco
    library_exit(id, isTeacher);

    free(arg); // Free memory allocated for arguments
    return NULL;
}
void *vacation(){
    pthread_mutex_lock(&mutex);
    while(1){
        pthread_mutex_unlock(&mutex);
        sleep(1); // Slight delay to avoid busy waiting
        pthread_mutex_lock(&mutex);
        vacationTime++;
        printf("Library is closing for vacation.\n");
        pthread_mutex_unlock(&mutex);
        sleep(3); // Slight delay to avoid busy waiting
        pthread_mutex_lock(&mutex);
        vacationTime++;
        printf("Library is now open after vacation.\n");
    }
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

    pthread_t threads[M +1];
    int id, isTeacher;
    pthread_create(&threads[0], NULL, vacation, NULL);
    // Initialize synchronization primitives
    sem_init(&capacity_sem, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    int numStudent = 0;
    int numTeacher=0;
    for (int i = 0; i < M; i++) {
        fscanf(file, "%d", &isTeacher);

        int *arg = malloc(3 * sizeof(int));
        arg[0] = i + 1;   // Client ID
        arg[1] = isTeacher;   // VIP status
        if(isTeacher){
            arg[2] = numTeacher;
            numTeacher +=1;
        }
        else{
            arg[2]= numStudent;
            numStudent +=1;
        }

        pthread_create(&threads[i +1], NULL, client, arg);
    }
    fclose(file);

    // Wait for all threads to complete
    for (int i = 1; i <= M; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_cancel(threads[0]);
    // Clean up
    sem_destroy(&capacity_sem);
    pthread_mutex_destroy(&mutex);

    return 0;
}
