#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "process.h"
#include "FCFS.h"
#include "Priority.h"
#include "SJF.h"
#include "util.h"

#include <string.h>

#define HELP_COMMAND_PRINT_SPACE 35

void helpAdd();

void helpRemove();

void helpRunGeneral();

void helpRunRR();

void helpList();

void helpExit();

void parseCommand() {
    printf("------------------------\n");
    printf("CPU Scheduling Simulator\n");
    printf("------------------------\n");

    printf("type 'help' for help\n");
    printf(">");

    char cmd[100];
    fgets(cmd, sizeof(cmd), stdin);
    cmd[strcspn(cmd, "\n")] = 0;

    char *parsedCmd[100];
    int cmdLen = split(parsedCmd, cmd, " ");

    for (int i = 0; i < cmdLen; i++) {
        parsedCmd[i] = toLowerCase(parsedCmd[i]);
    }

    while (1) {
        if (cmdLen > 0 && strcmp(parsedCmd[0], "exit") == 0) break;

        if (cmdLen == 0) {
            goto get_next_command;
        }

        // ================
        //       HELP
        // ================
        if (strcmp(parsedCmd[0], "help") == 0) {
            printf("Command Usage :\n");
            helpAdd();
            helpRemove();
            helpRunRR();
            helpRunGeneral();
            helpList();
            helpExit();

        // ================
        //       ADD
        // ================

        } else if (strcmp(parsedCmd[0], "add") == 0) {
            createProcess();
            getchar();
            sortProcesses(ARRIVAL_TIME);
        }

        // ================
        //      REMOVE
        // ================

        else if (strcmp(parsedCmd[0], "remove") == 0) {
            if (cmdLen < 2) {
                printf("Missing options for 'remove' : \n");
                helpRemove();
                goto get_next_command;
            }

            if (processInMemory == 0) {
                printf("Failed to remove; the memory is empty.\n");
                goto get_next_command;
            }

            if (strcmp(parsedCmd[1], "-a") == 0) {
                config();
                printf("Successfully removed all processes. \n");
            } else {
                if (strIsDigit(parsedCmd[1]) != 0) {
                    printf("Invalid argument '%s' for 'remove' : \n", parsedCmd[1]);
                    helpRemove();
                    goto get_next_command;
                }

                int PID = strToInt(parsedCmd[1]);

                int removeResult = removeProcess(PID);
                if (removeResult == 0) {
                    printf("Successfully removed the process %d\n", PID);
                } else {
                    printf("Unable to find the process %d\n", PID);
                }
            }
        }


        // ================
        //       LIST
        // ================
        else if (strcmp(parsedCmd[0], "list") == 0) {
            printQueue();
        }

        // ================
        //       RUN
        // ================

        else if (strcmp(parsedCmd[0], "run") == 0) {
            if (cmdLen < 2) {
                printf("Missing options for 'run' : \n");
                helpRunGeneral();
                helpRunRR();
                goto get_next_command;
            }

            if (processInMemory == 0) {
                printf("Failed to run: the memory is empty.\n");
                goto get_next_command;
            }

            if (strcmp(parsedCmd[1], "fcfs") == 0) {
                printFCFS();
                reset();
            } else if (strcmp(parsedCmd[1], "sjf") == 0) {
                printSJF();
                reset();
            } else if (strcmp(parsedCmd[1], "priority") == 0) {
                printPriority();
                reset();
            } else if (strcmp(parsedCmd[1], "-a") == 0) {
                printFCFS();
                reset();
                printSJF();
                reset();
                printPriority();
                reset();
            } else {
                printf("Invalid argument '%s' for 'run'\n", parsedCmd[1]);
                helpRunGeneral();
                helpRunRR();
            }
        }

        else {
            printf("Unknown command : '%s' \n", parsedCmd[0]);
            printf("type 'help' for help\n");
        }


    get_next_command:
        printf(">");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0;
        cmdLen = split(parsedCmd, cmd, " ");
    }
}

void helpAdd() {
    printf("\t%-*s%s\n", HELP_COMMAND_PRINT_SPACE,
           "add",
           "Adds a new process to memory through an interactive dialogue sequence. "
           "Type -1 any time to suspend.");
}

void helpRemove() {
    printf("\t%-*s%s\n", HELP_COMMAND_PRINT_SPACE,
           "remove [-a | PID]",
           "Remove processes. Use -a to remove all, "
           "or specify a PID to remove a specific process.");
}

void helpRunRR() {
    printf("\t%-*s%s\n", HELP_COMMAND_PRINT_SPACE,
           "run RR <time_quantum>",
           "Runs the CPU scheduler with the Round Robin algorithm "
           "with the specified time quantum.");
}

void helpRunGeneral() {
    printf("\t%-*s%s\n", HELP_COMMAND_PRINT_SPACE,
           "run <FCFS|SJF|PRIORITY> [-p]",
           "Runs the CPU scheduler with the chosen algorithm. "
           "-p enables preemptive mode.");

    printf("\t%-*s%s\n", HELP_COMMAND_PRINT_SPACE,
       "run -a",
       "Runs the CPU scheduler with all possible algorithms. "
       "(FCFS, SJF, PRIORITY, RR)");
}

void helpList() {
    printf("\t%-*s%s\n", HELP_COMMAND_PRINT_SPACE,
           "list",
           "Show all the processes in memory.");
}

void helpExit() {
    printf("\t%-*s%s\n", HELP_COMMAND_PRINT_SPACE,
           "exit",
           "Exit the CPU scheduler.");
}

void schedule() {
}
