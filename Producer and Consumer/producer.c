#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <inttypes.h> 
#include <unistd.h>
#include <signal.h>
void cleanup();
void sigint_handler(int sig);
double generate_normal(double mean, double stddev);
void gettime();
void set_shared_memory();

int N;
typedef struct {
    char name[11];
    double mean;
    double stddev;
    int T;
} producer;

typedef struct {
    double *buffer;
    sem_t mutex;
    sem_t empty;
    sem_t full;
    int i;
    bool initialized;
    int N;
} shared_data;

shared_data *shared;
double *buffer;
char (*names)[11];
int shmid,shmid1,shmid2;

int main() {
    signal(SIGINT, sigint_handler);

    producer produce;
    scanf("%10s %lf %lf %d %d", produce.name, &produce.mean, &produce.stddev, &produce.T, &N);
    set_shared_memory();
    while (1) {
        double price = generate_normal(produce.mean, produce.stddev);
        gettime();
        fprintf(stderr, "%s: generating a new value %.2lf\n", produce.name, price);

        sem_wait(&shared->empty);
        gettime();
        fprintf(stderr, "trying to get mutex on shared buffer\n");
        sem_wait(&shared->mutex);

        buffer[shared->i] = price;
        snprintf(names[shared->i], 11, "%s",produce.name);
        shared->i = (shared->i + 1) % N;
        gettime();
        fprintf(stderr, "placing %.2lf on shared buffer\n", price);

        sem_post(&shared->mutex);
        sem_post(&shared->full);

        gettime();
        fprintf(stderr, "sleeping for %d ms\n", produce.T);
        fprintf(stderr, "\n");
        usleep(produce.T * 1000);
    }

    return 0;
}

void cleanup() {
    shmdt(shared);
    shmdt(names);
    shmdt(buffer);
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    sem_destroy(&shared->mutex);
    sem_destroy(&shared->empty);
    sem_destroy(&shared->full);
    exit(0);
}

void sigint_handler(int sig) {
    cleanup();
}

double generate_normal(double mean, double stddev) {
    double u1, u2, z0;
    u1 = (double)rand() / RAND_MAX;
    u2 = (double)rand() / RAND_MAX;
    z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return z0 * stddev + mean;
}

void gettime() {
    struct timespec ts;
    struct tm *tm_info;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        perror("clock_gettime");
        exit(1);
    }
    tm_info = localtime(&ts.tv_sec);
    if (!tm_info) {
        perror("localtime");
        exit(1);
    }
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y %H:%M:%S", tm_info);
    fprintf(stderr, "[%s.%03ld] ", buffer, ts.tv_nsec / 1000000);
}

void set_shared_memory(){

    key_t key = ftok("shared_file.txt", 1);
    key_t key1= ftok("buffer.txt", 2);
    key_t key2= ftok("names.txt", 3);
    shmid = shmget(key, sizeof(shared_data), 0666 | IPC_CREAT);
    shmid1 = shmget(key1, N * sizeof(double), 0666 | IPC_CREAT);
    shmid2 = shmget(key2, N * 11, 0666 | IPC_CREAT);
    if (shmid == -1 || shmid1 == -1 || shmid2 == -1) {
        perror("shmget failed");
        exit(1);
    }
    shared = (shared_data *)shmat(shmid, NULL, 0);
    buffer = (double *)shmat(shmid1, NULL, 0);
    names = shmat(shmid2, NULL, 0);
    if (shared == (void *)-1 || buffer == (void *)-1 || names == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }
}