//
// Created by naruseon on 25. 4. 28.
//

#ifndef PROCESS_H
#define PROCESS_H
#define SIZE 100

enum ON {
    ARRIVAL_TIME,
    CPU_BURST_TIME,
    PRIORITY,
    REMAINING_IO_BURST_TIME
};

struct Process {
    int PID;
    int arrivalTime;
    int CPUburstTime;
    int executedCPUburstTime;
    int IOburstTime[SIZE][2];
    int IOburstTimeNumber;
    int currentIOburstNumber;
    int remainingIOburstTime;
    int priority;
} Process;

struct Evaluation {
    int turnaroundTime[SIZE];
    int waitingTime[SIZE];
    double averageTurnaroundTime;
    double averageWaitingTime;
} Evaluation;

void config();
void createProcess(int, int);

void schedule();

void scheduleFCFS();
void scheduleSJF();
void schedulePriority();
void scheduleRoundRobin();
void schedulePreemtiveFCFS();
void schedulePreemtiveSJF();

struct Evaluation *evaluateAlgorithm(int[][3], int, int);
void printEvaluation(struct Evaluation *, char *);

void insertMinHeap(struct Process **, struct Process*, int, int);
struct Process *popMinHeap(struct Process **, int, int);

void printQueue();
void drawChart(int[][3], int);

int *getNRandomNumbers(int, int, int, int, int);
int getTimeScaleGCD(int[][3], int);
int GCD(int, int);

#endif //PROCESS_H
