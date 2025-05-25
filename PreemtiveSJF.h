#ifndef PREEMTIVESJF_H
#define PREEMTIVESJF_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"


void schedulePreemtiveSJF(int queue[][3]);

void printPreemtiveSJF() {
    printf("============\n");
    printf("%s\n", "Preemtive SJF");
    printf("============\n");

    int queue[10 * SIZE][3];
    schedulePreemtiveSJF(queue);
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

void schedulePreemtiveSJF(int queue[][3]) {
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

            while (waitingQueue[0]->arrivalTime == firstArrivalTime && waitingQueueLen) {
                insertMinHeap(readyQueue, waitingQueue[0], readyQueuelen++, NEXT_CPU_BURST_TIME);
                popSortedArray(waitingQueue, 0, waitingQueueLen--);
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
            popMinHeap(readyQueue, readyQueuelen--, NEXT_CPU_BURST_TIME);
        }

        preempted = 0;

        PID = top->PID;
        topArrivalTime = top->arrivalTime;


        int expectedEnd = end + nextCPUburstTime(top);
        while (waitingQueueLen) {
            int candidateArrival = waitingQueue[0]->arrivalTime;
            if (candidateArrival > expectedEnd) break;

            if (nextCPUburstTime(waitingQueue[0]) < expectedEnd - candidateArrival) {
                // Preempt
                memcpy(preemptingProcess, waitingQueue[0], sizeof(struct Process));
                preempted = 1;
                top->executedCPUburstTime += candidateArrival - end;
                top->arrivalTime = candidateArrival;
                end = candidateArrival;
                insertMinHeap(readyQueue, top, readyQueuelen++, NEXT_CPU_BURST_TIME);
                popSortedArray(waitingQueue, 0, waitingQueueLen--);
                break;
            }
            insertMinHeap(readyQueue, waitingQueue[0], readyQueuelen++, NEXT_CPU_BURST_TIME);
            popSortedArray(waitingQueue, 0, waitingQueueLen--);
        }

        if (preempted) goto parse_next_task;

        if (top->IOburstTimeNumber) {
            if (top->currentIOburstNumber >= top->IOburstTimeNumber) {
                end += top->CPUburstTime - top->executedCPUburstTime;
                runningProcesses--;
            } else {
                end += nextCPUburstTime(top);
                int *currentIOburstInfo = top->IOburstTime[top->currentIOburstNumber++];
                top->arrivalTime = end + currentIOburstInfo[1];
                top->executedCPUburstTime = currentIOburstInfo[0];
                insertSortedArray(waitingQueue, top, waitingQueueLen++, ARRIVAL_TIME);
            }
        } else {
            // If the process does not request I/O
            end += top->CPUburstTime;
            runningProcesses--;
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
