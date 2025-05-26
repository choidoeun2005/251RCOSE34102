#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "process.h"

#define max(x, y) ((x > y) ? x : y)
#define min(x, y) ((x < y) ? x : y)

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

// Creates process(es) by keyboard input or random numbers
// User can type '-1' any time to suspend current process creation.
void createProcess() {
    int processNumber;
    printf("Number of processes:");
    scanf("%d", &processNumber);

    int random;
    printf("Random for 1, else 0:");
    scanf("%d", &random);

    int scale = random ? 5 : 1;
    // Every figure is a multiple of {scale} in case of random (for readability)

    int currentProcessInMemory = processInMemory;

    for (int i = currentProcessInMemory; i < processNumber + currentProcessInMemory; i++) {
        printf("-- %dth Process Config --\n", i + 1);

        printf("PID (0 ~ 99) :");
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
                if (PID < 0 || PID >= SIZE) {
                    printf("PID should be in the range 0 to %d inclusive\n", SIZE - 1);
                    printf("PID :");
                    continue;
                }

                if (!PIDOccupation[PID])
                    break;
                // handle the case of duplicated PID
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


        int CPUBurstTime;
        printf("CPU Burst Time:");
        if (random) {
            CPUBurstTime = (rand() % 49) * scale + scale; // not to be 0
            printf(" %d\n", CPUBurstTime);
        } else {
            while (1) {
                scanf("%d", &CPUBurstTime);

                if (CPUBurstTime == -1) goto suspend;

                if (CPUBurstTime > 0) break;
                printf("Invalid CPU Burst Time: %d, try again\nCPU burst Time:", CPUBurstTime);
            }
        }

        int IOBurstTimeNumber;
        printf("Number of IO burst Time:");

        int maxIOBurstTimeNumber = CPUBurstTime / scale - 1;
        if (maxIOBurstTimeNumber > 5)
            maxIOBurstTimeNumber = 5;

        if (random) {
            if (maxIOBurstTimeNumber)
                IOBurstTimeNumber = rand() % maxIOBurstTimeNumber;
            else
                IOBurstTimeNumber = 0;
            printf(" %d\n", IOBurstTimeNumber);
        } else {
            while (1) {
                scanf("%d", &IOBurstTimeNumber);
                if (IOBurstTimeNumber <= maxIOBurstTimeNumber) break;
                printf("Invalid IO burst time number: %d\n", IOBurstTimeNumber);
                printf("Maximum available IO burst time number is: %d\n", maxIOBurstTimeNumber);
                printf("Number of IO burst Time:");
            }
        }

        if (IOBurstTimeNumber == -1) goto suspend;

        int IOBurstTime[6][2];
        IOBurstTime[IOBurstTimeNumber][0] = -1; // -1 Indicates the end of the list
        if (IOBurstTimeNumber != 0) {
            printf("IO Request Time & IO Burst Time:\n");
            int *randomRequests = getNRandomNumbers(IOBurstTimeNumber, scale, CPUBurstTime - scale,
                                                    1, scale);
            int *randomTimes = getNRandomNumbers(IOBurstTimeNumber, scale, 30 * scale, 0, scale);

            int IORequestPoints[5];
            int len = 0;

            for (int j = 0; j < IOBurstTimeNumber; j++) {
                if (random) {
                    IOBurstTime[j][0] = randomRequests[j];
                    IOBurstTime[j][1] = randomTimes[j];
                    printf("| %dth IO :", j + 1);
                    printf(" %d %d\n",
                           IOBurstTime[j][0],
                           IOBurstTime[j][1]);
                } else {
                    // User input
                    while (1) {
                        int duplicateFlag = 0;
                        printf("| %dth IO :", j + 1);
                        scanf("%d", &IOBurstTime[j][0]);
                        if (IOBurstTime[j][0] == -1) goto suspend;
                        scanf("%d", &IOBurstTime[j][1]);
                        if (IOBurstTime[j][1] == -1) goto suspend;

                        // IO Requesting time not to be duplicated
                        for (int k = 0; k < len; k++) {
                            if (IORequestPoints[k] == IOBurstTime[j][0]) {
                                printf("Duplicated IO Request Time : %d. try again\n",
                                       IOBurstTime[j][0]);
                                duplicateFlag = 1;
                                break;
                            }
                        }

                        if (duplicateFlag) continue;

                        if (IOBurstTime[j][0] >= CPUBurstTime) {
                            printf(
                                "IO request time must be smaller than CPU burst time. try again\n");
                            continue;
                        }

                        // not to be 0 or negative values
                        if (IOBurstTime[j][0] > 0 && IOBurstTime[j][1] > 0) {
                            IORequestPoints[len++] = IOBurstTime[j][0];
                            break;
                        }
                        printf("Values must be positive. try again\n");
                    }
                }
            }

            if (!random) {
                // insertion sort of IOburstTime array
                for (int k = 1; k < IOBurstTimeNumber; k++) {
                    int temp0 = IOBurstTime[k][0];
                    int temp1 = IOBurstTime[k][1];
                    int insertPos = k - 1;

                    while (insertPos >= 0 && IOBurstTime[insertPos][0] > temp0) {
                        insertPos--;
                    }

                    for (int l = k; l > insertPos + 1; l--) {
                        IOBurstTime[l][0] = IOBurstTime[l - 1][0];
                        IOBurstTime[l][1] = IOBurstTime[l - 1][1];
                    }

                    IOBurstTime[insertPos + 1][0] = temp0;
                    IOBurstTime[insertPos + 1][1] = temp1;
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
        currentProcess->CPUBurstTime = CPUBurstTime;
        currentProcess->executedCPUBurstTime = 0;
        currentProcess->priority = priority;

        memcpy(currentProcess->IOBurstTime, IOBurstTime, sizeof(IOBurstTime));
        currentProcess->IOBurstTimeNumber = IOBurstTimeNumber;
        currentProcess->currentIOBurstNumber = 0;

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

    int maxIONum = 0;
    for (int i = 0; i < processInMemory; i++) {
        maxIONum = max(maxIONum, processes[i]->IOBurstTimeNumber);
    }

    printf("[Processes in memory]\n");

    // {PID, AT, CBT, IO, Priority} columns in order
    int width[] = {5, 5, 5, max(20, maxIONum * 15), 5};
    int col = 5;

    printBorder(width, col);
    printf("| %-*s | %-*s | %-*s | %-*s | %-*s |\n",
           width[0] - 2, "PID",
           width[1] - 2, "AT",
           width[2] - 2, "CBT",
           width[3] - 2, "I/O Burst Times",
           width[4] - 2, "Pri");
    printBorder(width, col);

    for (int i = 0; i < processInMemory; i++) {
        char IOBurstTimeStr[width[3] + 1];
        IOBurstTimeStr[0] = '\0';

        for (int j = 0; j < processes[i]->IOBurstTimeNumber; j++) {
            char buf[12];
            snprintf(buf, sizeof(buf), "(%d,%d)",
                     processes[i]->IOBurstTime[j][0],
                     processes[i]->IOBurstTime[j][1]);

            if (j != 0) {
                strncat(IOBurstTimeStr, ", ", width[3] - strlen(IOBurstTimeStr) - 1);
            }
            strncat(IOBurstTimeStr, buf, width[3] - strlen(IOBurstTimeStr) - 1);
        }

        printf("| %-*d | %-*d | %-*d | %-*s | %-*d |\n",
               width[0] - 2, processes[i]->PID,
               width[1] - 2, processes[i]->arrivalTime,
               width[2] - 2, processes[i]->CPUBurstTime,
               width[3] - 2, IOBurstTimeStr,
               width[4] - 2, processes[i]->priority);
    }

    printBorder(width, col);
}

void printBorder(int width[], int col) {
    printf("+");
    for (int i = 0; i < col; i++) {
        for (int j = 0; j < width[i]; j++) {
            printf("-");
        }
        printf("+");
    }
    printf("\n");
}


void drawChart(int info[][3], int taskNumber) {
    printf("[Gantt Chart]\n\n");
    int line = 0;
    int stop = 0;
    int repeatPrevLine = 0;
    while (!stop) {
        for (int i = line * MAX_LINE_WIDTH; i < (line + 1) * MAX_LINE_WIDTH; i++) {
            int PID = info[i][0];
            if (PID == -1) {
                for (int j = 0; j < 6; j++)
                    printf(" ");
            } else {
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
            case NEXT_CPU_BURST_TIME:
                swap = nextCPUBurstTime(childProcess) < nextCPUBurstTime(parentProcess);
                break;
            case PRIORITY:
                swap = heapQueue[heapLen]->priority < heapQueue[parent]->priority;
                break;
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
                else
                    minChild = ((r < heapLen) &&
                                heapQueue[l]->arrivalTime > heapQueue[r]->arrivalTime)
                                   ? r
                                   : l;
                swap = heapQueue[parent]->arrivalTime > heapQueue[minChild]->arrivalTime;
                break;
            case NEXT_CPU_BURST_TIME:
                if (r < heapLen && heapQueue[r] == NULL) minChild = l;
                else
                    minChild = ((r < heapLen) &&
                                nextCPUBurstTime(heapQueue[l]) > nextCPUBurstTime(heapQueue[r]))
                                   ? r
                                   : l;
                swap = nextCPUBurstTime(heapQueue[parent]) > nextCPUBurstTime(heapQueue[minChild]);
                break;
            case PRIORITY:
                if (r < heapLen && heapQueue[r] == NULL) minChild = l;
                else
                    minChild = ((r < heapLen) &&
                                heapQueue[l]->priority > heapQueue[r]->priority)
                                   ? r
                                   : l;
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

void insertArrayAndSort(struct Process **processArr, struct Process *process, int arrLen,
                        int mode) {
    switch (mode) {
        case ARRIVAL_TIME:
            int currentArrival = process->arrivalTime;
            int l = 0;
            int r = arrLen;
            int mid = l + r >> 1;

            while (l < r) {
                int midArrival = processArr[mid]->arrivalTime;
                if (midArrival < currentArrival) l = mid + 1;
                else if (midArrival > currentArrival) r = mid;
                else break;
                mid = l + r >> 1;
            }

            for (int i = arrLen; i > mid; i--) {
                processArr[i] = processArr[i - 1];
            }
            processArr[mid] = process;
            break;
        case PRIORITY:
            int currentPriority = process->priority;
            l = 0;
            r = arrLen;
            mid = l + r >> 1;
            while (l < r) {
                int midPriority = processArr[mid]->priority;
                if (midPriority < currentPriority) l = mid + 1;
                else if (midPriority > currentPriority) r = mid;
                else break;
                mid = l + r >> 1;
            }

            for (int i = arrLen; i > mid; i--) {
                processArr[i] = processArr[i - 1];
            }
            processArr[mid] = process;
            break;
        case INSERT_TO_LAST:
            processArr[arrLen] = process;
            break;
        case INSERT_TO_FIRST:
            for (int i = arrLen; i > 0; i--) {
                processArr[i] = processArr[i - 1];
            }
            processArr[0] = process;
            break;
        default:
            break;
    }
}

struct Process *popArray(struct Process **processArr, int index, int arrLen) {
    struct Process *result = processArr[index];
    for (int i = index; i < arrLen - 1; i++) {
        processArr[i] = processArr[i + 1];
    }
    return result;
}

int nextCPUBurstTime(struct Process *p) {
    int result;
    if (p->IOBurstTimeNumber == p->currentIOBurstNumber) {
        // Process which has done all its I/O's
        result = p->CPUBurstTime - p->executedCPUBurstTime;
    } else {
        result = p->IOBurstTime[p->currentIOBurstNumber][0]
                 - p->executedCPUBurstTime;
    }

    return result;
}
