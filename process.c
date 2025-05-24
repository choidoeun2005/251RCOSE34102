#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "process.h"

struct Process *processes[SIZE];
struct Process *readyQueue[SIZE];
struct Process *waitingQueue[SIZE];

int PIDOccupation[SIZE]; // Each element is 0 or 1, indicating whether the PID of the index is taken
int availablePIDs[SIZE]; // Available list of PIDs
int arrivalTimes[SIZE]; // Arrival Times of all processes

int processInMemory = 0; // Number of all processes in ready/waiting/running queue
int processWaiting = 0;

int PID;

void config() {
    processInMemory = 0;
    srand(time(NULL));

    for (int i = 0; i < SIZE; i++) {
        processes[i] = NULL;
        readyQueue[i] = NULL;
        waitingQueue[i] = NULL;
        availablePIDs[i] = i;
        PIDOccupation[i] = 0;
    }
}

void reset() {
    for (int i = 0; i < SIZE; i++) {
        readyQueue[i] = NULL;
        waitingQueue[i] = NULL;
    }
}

void createProcess() {
    int processNumber;
    printf("Number of processes:");
    scanf("%d", &processNumber);

    int random;
    printf("Random for 1, else 0:");
    scanf("%d", &random);

    int scale = random ? 5 : 1;
    // Every figure is a multiple of {scale} (for readability)

    int currentProcessInMemory = processInMemory;

    for (int i = currentProcessInMemory; i < processNumber + currentProcessInMemory; i++) {
        printf("-- %dth Process Config --\n", i + 1);

        printf("PID :");
        if (random) {
            int index = rand() % (SIZE - processInMemory);
            PID = availablePIDs[index];
            // assign the last part of the list with the taken PIDs
            // the first part of the list with not yet taken PIDs (which may be taken afterward)
            int temp = availablePIDs[SIZE - processInMemory - 1];
            availablePIDs[SIZE - processInMemory - 1] = availablePIDs[index];
            availablePIDs[index] = temp;
            PIDOccupation[PID] = 1;

            printf("%d\n", PID);
        } else {
            while (1) {
                scanf(" %d", &PID);
                if (PID == -1) goto suspend;
                if (!PIDOccupation[PID])
                    break;
                printf("Duplicated PID: %d, try again\n", PID);
                printf("Occupied PIDs: ");
                for (int p = 0; p < SIZE; p++) {
                    if (PIDOccupation[p]) {
                        printf("%d ", p);
                    }
                }
                printf("\n");
                printf("PID :");
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

        if (ArrivalTime == -1) goto suspend;

        arrivalTimes[PID] = ArrivalTime;


        int CPUburstTime;
        printf("CPU burst Time:");
        if (random) {
            CPUburstTime = (rand() % 49) * scale + scale; // not to be 0
            printf(" %d\n", CPUburstTime);
        } else {
            while (1) {
                scanf("%d", &CPUburstTime);

                if (ArrivalTime == -1) goto suspend;

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

        if (IOburstTimeNumber == -1)  goto suspend;

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

                        if (IOburstTime[j][0] == -1 || IOburstTime[j][1] == -1) goto suspend;

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

        if (priority == -1) goto suspend;

        struct Process *currentProcess = malloc(sizeof(struct Process));
        currentProcess->PID = PID;
        currentProcess->arrivalTime = ArrivalTime;
        currentProcess->CPUburstTime = CPUburstTime;
        currentProcess->executedCPUburstTime = 0;
        currentProcess->priority = priority;

        memcpy(currentProcess->IOburstTime, IOburstTime, sizeof(IOburstTime));
        currentProcess->IOburstTimeNumber = IOburstTimeNumber;
        currentProcess->currentIOburstNumber = 0;
        currentProcess->rearrivalTime = -1;

        processes[i] = currentProcess;
        processInMemory++;
    }
    return;

    suspend:
        if (PID >= 0) freePID(PID);
        printf("suspending...\n");
}

int removeProcess(int PID) {
    int removeTargetIndex = 0;
    int found = 0;
    for (; removeTargetIndex < processInMemory; removeTargetIndex++) {
        if (processes[removeTargetIndex]->PID == PID) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return -1;
    }

    for (int j = removeTargetIndex; j < SIZE - 1; j++) {
        processes[j] = processes[j + 1];
    }

    freePID(PID);

    processInMemory--;

    return 0;
}

void freePID(int PID) {
    PIDOccupation[PID] = 0;

    int removedPIDIndex = 0;
    for (; removedPIDIndex < SIZE; removedPIDIndex++) {
        if (availablePIDs[removedPIDIndex] == PID) break;
    }

    int temp = availablePIDs[SIZE - processInMemory];
    availablePIDs[SIZE - processInMemory] = availablePIDs[removedPIDIndex];
    availablePIDs[removedPIDIndex] = temp;
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

        result->turnaroundTime[PID] -= arrivalTimes[PID];
        result->averageTurnaroundTime -= arrivalTimes[PID];
        foundTurnaroundTime++;
        if (foundTurnaroundTime == taskNumbers) break;
    }

    for (int i = 1; i < len; i++) {
        int currentTaskArrival = info[i][2];
        int previousTaskEnd = info[i - 1][1];
        if (info[i - 1][0] == -1 || info[i][0] == -1) continue;

        int diff = previousTaskEnd - currentTaskArrival;
        result->waitingTime[info[i][0]] += diff > 0 ? diff : 0;
        result->averageWaitingTime += diff > 0 ? diff : 0;
    }

    result->averageWaitingTime /= taskNumbers;
    result->averageTurnaroundTime /= taskNumbers;
    return result;
}

void sortProcesses(int mode) {
    struct Process *processMinHeap[SIZE];

    for (int i = 0; i < processInMemory; i++) {
        insertMinHeap(processMinHeap, processes[i], i, mode);
    }

    for (int i = 0; i < processInMemory; i++) {
        processes[i] = popMinHeap(processMinHeap, processInMemory - i, mode);
    }
}

void printQueue() {
    if (processInMemory == 0) {
        printf("No process in memory.\n");
        return;
    }

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

void drawChart(int info[][3], int taskNumber) {
    int line = 0;
    int stop = 0;
    int repeatPrevLine = 0;
    while (!stop) {
        for (int i = line * MAX_LINE_WIDTH; i < (line + 1) * MAX_LINE_WIDTH; i++) {
            int PID = info[i][0];
            if (PID == -1) {
                for (int j = 0; j < 6; j++)
                    printf(" ");
            }
            else {
                int digit = 1;
                if (PID >= 10) digit = 2;
                if (PID >= 100) digit = 3;
                printf("%d", PID);
                for (int j = 0; j < 6 - digit; j++) {
                    printf("_");
                }
            }

            if (i + 1 == taskNumber) {
                stop = 1;
                break;
            }
        }

        printf("\n");

        printf("%-6d", repeatPrevLine);

        for (int i = line * MAX_LINE_WIDTH; i < (line + 1) * MAX_LINE_WIDTH; i++) {
            printf("%-6d", info[i][1]);
            if (i == (line + 1) * MAX_LINE_WIDTH - 1)
                repeatPrevLine = info[i][1];

            if (i + 1 == taskNumber) break;
        }

        line++;
        if (!stop)
            printf("\n\n");
    }
    printf("\n");
}

void insertMinHeap(struct Process **heapQueue, struct Process *process, int heapLen, int mode) {
    heapQueue[heapLen] = process;
    int parent = heapLen - 1 >> 1;
    while (parent >= 0) {
        int swap = 0;
        struct Process *childProcess = heapQueue[heapLen];
        struct Process *parentProcess = heapQueue[parent];
        switch (mode) {
            case ARRIVAL_TIME:
                swap = childProcess->arrivalTime < parentProcess->arrivalTime;
                break;
            case SORT_SJF:
                swap = nextCPUburstTime(childProcess) < nextCPUburstTime(parentProcess);
                break;
            case PRIORITY:
                swap = heapQueue[heapLen]->priority < heapQueue[parent]->priority;
                break;
            case REARRIVAL_TIME:
                swap = heapQueue[heapLen]->rearrivalTime < heapQueue[parent]->rearrivalTime;
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

struct Process *popMinHeap(struct Process **heapQueue, int heapLen, int mode) {
    struct Process *root = heapQueue[0];
    heapQueue[0] = heapQueue[--heapLen];
    int parent = 0;
    heapQueue[heapLen] = NULL;
    for (;;) {
        int l = (parent << 1) + 1;
        int r = (parent << 1) + 2;
        if (r < heapLen) {
            if (heapQueue[l] == NULL && heapQueue[r] == NULL) break;
        } else if (l < heapLen && heapQueue[l] == NULL) break;

        if (l >= heapLen) break;

        int swap = 0;
        int minChild;
        switch (mode) {
            case ARRIVAL_TIME:
                if (r < heapLen && heapQueue[r] == NULL) minChild = l;
                else minChild = ((r < heapLen) &&
                    heapQueue[l]->arrivalTime > heapQueue[r]->arrivalTime) ? r : l;
                swap = heapQueue[parent]->arrivalTime > heapQueue[minChild]->arrivalTime;
                break;
            case SORT_SJF:
                if (r < heapLen && heapQueue[r] == NULL) minChild = l;
                else minChild = ((r < heapLen) &&
                    nextCPUburstTime(heapQueue[l]) > nextCPUburstTime(heapQueue[r])) ? r : l;
                swap = nextCPUburstTime(heapQueue[parent]) > nextCPUburstTime(heapQueue[minChild]);
                break;
            case PRIORITY:
                if (r < heapLen && heapQueue[r] == NULL) minChild = l;
                else minChild = ((r < heapLen) &&
                heapQueue[l]->priority > heapQueue[r]->priority) ? r : l;
                swap = heapQueue[parent]->priority > heapQueue[minChild]->priority;
            default:
                break;
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

int nextCPUburstTime(struct Process *p) {
    int result;
    if (p->IOburstTimeNumber == 0) {
        // Process with no I/O
        result = p->CPUburstTime;
    } else if (p->IOburstTimeNumber == p->currentIOburstNumber) {
        // Process which has done all its I/O's
        result = p->CPUburstTime - p->executedCPUburstTime;
    } else {
        int currentIOburstNumber = p->currentIOburstNumber;
        if (currentIOburstNumber == 0) {
            result = p->IOburstTime[0][0];
        } else {
            result = p->IOburstTime[currentIOburstNumber][0]
                     - p->IOburstTime[currentIOburstNumber - 1][0];
        }
    }

    return result;
}
