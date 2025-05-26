#ifndef PROCESS_H
#define PROCESS_H

#define SIZE 100
#define MAX_LINE_WIDTH 16

extern struct Process *processes[SIZE];
extern struct Process *readyQueue[SIZE];
extern struct Process *waitingQueue[SIZE];

extern int PIDOccupation[SIZE];
// Each element is 0 or 1, indicating whether the PID of the index is taken
extern int availablePIDs[SIZE]; // Available list of PIDs
extern int arrivalTimes[SIZE]; // Arrival Times of all processes

extern int processInMemory; // Number of all processes in ready/waiting/running queue
extern int processWaiting;

enum MODE {
    ARRIVAL_TIME,
    NEXT_CPU_BURST_TIME,
    PRIORITY,
    INSERT_TO_LAST,
    INSERT_TO_FIRST
};

struct Process {
    int PID;
    int arrivalTime;
    int CPUBurstTime;
    int executedCPUBurstTime;
    int IOBurstTime[6][2];
    int IOBurstTimeNumber;
    int currentIOBurstNumber;
    int priority;
};

struct Evaluation {
    int turnaroundTime[SIZE];
    int waitingTime[SIZE];
    double averageTurnaroundTime;
    double averageWaitingTime;
};

void config();

void reset();

void createProcess();

int removeProcess(int PID);

void freePID(int PID);

struct Evaluation *evaluateAlgorithm(int info[][3], int len, int taskNumbers);

void sortProcesses(int mode);

void printQueue();

void printBorder(int width[], int col);

void drawChart(int info[][3], int taskNumber);

int nextCPUBurstTime(struct Process *p);

void insertMinHeap(struct Process **heapQueue, struct Process *process, int heapLen, int mode);

struct Process *popMinHeap(struct Process **heapQueue, int heapLen, int mode);

void insertArrayAndSort(struct Process **processArr, struct Process *process, int arrLen, int mode);

struct Process *popArray(struct Process **processArr, int index, int arrLen);

int *getNRandomNumbers(int N, int start, int end, int sorted, int scale);


#endif //PROCESS_H

