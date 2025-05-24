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
    SORT_SJF,
    PRIORITY,
    REARRIVAL_TIME
};

struct Process {
    int PID;
    int arrivalTime;
    int CPUburstTime;
    int executedCPUburstTime;
    int IOburstTime[SIZE][2];
    int IOburstTimeNumber;
    int currentIOburstNumber;
    int rearrivalTime;
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

void drawChart(int info[][3], int taskNumber);

int nextCPUburstTime(struct Process *p);

void insertMinHeap(struct Process **heapQueue, struct Process *process, int heapLen, int mode);

struct Process *popMinHeap(struct Process **heapQueue, int heapLen, int mode);

int *getNRandomNumbers(int N, int start, int end, int sorted, int scale);


#endif //PROCESS_H
