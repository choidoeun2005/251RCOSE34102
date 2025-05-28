#ifndef SJF_H
#define SJF_H

#include <stdio.h>
#include <string.h>

#include "process.h"
#include "util.h"

void scheduleSJF(int queue[][3]);

struct Evaluation *evalSJF();

void printSJF() {
    printf("============\n");
    printf("%s\n", "SJF");
    printf("============\n");


    int queue[10 * SIZE][3];
    scheduleSJF(queue);
    int i = 0;
    while (queue[i][0] != -2) i++;

    printf("\n");
    drawChart(queue, i);
    printf("\n");

    struct Evaluation *eval = evaluateAlgorithm(queue, i, processInMemory);
    printf("Average Turnaround : %.2lf\n", eval->averageTurnaroundTime);
    printf("Average Waiting : %.2lf\n", eval->averageWaitingTime);
    printf("Maximum Waiting : %.2lf\n", eval->maxWaitingTime);

    free(eval);

    printf("\n");
}

struct Evaluation *evalSJF() {
    int queue[10 * SIZE][3];
    scheduleSJF(queue);

    int len = 0;
    while (queue[len][0] != -2) len++;

    struct Evaluation *eval = evaluateAlgorithm(queue, len, processInMemory);
    return eval;
}

void scheduleSJF(int queue[][3]) {
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
                insertMinHeap(readyQueue, processes[cursor++], readyQueueLen++,
                              NEXT_CPU_BURST_TIME);
            } else break;
        }

        if (waitingQueueLen) {
            while (waitingQueueLen && waitingQueue[0]->arrivalTime <= end) {
                insertMinHeap(readyQueue,
                              popMinHeap(waitingQueue, waitingQueueLen--, ARRIVAL_TIME),
                              readyQueueLen++, NEXT_CPU_BURST_TIME);
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
        popMinHeap(readyQueue, readyQueueLen--, NEXT_CPU_BURST_TIME);

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

#endif //SJF_H
