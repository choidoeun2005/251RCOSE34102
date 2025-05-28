#ifndef PREEMTIVEPRIORITY_H
#define PREEMTIVEPRIORITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"


void schedulePreemtivePriority(int queue[][3]);

struct Evaluation *evalPreemtivePriority();

void printPreemtivePriority() {
    printf("============\n");
    printf("%s\n", "Preemtive Priority");
    printf("============\n");

    int queue[10 * SIZE][3];
    schedulePreemtivePriority(queue);
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

struct Evaluation *evalPreemtivePriority() {
    int queue[10 * SIZE][3];
    schedulePreemtivePriority(queue);

    int len = 0;
    while (queue[len][0] != -2) len++;

    struct Evaluation *eval = evaluateAlgorithm(queue, len, processInMemory);
    return eval;
}

void schedulePreemtivePriority(int queue[][3]) {
    int runningProcesses = processInMemory;
    int waitingQueueLen = processInMemory;
    int readyQueuelen = 0;
    int topArrivalTime = 0;
    int end = 0;
    int PID = -1;
    int i = 0;

    int preempted = 0;
    struct Process *preemptingProcess = malloc(sizeof(struct Process));

    memcpy(waitingQueue, processes, sizeof(processes));

    while (runningProcesses) {
        if (!readyQueuelen) {
            int firstArrivalTime = waitingQueue[0]->arrivalTime;

            while (waitingQueueLen && waitingQueue[0]->arrivalTime == firstArrivalTime) {
                insertMinHeap(readyQueue, waitingQueue[0], readyQueuelen++, PRIORITY);
                popMinHeap(waitingQueue, waitingQueueLen--, ARRIVAL_TIME);
            }

            if (firstArrivalTime != 0) {
                PID = -1;
                end = firstArrivalTime;
                topArrivalTime = -1;
                goto parse_next_task;
            }
        }

        struct Process *top = malloc(sizeof(struct Process));

        if (preempted) {
            memcpy(top, preemptingProcess, sizeof(struct Process));
        } else {
            memcpy(top, readyQueue[0], sizeof(struct Process));
            popMinHeap(readyQueue, readyQueuelen--, PRIORITY);
        }

        preempted = 0;

        PID = top->PID;
        topArrivalTime = top->arrivalTime;

        int expectedEnd = end + nextCPUBurstTime(top);
        while (waitingQueueLen) {
            int candidateArrival = waitingQueue[0]->arrivalTime;
            if (candidateArrival > expectedEnd) break;

            if (waitingQueue[0]->priority < top->priority) {
                // If waitingQueue[0] can preempt current process -> preempt
                memcpy(preemptingProcess, waitingQueue[0], sizeof(struct Process));
                preempted = 1;
                top->executedCPUBurstTime += candidateArrival - end;
                top->arrivalTime = candidateArrival;
                end = candidateArrival;
                insertMinHeap(readyQueue, top, readyQueuelen++, PRIORITY);
                popMinHeap(waitingQueue, waitingQueueLen--, ARRIVAL_TIME);
                break;
            }
            // waitingQueue[0] cannot preempt current process -> insert to ready queue
            insertMinHeap(readyQueue, waitingQueue[0], readyQueuelen++, PRIORITY);
            popMinHeap(waitingQueue, waitingQueueLen--, ARRIVAL_TIME);
        }

        if (preempted) goto parse_next_task;

        if (top->currentIOBurstNumber >= top->IOBurstTimeNumber) {
            // Completed I/O
            end += top->CPUBurstTime - top->executedCPUBurstTime;
            runningProcesses--;
        } else {
            end += nextCPUBurstTime(top);
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
    free(preemptingProcess);
}

#endif //PREEMTIVESJF_H
