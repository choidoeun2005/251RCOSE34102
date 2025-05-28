#ifndef PRIORITY_H
#define PRIORITY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "process.h"
#include "util.h"

void schedulePriority(int queue[][3]);

struct Evaluation *evalPriority();

void printPriority() {
    printf("============\n");
    printf("%s\n", "Priority");
    printf("============\n");

    int queue[10 * SIZE][3];
    schedulePriority(queue);
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

struct Evaluation *evalPriority() {
    int queue[10 * SIZE][3];
    schedulePriority(queue);

    int len = 0;
    while (queue[len][0] != -2) len++;

    struct Evaluation *eval = evaluateAlgorithm(queue, len, processInMemory);
    return eval;
}

void schedulePriority(int queue[][3]) {
    // readyQueue ==> heap
    int runningProcesses = processInMemory;
    int waitingQueueLen = 0;
    int readyQueueLen = 0;
    int topArrivalTime = 0;
    int end = 0;
    int cursor = 0;
    int PID = -1;
    int i = 0;

    while (runningProcesses) {
        while (cursor < processInMemory) {
            if (processes[cursor]->arrivalTime <= end) {
                insertMinHeap(readyQueue, processes[cursor++], readyQueueLen++,PRIORITY);
            } else break;
        }

        if (waitingQueueLen) {
            while (waitingQueueLen && waitingQueue[0]->arrivalTime <= end) {
                insertMinHeap(readyQueue,
                              popMinHeap(waitingQueue, waitingQueueLen--, ARRIVAL_TIME),
                              readyQueueLen++, PRIORITY);
            }
        }

        if (!readyQueueLen) {
            PID = -1;
            if (cursor < processInMemory) {
                if (waitingQueueLen)
                    end = min(processes[cursor]->arrivalTime, waitingQueue[0]->arrivalTime);
                else
                    end = processes[cursor]->arrivalTime;
            } else end = waitingQueue[0]->arrivalTime;
            goto parse_next_task;
        }

        struct Process *top = malloc(sizeof(struct Process));
        memcpy(top, readyQueue[0], sizeof(struct Process));
        popMinHeap(readyQueue, readyQueueLen--, PRIORITY);

        PID = top->PID;
        topArrivalTime = top->arrivalTime;

        end += nextCPUBurstTime(top);

        if (top->currentIOBurstNumber >= top->IOBurstTimeNumber) {
            runningProcesses--;
        } else {
            int *currentIOBurstInfo = top->IOBurstTime[top->currentIOBurstNumber++];
            top->arrivalTime = end + currentIOBurstInfo[1];
            top->executedCPUBurstTime = currentIOBurstInfo[0];
            insertMinHeap(waitingQueue, top, waitingQueueLen++, ARRIVAL_TIME);
        }


    parse_next_task:
        queue[i][0] = PID;
        queue[i][1] = end;
        queue[i][2] = topArrivalTime;
        i++;
    }
    queue[i][0] = -2;
}

#endif //PRIORITY_H
