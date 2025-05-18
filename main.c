#include <iso646.h>
#include <stdio.h>
#include "process.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define max(x, y) ((x > y) ? x : y)
#define min(x, y) ((x < y) ? x : y)
#define MAX_LINE_WIDTH 20

struct Process *processes[SIZE];
struct Process *readyQueue[SIZE];
struct Process *waitingQueue[SIZE];

int PIDOccupation[SIZE]; // Each element is 0 or 1, indicating whether the PID of the index is taken
int availablePIDs[SIZE]; // Available list of PIDs
int processInMemory = 0; // Number of all processes in ready/waiting/running queue
int processWaiting = 0;
int processNumber;
int random;
int main(void) {
    config();

    printf("============\n");
    printf("%s\n", "FCFS");
    printf("============\n");

    int FCFSqueue[10 * SIZE][3];
    scheduleFCFS(FCFSqueue);

    int i = 0;
    while (FCFSqueue[i][0] != -2) i++;

    printf("\n");
    drawChart(FCFSqueue, i);
    printf("\n");

    struct Evaluation *FCFSevaluation = evaluateAlgorithm(FCFSqueue, i, processInMemory);

    printf("Average Turnaround : %.2lf\n", FCFSevaluation->averageTurnaroundTime);
    printf("Average Waiting : %.2lf\n", FCFSevaluation->averageWaitingTime);

    free(FCFSevaluation);

    return 0;
}

void config() {
    srand(time(NULL));

    for (int i = 0; i < SIZE; i++) {
        processes[i] = NULL;
        readyQueue[i] = NULL;
        waitingQueue[i] = NULL;
        availablePIDs[i] = i;
        PIDOccupation[i] = 0;
    }

    printf("Number of processes:");
    scanf("%d", &processNumber);


    printf("Random for 1, else 0:");
    scanf("%d", &random);
    createProcess(processNumber, random);
}

void createProcess(int processNumber, int random) {
    int scale = random ? 5 : 1;
    // Every figure is a multiple of {scale} (for readability)
    for (int i = 0; i < processNumber; i++) {
        printf("-- %dth Process Config --\n", i + 1);
        int PID;
        printf("PID :");
        if (random) {
            int index = rand() % (SIZE - processInMemory);
            PID = availablePIDs[index];

            // assign the last part of the list with the taken PIDs
            // the first part of the list with not yet taken PIDs (which may be taken afterward)
            int temp = availablePIDs[SIZE - processInMemory - 1];
            availablePIDs[SIZE - processInMemory - 1] = availablePIDs[index];
            availablePIDs[index] = temp;

            printf(" %d\n", PID);
        } else {
            while (1) {
                scanf("%d", &PID);
                if (!PIDOccupation[PID])
                    break;
                printf("Duplicated PID: %d, try again\nPID:", PID);
            }
            PIDOccupation[PID] = 1;
        }

        int ArrivalTime;
        printf("Arrival Time:");
        if (random) {
            ArrivalTime = (rand() % 50) * scale;
            printf(" %d\n", ArrivalTime);
        } else
            scanf("%d", &ArrivalTime);


        int CPUburstTime;
        printf("CPU burst Time:");
        if (random) {
            CPUburstTime = (rand() % 49) * scale + scale; // not to be 0
            printf(" %d\n", CPUburstTime);
        } else {
            while (1) {
                scanf("%d", &CPUburstTime);
                if (CPUburstTime > 0) break;
                printf("Invalid CPU Burst Time: %d, try again\nCPU burst Time:", CPUburstTime);
            }
        }

        int IOburstTimeNumber;
        printf("Number of IO burst Time:");

        int maxIOburstTimeNumber = CPUburstTime / scale - 1;
        if (maxIOburstTimeNumber > 5)
            maxIOburstTimeNumber = 5;

        if (random) {
            if (maxIOburstTimeNumber)
                IOburstTimeNumber = rand() % maxIOburstTimeNumber;
            else
                IOburstTimeNumber = 0;
            printf(" %d\n", IOburstTimeNumber);
        } else
            scanf("%d", &IOburstTimeNumber);

        int IOburstTime[SIZE][2];
        IOburstTime[IOburstTimeNumber][0] = -1; // -1 Indicates the end of the list
        if (IOburstTimeNumber != 0) {
            printf("IO Burst Point & IO Burst Time:\n");
            int *randomRequests = getNRandomNumbers(IOburstTimeNumber, scale, CPUburstTime - scale,
                                                    1, scale);
            int *randomTimes = getNRandomNumbers(IOburstTimeNumber, scale, 30 * scale, 0, scale);
            for (int j = 0; j < IOburstTimeNumber; j++) {
                if (random) {
                    IOburstTime[j][0] = randomRequests[j];
                    IOburstTime[j][1] = randomTimes[j];
                    printf("| %dth IO :", j + 1);
                    printf(" %d %d\n",
                           IOburstTime[j][0],
                           IOburstTime[j][1]);
                } else {
                    while (1) {
                        printf("| %dth IO :", j + 1);
                        scanf("%d %d",
                              &IOburstTime[j][0],
                              &IOburstTime[j][1]);
                        // not to be 0 values
                        if (IOburstTime[j][0] * IOburstTime[j][1]) break;
                        printf("Invalid IO Request Time or IO Burst time\ntry again:");
                    }
                }
            }
            free(randomRequests);
            free(randomTimes);
        }

        printf("Priority:");
        int priority;
        if (random) {
            priority = rand() % 20;
            printf(" %d\n", priority);
        } else
            scanf("%d", &priority);

        struct Process *currentProcess = malloc(sizeof(struct Process));
        currentProcess->PID = PID;
        currentProcess->arrivalTime = ArrivalTime;
        currentProcess->CPUburstTime = CPUburstTime;
        currentProcess->executedCPUburstTime = 0;
        currentProcess->priority = priority;

        memcpy(currentProcess->IOburstTime, IOburstTime, sizeof(IOburstTime));
        currentProcess->IOburstTimeNumber = IOburstTimeNumber;
        currentProcess->currentIOburstNumber = 0;
        currentProcess->remainingIOburstTime = -1;

        processes[i] = currentProcess;
        processInMemory++;
    }
}

void printQueue() {
    for (int i = 0; i < SIZE; i++) {
        if (processes[i] == NULL) break;
        struct Process currentProcess = *processes[i];
        printf("------------------------\n");
        printf("Process ID: %d\n", currentProcess.PID);
        printf("Arrival Time: %d\n", currentProcess.arrivalTime);
        printf("CPU Burst Time: %d\n", currentProcess.CPUburstTime);
        if (currentProcess.IOburstTime[0][0] != -1) {
            printf("IO Burst Point & IO Burst Time\n");
            for (int j = 0; j < SIZE; j++) {
                if (currentProcess.IOburstTime[j][0] < 0) break;
                printf("| %dth IO: %d %d\n", j + 1,
                       currentProcess.IOburstTime[j][0], currentProcess.IOburstTime[j][1]);
            }
        }
        printf("Priority: %d\n", currentProcess.priority);
    }
}

int *getNRandomNumbers(int N, int start, int end, int sorted, int scale) {
    // returns N distinct random numbers in [start, end]
    // unit is 5
    int len = (end - start) / scale + 1;
    int *arr = malloc(len * sizeof(int));
    for (int i = 0; i < len; i++) {
        arr[i] = start + i * scale;
    }

    int iter = rand() % 6 + 1;
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < N; j++) {
            int index = rand() % (len - 1);
            if (index >= j)
                index++;

            int temp = arr[j];
            arr[j] = arr[index];
            arr[index] = temp;
        }
    }

    int *result = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        result[i] = arr[i];
    }

    if (!sorted) {
        free(arr);
        return result;
    }

    // insertion sort
    for (int i = 1; i < N; i++) {
        int cur = result[i];
        for (int j = i - 1; j >= -1; j--) {
            if (result[j] > cur)
                result[j + 1] = result[j];
            else {
                result[j + 1] = cur;
                break;
            }
            if (j == -1)
                result[j + 1] = cur;
        }
    }

    free(arr);
    return result;
}

void schedule() {
}

void scheduleFCFS(int queue[][3]) {
    struct Process *processMinHeap[SIZE];
    for (int i = 0; i < processInMemory; i++) {
        insertMinHeap(processMinHeap, processes[i], i, ARRIVAL_TIME);
    }

    for (int i = 0; i < processInMemory; i++) {
        readyQueue[i] = popMinHeap(processMinHeap, processInMemory - i, ARRIVAL_TIME);
    }

    int runningProcesses = processInMemory;
    int PID;
    int end = 0;
    int i = 0;
    while (runningProcesses) {
        struct Process *peek = readyQueue[0];
        PID = peek->arrivalTime > end ? -1 : peek->PID;

        queue[i][2] = peek->arrivalTime;

        if (PID == -1) {
            // Idle process
            end = peek->arrivalTime;
        } else if (peek->IOburstTimeNumber) {
            // If the process requests I/O
            popMinHeap(readyQueue, runningProcesses--, ARRIVAL_TIME);
            if (peek->currentIOburstNumber >= peek->IOburstTimeNumber) {
                end += peek->CPUburstTime - peek->executedCPUburstTime;
            } else {
                int *currentIOburstInfo = peek->IOburstTime[peek->currentIOburstNumber++];
                end += currentIOburstInfo[0] - peek->executedCPUburstTime;
                peek->arrivalTime = end + currentIOburstInfo[1];
                peek->executedCPUburstTime = currentIOburstInfo[0];
                insertMinHeap(readyQueue, peek, runningProcesses++, ARRIVAL_TIME);
            }
        } else {
            // If the process does not request I/O
            popMinHeap(readyQueue, runningProcesses--, ARRIVAL_TIME);
            end += peek->CPUburstTime;
        }
        queue[i][0] = PID;
        queue[i][1] = end;

        i++;
    }
    queue[i][0] = -2;
}

struct Evaluation *evaluateAlgorithm(int info[][3], int len, int taskNumbers) {
    struct Evaluation *result = calloc(1, sizeof(struct Evaluation));
    int foundTurnaroundTime = 0;
    for (int i = len - 1; i >= 0; i--) {
        int PID = info[i][0];
        if (PID == -1) continue;
        if (result->turnaroundTime[PID]) continue;

        result->turnaroundTime[PID] = info[i][1];
        result->averageTurnaroundTime += info[i][1];
        foundTurnaroundTime++;
        if (foundTurnaroundTime == taskNumbers) break;
    }

    for (int i = 1; i < len; i++) {
        int currentTaskArrival = info[i][2];
        int previousTaskEnd = info[i - 1][1];
        if (info[i - 1][0] == -1) continue;
        int diff = previousTaskEnd - currentTaskArrival;
        result->waitingTime[info[i][0]] += diff > 0 ? diff : 0;
        result->averageWaitingTime += diff > 0 ? diff : 0;
    }

    result->averageWaitingTime /= taskNumbers;
    result->averageTurnaroundTime /= taskNumbers;
    return result;
}

void printEvaluation(struct Evaluation* evaluation, char *name) {

}

void insertMinHeap(struct Process **heapQueue, struct Process *process, int heapLen, int on) {
    heapQueue[heapLen] = process;
    int parent = heapLen - 1 >> 1;
    while (parent >= 0) {
        int swap = 0;
        switch (on) {
            case ARRIVAL_TIME:
                swap = heapQueue[heapLen]->arrivalTime < heapQueue[parent]->arrivalTime;
                break;
            case CPU_BURST_TIME:
                swap = heapQueue[heapLen]->CPUburstTime < heapQueue[parent]->CPUburstTime;
                break;
            case PRIORITY:
                swap = heapQueue[heapLen]->priority < heapQueue[parent]->priority;
                break;
            default:
                break;
        }
        if (swap) {
            struct Process *temp = heapQueue[parent];
            heapQueue[parent] = heapQueue[heapLen];
            heapQueue[heapLen] = temp;
            heapLen = parent;
            parent = parent - 1 >> 1;
        } else break;
    }
}

struct Process *popMinHeap(struct Process **heapQueue, int heapLen, int on) {
    struct Process *root = heapQueue[0];
    heapQueue[0] = heapQueue[--heapLen];
    int parent = 0;
    heapQueue[heapLen] = NULL;
    for (;;) {
        int l = (parent << 1) + 1;
        int r = (parent << 1) + 2;
        if (l > heapLen) break;
        if (r < heapLen) {
            if (heapQueue[l] == NULL && heapQueue[r] == NULL) break;
        } else if (heapQueue[l] == NULL) break;

        int swap = 0;
        int minChild;
        switch (on) {
            case ARRIVAL_TIME:
                if (heapQueue[l] == NULL) minChild = r;
                else if (heapQueue[r] == NULL) minChild = l;
                else minChild = (heapQueue[l]->arrivalTime < heapQueue[r]->arrivalTime) ? l : r;
                swap = heapQueue[parent]->arrivalTime > heapQueue[minChild]->arrivalTime;
            default:
                break;
            // TODO
        }

        if (swap) {
            struct Process *temp = heapQueue[parent];
            heapQueue[parent] = heapQueue[minChild];
            heapQueue[minChild] = temp;
            parent = minChild;
        } else break;
    }

    return root;
}

void drawChart(int info[][3], int taskNumber) {
    int line = 0;
    int stop = 0;
    int repeatPrevLine = 0;
    while (!stop) {
        for (int i = line * MAX_LINE_WIDTH; i < (line + 1) * MAX_LINE_WIDTH; i++) {
            if (i == taskNumber) break;
            int PID = info[i][0];
            if (PID == -1)
                printf("     ");
            else {
                int digit = 1;
                if (PID >= 10) digit = 2;
                if (PID >= 100) digit = 3;
                printf("%d", PID);
                for (int j = 0; j < 5 - digit; j++) {
                    printf("_");
                }
            }
        }

        printf("\n");

        printf("%-5d", repeatPrevLine);
        for (int i = line * MAX_LINE_WIDTH; i < (line + 1) * MAX_LINE_WIDTH; i++) {
            if (i == taskNumber) {
                stop = 1;
                break;
            };
            printf("%-5d", info[i][1]);
            if (i == (line + 1) * MAX_LINE_WIDTH - 1)
                repeatPrevLine = info[i][1];
        }

        line++;
        printf("\n\n");
    }
}

int getTimeScaleGCD(int timeInfo[][3], int len) {
    if (!len) return 0;

    int result = timeInfo[0][1];
    for (int i = 1; i < len; i++) {
        result = GCD(result, timeInfo[i][1]);
    }

    return result;
}

int GCD(int m, int n) {
    while (n) {
        int t = m % n;
        m = n;
        n = t;
    }
    return m;
}
