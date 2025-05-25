#ifndef SJF_H
#define SJF_H

#include <stdio.h>
#include <string.h>

#include "process.h"
#include "util.h"

void scheduleSJF(int queue[][3]);

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

    free(eval);

    printf("\n");
}

void scheduleSJF(int queue[][3]) {
    // readyQueue ==> heap
    int runningProcesses = processInMemory;
    int waitingProcesses = 0;
    int readyQueuelen = 0;
    int end = 0;
    int cursor = 0;
    int PID = -1;
    int i = 0;

    while (runningProcesses) {
        while (cursor < processInMemory) {
            if (processes[cursor]->arrivalTime <= end) {
                insertMinHeap(readyQueue, processes[cursor++], readyQueuelen++, NEXT_CPU_BURST_TIME);
            } else break;
        }

        if (waitingProcesses) {
            while (waitingProcesses && waitingQueue[0]->arrivalTime <= end) {
                insertMinHeap(readyQueue,
                              popMinHeap(waitingQueue, waitingProcesses--, ARRIVAL_TIME),
                              readyQueuelen++, NEXT_CPU_BURST_TIME);
            }
        }

        if (!readyQueuelen) {
            PID = -1;
            if (cursor < processInMemory) {
                if (waitingProcesses)
                    end = min(processes[cursor]->arrivalTime, waitingQueue[0]->arrivalTime);
                else
                    end = processes[cursor]->arrivalTime;
            } else end = waitingQueue[0]->arrivalTime;
        } else {
            struct Process *top = malloc(sizeof(struct Process));
            memcpy(top, readyQueue[0], sizeof(struct Process));
            popMinHeap(readyQueue, readyQueuelen--, NEXT_CPU_BURST_TIME);
            PID = top->PID;
            queue[i][2] = top->arrivalTime;

            if (top->IOburstTimeNumber) {
                if (top->currentIOburstNumber >= top->IOburstTimeNumber) {
                    end += top->CPUburstTime - top->executedCPUburstTime;
                    runningProcesses--;
                } else {
                    end += nextCPUburstTime(top);
                    int *currentIOburstInfo = top->IOburstTime[top->currentIOburstNumber++];
                    top->arrivalTime = end + currentIOburstInfo[1];
                    top->executedCPUburstTime = currentIOburstInfo[0];
                    insertMinHeap(waitingQueue, top, waitingProcesses++, ARRIVAL_TIME);
                }
            } else {
                // If the process does not request I/O
                end += top->CPUburstTime;
                runningProcesses--;
            }
        }

        queue[i][0] = PID;
        queue[i][1] = end;
        i++;
    }
    queue[i][0] = -2;
}

#endif //SJF_H
