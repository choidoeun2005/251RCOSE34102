#ifndef RR_H
#define RR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"
#include "util.h"

int scheduleRR(int queue[][3], int timeQuantum);

struct Evaluation *evalRR();

void printRR(int timeQuantum) {
    printf("============\n");
    printf("RR (T = %d)\n", timeQuantum);
    printf("============\n");


    int queue[10 * SIZE][3];
    int result = scheduleRR(queue, timeQuantum);
    if (result < 0) {
        printf("Too much processes or too small time quantum to simulate RR!\n");
        printf("Try reducing time quantum or the number of processes.\n");
        return;
    }
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

struct Evaluation *evalRR(int timeQuantum) {
    int queue[10 * SIZE][3];
    int result = scheduleRR(queue, timeQuantum);

    struct Evaluation *eval;
    if (result < 0) { // Exception handle
        printf("Too much processes or too small time quantum to simulate RR!\n");
        printf("Try reducing time quantum or the number of processes.\n");
        eval = malloc(sizeof(struct Evaluation));
        eval->averageWaitingTime = -1;
        eval->averageTurnaroundTime = -1;
        return eval;
    }

    int len = 0;
    while (queue[len][0] != -2) len++;

    eval = evaluateAlgorithm(queue, len, processInMemory);
    return eval;
}

int scheduleRR(int queue[][3], int timeQuantum) {
    int runningProcesses = processInMemory;
    int waitingQueueLen = 0;
    int readyQueueLen = 0;
    int cursor = 0;
    int topArrivalTime = 0;
    int end = 0;
    int PID = -1;
    int i = 0;

    while (cursor < processInMemory && processes[cursor]->arrivalTime == 0) {
        readyQueue[readyQueueLen++] = processes[cursor++];
    }

    while (runningProcesses) {
        int currentProcessDone = 0;
        int requestsIO = 0;

        if (!readyQueueLen) {
            PID = -1;

            if (cursor < processInMemory) {
                if (waitingQueueLen) {
                    end = min(waitingQueue[0]->arrivalTime, processes[cursor]->arrivalTime);
                } else end = processes[cursor]->arrivalTime;
            } else end = waitingQueue[0]->arrivalTime;

            topArrivalTime = -1;

            goto parse_next_task;
        }

        struct Process *top = malloc(sizeof(struct Process));
        memcpy(top, readyQueue[0], sizeof(struct Process));
        popArray(readyQueue, 0, readyQueueLen--);

        PID = top->PID;
        topArrivalTime = top->arrivalTime;
        int topNextCPUBurstTime = nextCPUBurstTime(top);

        if (topNextCPUBurstTime > timeQuantum) {
            top->executedCPUBurstTime += timeQuantum;
            end += timeQuantum;
            top->arrivalTime = end;

            goto parse_next_task;
        }

        end += topNextCPUBurstTime;

        if (top->currentIOBurstNumber >= top->IOBurstTimeNumber) {
            currentProcessDone = 1;
            runningProcesses--;
        } else {
            int *currentIOBurstInfo = top->IOBurstTime[top->currentIOBurstNumber++];
            top->arrivalTime = end + currentIOBurstInfo[1];
            top->executedCPUBurstTime = currentIOBurstInfo[0];
            requestsIO = 1;
        }

    parse_next_task:
        int selectFromWaiting = 0;

        // Add all the processes arrived during the current burst time to the ready queue
        while (waitingQueueLen || cursor < processInMemory) {
            if (cursor < processInMemory) {
                if (waitingQueueLen) {
                    selectFromWaiting = (
                        waitingQueue[0]->arrivalTime <= processes[cursor]->arrivalTime);
                } else selectFromWaiting = 0;
            } else selectFromWaiting = 1;

            if (selectFromWaiting) {
                if (waitingQueue[0]->arrivalTime > end) break;
                readyQueue[readyQueueLen++] = waitingQueue[0];
                popMinHeap(waitingQueue, waitingQueueLen--, ARRIVAL_TIME);
            } else {
                // select from Struct Process *processes[]
                if (processes[cursor]->arrivalTime > end) break;
                readyQueue[readyQueueLen++] = processes[cursor++];
            }
        }

        if (requestsIO) {
            insertMinHeap(waitingQueue, top, waitingQueueLen++, ARRIVAL_TIME);
        } else if (!currentProcessDone && PID != -1) {
            readyQueue[readyQueueLen++] = top;
        }

        if (i == SIZE * 10) {
            return -1;
        }

        queue[i][0] = PID;
        queue[i][1] = end;
        queue[i][2] = topArrivalTime;
        // printf("%d %d %d\n", PID, end, topArrivalTime);
        i++;
    }

    queue[i][0] = -2;
    return 0;
}

#endif //RR_H
