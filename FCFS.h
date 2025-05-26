#ifndef FCFS_H
#define FCFS_H

#include <stdlib.h>
#include <string.h>

#include "process.h"

struct Evaluation *evalFCFS();

void scheduleFCFS(int queue[][3]);

void printFCFS() {
    printf("============\n");
    printf("%s\n", "FCFS");
    printf("============\n");

    int queue[10 * SIZE][3];
    scheduleFCFS(queue);
    int len = 0;
    while (queue[len][0] != -2) len++;

    printf("\n");
    drawChart(queue, len);
    printf("\n");

    struct Evaluation *eval = evaluateAlgorithm(queue, len, processInMemory);
    printf("Average Turnaround : %.2lf\n", eval->averageTurnaroundTime);
    printf("Average Waiting : %.2lf\n", eval->averageWaitingTime);

    free(eval);

    printf("\n");
}

struct Evaluation *evalFCFS() {
    int queue[10 * SIZE][3];
    scheduleFCFS(queue);

    int len = 0;
    while (queue[len][0] != -2) len++;

    struct Evaluation *eval = evaluateAlgorithm(queue, len, processInMemory);
    return eval;
}

void scheduleFCFS(int queue[][3]) {
    int runningProcesses = processInMemory;
    int readyQueueLen = processInMemory;
    int PID;
    int end = 0;
    int topArrivalTime = 0;
    int i = 0;

    memcpy(readyQueue, processes, sizeof(processes));

    while (runningProcesses) {
        struct Process *top = malloc(sizeof(struct Process));
        memcpy(top, readyQueue[0], sizeof(struct Process));


        PID = top->arrivalTime > end ? -1 : top->PID;
        topArrivalTime = top->arrivalTime;

        if (PID == -1) {
            // Idle process
            end = top->arrivalTime;
            goto parse_next_task;
        }

        popMinHeap(readyQueue, readyQueueLen--, ARRIVAL_TIME);

        end += nextCPUBurstTime(top);

        if (top->currentIOBurstNumber >= top->IOBurstTimeNumber) {
            // Process has finished all its I/O
            runningProcesses--;
        } else {
            // Process has some I/O remaining
            int *currentIOBurstInfo = top->IOBurstTime[top->currentIOBurstNumber++];
            top->arrivalTime = end + currentIOBurstInfo[1];
            top->executedCPUBurstTime = currentIOBurstInfo[0];
            insertMinHeap(readyQueue, top, readyQueueLen++, ARRIVAL_TIME);
        }

    parse_next_task:
        queue[i][0] = PID;
        queue[i][1] = end;
        queue[i][2] = topArrivalTime;

        i++;
    }
    queue[i][0] = -2;
}


#endif //FCFS_H
