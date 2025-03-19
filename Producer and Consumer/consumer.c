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
#include <string.h>
#include <ctype.h>

void updateCommodity(double price, double arr[], int *index, int *counter, const char *name, int terminalRow);
double calculateAverage(double arr[4]);

double GoldArr[4];
double SilverArr[4];
double CrudeOilArr[4];
double NaturalGasArr[4];
double AluminiumArr[4];
double CopperArr[4];
double NickelArr[4];
double LeadArr[4];
double ZincArr[4];
double MenthaOilArr[4];
double CottonArr[4];

int GoldCounter = 0;
int SilverCounter = 0;
int CrudeOilCounter = 0;
int NaturalGasCounter = 0;
int AluminiumCounter = 0;
int CopperCounter = 0;
int NickelCounter = 0;
int LeadCounter = 0;
int ZincCounter = 0;
int MenthaOilCounter = 0;
int CottonCounter = 0;

int GoldIndex = 0;
int SilverIndex = 0;
int CrudeOilIndex = 0;
int NaturalGasIndex = 0;
int AluminiumIndex = 0;
int CopperIndex = 0;
int NickelIndex = 0;
int LeadIndex = 0;
int ZincIndex = 0;
int MenthaOilIndex = 0;
int CottonIndex = 0;

typedef struct
{
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
int shmid, shmid1, shmid2;

void printcommodities()
{
    printf("\e[1;1H\e[2J");
    printf("+----------------------------------------+\n");
    printf("|  Currency  |   Price    |  AvgPrice    |\n");
    printf("+----------------------------------------+\n");
    printf("| ALUMINIUM  |    0.00    |    0.00      |\n");
    printf("| COPPER     |    0.00    |    0.00      |\n");
    printf("| COTTON     |    0.00    |    0.00      |\n");
    printf("| CRUDEOIL   |    0.00    |    0.00      |\n");
    printf("| GOLD       |    0.00    |    0.00      |\n");
    printf("| LEAD       |    0.00    |    0.00      |\n");
    printf("| MENTHAOIL  |    0.00    |    0.00      |\n");
    printf("| NATURALGAS |    0.00    |    0.00      |\n");
    printf("| NICKEL     |    0.00    |    0.00      |\n");
    printf("| SILVER     |    0.00    |    0.00      |\n");
    printf("| ZINC       |    0.00    |    0.00      |\n");
    printf("+----------------------------------------+\n");
}

void set_shared_memory();
int main()
{
    int N;
    scanf("%d", &N);
    set_shared_memory(N);

    shared->N = N;
    sem_init(&shared->mutex, 1, 1);
    sem_init(&shared->empty, 1, N);
    sem_init(&shared->full, 1, 0);
    shared->i = 0;

    char name[11];
    printcommodities();
    while (1)
    {
        sem_wait(&shared->full);
        sem_wait(&shared->mutex);

        double price = buffer[shared->i - 1];
        strcpy(name, names[shared->i - 1]);
        shared->i -= 1;

        sem_post(&shared->mutex);
        sem_post(&shared->empty);
        for (int j = 0; name[j] != '\0'; j++)
        {
            name[j] = toupper(name[j]);
        }
        if (strcmp(name, "ALUMINIUM") == 0)
        {
            updateCommodity(price, GoldArr, &GoldIndex, &GoldCounter, name, 4);
        }
        else if (strcmp(name, "COPPER") == 0)
        {
            updateCommodity(price, SilverArr, &SilverIndex, &SilverCounter, name, 5);
        }
        else if (strcmp(name, "COTTON") == 0)
        {
            updateCommodity(price, CottonArr, &CottonIndex, &CottonCounter, name, 6);
        }
        else if (strcmp(name, "CRUDEOIL") == 0)
        {
            updateCommodity(price, CrudeOilArr, &CrudeOilIndex, &CrudeOilCounter, name, 7);
        }
        else if (strcmp(name, "GOLD") == 0)
        {
            updateCommodity(price, GoldArr, &GoldIndex, &GoldCounter, name, 8);
        }
        else if (strcmp(name, "LEAD") == 0)
        {
            updateCommodity(price, LeadArr, &LeadIndex, &LeadCounter, name, 9);
        }
        else if (strcmp(name, "MENTHAOIL") == 0)
        {
            updateCommodity(price, MenthaOilArr, &MenthaOilIndex, &MenthaOilCounter, name, 10);
        }
        else if (strcmp(name, "NATURALGAS") == 0)
        {
            updateCommodity(price, NaturalGasArr, &NaturalGasIndex, &NaturalGasCounter, name, 11);
        }
        else if (strcmp(name, "NICKEL") == 0)
        {
            updateCommodity(price, NickelArr, &NickelIndex, &NickelCounter, name, 12);
        }
        else if (strcmp(name, "SILVER") == 0)
        {
            updateCommodity(price, SilverArr, &SilverIndex, &SilverCounter, name, 13);
        }
        else if (strcmp(name, "ZINC") == 0)
        {
            updateCommodity(price, ZincArr, &ZincIndex, &ZincCounter, name, 14);
        }
        else
        {
            printf("Unknown commodity: %s\n", name);
        }

        printf("\033[16;0H");
    }

    return 0;
}

void set_shared_memory(int N)
{

    key_t key = ftok("shared_file.txt", 1);
    key_t key1 = ftok("buffer.txt", 2);
    key_t key2 = ftok("names.txt", 3);
    shmid = shmget(key, sizeof(shared_data), 0666 | IPC_CREAT);
    shmid1 = shmget(key1, N * sizeof(double), 0666 | IPC_CREAT);
    shmid2 = shmget(key2, N * 11, 0666 | IPC_CREAT);
    if (shmid == -1 || shmid1 == -1 || shmid2 == -1)
    {
        perror("shmget failed");
        exit(1);
    }
    shared = (shared_data *)shmat(shmid, NULL, 0);
    buffer = (double *)shmat(shmid1, NULL, 0);
    names = shmat(shmid2, NULL, 0);
    if (shared == (void *)-1 || buffer == (void *)-1 || names == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }
}

double calculateAverage(double arr[4])
{
    double sum = 0.00;
    for (int i = 0; i < 4; i++)
    {
        sum += arr[i];
    }
    return sum / 4;
}

void updateCommodity(double price, double arr[], int *index, int *counter, const char *name, int terminalRow)
{   (*counter)++;
    if (*counter < 4)
    {
        arr[*index] = price;
        *index = (*index + 1) % 4;
        
    }
    else
    {
        arr[*index] = price;

        printf("\033[%d;0H", terminalRow);

        if (price >= arr[(*index - 1 + 4) % 4])
        { // Handle negative index with circular buffer logic
            printf("| %-10s |\033[;32m  %7.2lf ↑ \033[0m|\033[;32m   %7.2lf  ↑ \033[0m|\n", name, price, calculateAverage(arr));
        }
        else
        {
            printf("| %-10s |\033[;31m  %7.2lf ↓ \033[0m|\033[;31m   %7.2lf  ↓ \033[0m|\n", name, price, calculateAverage(arr));
        }

        *index = (*index + 1) % 4;
    }
}