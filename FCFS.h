#ifndef FCFS_H
#define FCFS_H

#include <stdlib.h>
#include <string.h>

#include "process.h"

void scheduleFCFS(int queue[][3]);

void printFCFS() {
    printf("============\n");
    printf("%s\n", "FCFS");
    printf("============\n");

    int queue[10 * SIZE][3];
    scheduleFCFS(queue);
    int i = 0;
    while (queue[i][0] != -2) i++;

    printf("\n");
    drawChart(queue, i);
    printf("\n");

    struct Evaluation *eval = evaluateAlgorithm(queue, i, processInMemory);
    printf("Average Turnaround : %.2lf\n", eval->averageTurnaroundTime);
    printf("Average Waiting : %.2lf\n", eval->averageWaitingTime);

    free(eval);

    printf("\n");
}

void scheduleFCFS(int queue[][3]) {
    memcpy(readyQueue, processes, sizeof(processes));

    int runningProcesses = processInMemory;
    int PID;
    int end = 0;
    int i = 0;
    while (runningProcesses) {
        struct Process *peek = malloc(sizeof(struct Process));
        memcpy(peek, readyQueue[0], sizeof(struct Process));

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



#endif //FCFS_H
