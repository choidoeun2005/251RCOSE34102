#pragma once

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"
#include "FCFS.h"
#include "SJF.h"
#include "PreemtiveSJF.h"
#include "Priority.h"
#include "PreemtivePriority.h"
#include "RR.h"
#include "util.h"

#define HELP_COMMAND_PRINT_SPACE 50

char *compareAlgoNames[10];
int compareAlgoNumbers = 0;
struct Evaluation *compareEvals[10];

void helpAdd();

void helpRemove();

void helpRunGeneral();

void helpRunRR();

void helpCompare();

void helpCompareRR();

void helpCompareAll();

void helpComparePrint();

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
            helpCompare();
            helpCompareRR();
            helpCompareAll();
            helpComparePrint();
            helpList();
            helpExit();

            // ================
            //       ADD
            // ================
        } else if (strcmp(parsedCmd[0], "add") == 0) {
            createProcess();
            getchar(); // To flush buffer
            sortProcesses(ARRIVAL_TIME);
            compareAlgoNumbers = 0;
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
                compareAlgoNumbers = 0;
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
                    compareAlgoNumbers = 0;
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
                if (cmdLen > 2) {
                    if (strcmp(parsedCmd[2], "-p") == 0)
                        printPreemtiveSJF();
                    else {
                        printf("Invalid argument '%s' for 'run sjf'.\n", parsedCmd[2]);
                        helpRunGeneral();
                        goto get_next_command;
                    }
                } else {
                    printSJF();
                }
                reset();
            } else if (strcmp(parsedCmd[1], "priority") == 0) {
                if (cmdLen > 2) {
                    if (strcmp(parsedCmd[2], "-p") == 0)
                        printPreemtivePriority();
                    else {
                        printf("Invalid argument '%s' for 'run priority'.\n", parsedCmd[2]);
                        helpRunGeneral();
                        goto get_next_command;
                    }
                } else {
                    printPriority();
                }
                reset();
            } else if (strcmp(parsedCmd[1], "rr") == 0) {
                int timeQuantum = 10;
                if (cmdLen >= 3) {
                    if (strIsDigit(parsedCmd[2]) != 0) {
                        printf("Invalid argument '%s' for 'run rr' : \n", parsedCmd[2]);
                        helpRunRR();
                        goto get_next_command;
                    }
                    timeQuantum = strToInt(parsedCmd[2]);
                    if (timeQuantum == 0) {
                        printf("Invalid time quantum '0' for 'run rr' : \n");
                        helpRunRR();
                        goto get_next_command;
                    }
                }
                printRR(timeQuantum);
                reset();
            } else if (strcmp(parsedCmd[1], "-a") == 0) {
                printFCFS();
                reset();
                printSJF();
                reset();
                printPreemtiveSJF();
                reset();
                printPriority();
                reset();
                printPreemtivePriority();
                reset();
                printRR(10);
                reset();
            } else {
                printf("Invalid argument '%s' for 'run'\n", parsedCmd[1]);
                helpRunGeneral();
                helpRunRR();
            }
        }

        // ================
        //     COMPARE
        // ================

        else if (strcmp(parsedCmd[0], "compare") == 0) {
            if (cmdLen == 1) {
                // Display compare view
                if (compareAlgoNumbers == 0) {
                    printf("No algorithm to compare with. 'compare add' to add an algorithm to the view.\n");
                    helpCompare();
                    helpCompareRR();
                    helpCompareAll();
                    goto get_next_command;
                }

                printCompareAlgorithms(compareAlgoNames, compareEvals, compareAlgoNumbers);
                goto get_next_command;
            }

            if (strcmp(parsedCmd[1], "add") == 0) {

                if (cmdLen < 3) {
                    printf("Missing options for 'compare' : \n");
                    helpCompare();
                    helpCompareAll();
                    goto get_next_command;
                }

                if (processInMemory == 0) {
                    printf("Failed to run: the memory is empty.\n");
                    goto get_next_command;
                }

                if (compareAlgoNumbers >= 10) {
                    printf("Compare supports up to 10 different algorithms. \n");
                    printf("Execute 'compare clear' and try again. \n");
                    goto get_next_command;
                }

                // =========================
                //     COMPARE ADD FCFS
                // =========================

                if (strcmp(parsedCmd[2], "fcfs") == 0) {
                    if (cmdLen >= 4) {
                        printf("Invalid argument '%s' for 'compare add FCFS'\n", parsedCmd[1]);
                        helpCompare();
                        helpCompareAll();
                        goto get_next_command;
                    }

                    //  check
                    for (int i = 0; i < compareAlgoNumbers; i++) {
                        if (strcmp(compareAlgoNames[i], "FCFS") == 0) {
                            printf("Algorithm 'FCFS' is already in the compare view.\n");
                            goto get_next_command;
                        }
                    }

                    compareAlgoNames[compareAlgoNumbers] = "FCFS";
                    compareEvals[compareAlgoNumbers++] = evalFCFS();
                }

                // =========================
                //     COMPARE ADD SJF
                // =========================

                else if (strcmp(parsedCmd[2], "sjf") == 0) {
                    if (cmdLen >= 4) {
                        if (strcmp(parsedCmd[3], "-p") == 0) { // preemtive mode

                            // Duplication check
                            for (int i = 0; i < compareAlgoNumbers; i++) {
                                if (strcmp(compareAlgoNames[i], "Preemtive SJF") == 0) {
                                    printf("Algorithm 'Preemtive SJF' is already in the compare view.\n");
                                    goto get_next_command;
                                }
                            }

                            compareAlgoNames[compareAlgoNumbers] = "Preemtive SJF";
                            compareEvals[compareAlgoNumbers++] = evalPreemtiveSJF();
                            goto get_next_command;
                        }
                        printf("Invalid option '%s' for 'compare add sjf'\n", parsedCmd[1]);
                        helpCompare();
                        helpCompareAll();
                        goto get_next_command;
                    }

                    // Duplication check
                    for (int i = 0; i < compareAlgoNumbers; i++) {
                        if (strcmp(compareAlgoNames[i], "SJF") == 0) {
                            printf("Algorithm 'SJF' is already in the compare view.\n");
                            goto get_next_command;
                        }
                    }

                    compareAlgoNames[compareAlgoNumbers] = "SJF";
                    compareEvals[compareAlgoNumbers++] = evalSJF();
                }

                // =========================
                //   COMPARE ADD PRIORITY
                // =========================

                else if (strcmp(parsedCmd[2], "priority") == 0) {
                    if (cmdLen >= 4) {
                        if (strcmp(parsedCmd[3], "-p") == 0) { // preemtive mode
                            // Duplication check
                            for (int i = 0; i < compareAlgoNumbers; i++) {
                                if (strcmp(compareAlgoNames[i], "Preemtive Priority") == 0) {
                                    printf("Algorithm 'Preemtive Priority' is already in the compare view.\n");
                                    goto get_next_command;
                                }
                            }

                            compareAlgoNames[compareAlgoNumbers] = "Preemtive Priority";
                            compareEvals[compareAlgoNumbers++] = evalPreemtivePriority();
                            goto get_next_command;
                        }
                        printf("Invalid argument '%s' for 'compare add priority'\n", parsedCmd[1]);
                        helpCompare();
                        helpCompareAll();
                        goto get_next_command;
                    }

                    // Duplication check
                    for (int i = 0; i < compareAlgoNumbers; i++) {
                        if (strcmp(compareAlgoNames[i], "Priority") == 0) {
                            printf("Algorithm 'Priority' is already in the compare view.\n");
                            goto get_next_command;
                        }
                    }
                    compareAlgoNames[compareAlgoNumbers] = "Priority";
                    compareEvals[compareAlgoNumbers++] = evalPriority();
                }

                // =========================
                //      COMPARE ADD RR
                // =========================

                else if (strcmp(parsedCmd[2], "rr") == 0) {
                    if (cmdLen >= 4) {
                        if (strIsDigit(parsedCmd[3]) != 0) {
                            printf("Invalid time quantum '%s' for 'compare add rr'\n", parsedCmd[3]);
                            helpCompareRR();
                            goto get_next_command;
                        }

                        int timeQuantum = strToInt(parsedCmd[3]);
                        if (timeQuantum == 0) {
                            printf("Invalid time quantum '0' for 'compare add rr' : \n");
                            goto get_next_command;
                        }

                        struct Evaluation *eval = evalRR(timeQuantum);
                        if (eval->averageTurnaroundTime == -1) {
                            // exception handle
                            goto get_next_command;
                        }

                        char RRName[20];
                        sprintf(RRName, "Round Robin (T = %d)", timeQuantum);

                        // Duplication check
                        for (int i = 0; i < compareAlgoNumbers; i++) {
                            if (strcmp(compareAlgoNames[i], RRName) == 0) {
                                printf("Algorithm '%s' is already in the compare view.\n", RRName);
                                goto get_next_command;
                            }
                        }

                        compareAlgoNames[compareAlgoNumbers] = malloc(strlen(RRName) + 1);
                        strcpy(compareAlgoNames[compareAlgoNumbers], RRName);

                        compareEvals[compareAlgoNumbers++] = evalRR(timeQuantum);
                        goto get_next_command;
                    }
                    // rr with no time quantum (default)


                    // Duplication check
                    for (int i = 0; i < compareAlgoNumbers; i++) {
                        if (strcmp(compareAlgoNames[i], "Round Robin (T = 10)") == 0) {
                            printf("Algorithm 'Round Robin (T = 10)' is already in the compare view.\n");
                            goto get_next_command;
                        }
                    }

                    compareAlgoNames[compareAlgoNumbers] = "Round Robin (T = 10)";
                    compareEvals[compareAlgoNumbers++] = evalRR(10);
                }

                // =========================
                //      COMPARE ADD -A
                // =========================

                else if (strcmp(parsedCmd[2], "-a") == 0) {
                    if (compareAlgoNumbers != 0) {
                        printf("Compare -a option may only be used when the compare view is empty.\n");
                        goto get_next_command;
                    }
                    compareAlgoNames[compareAlgoNumbers] = "FCFS";
                    compareEvals[compareAlgoNumbers++] = evalFCFS();
                    compareAlgoNames[compareAlgoNumbers] = "SJF";
                    compareEvals[compareAlgoNumbers++] = evalSJF();
                    compareAlgoNames[compareAlgoNumbers] = "Preemtive SJF";
                    compareEvals[compareAlgoNumbers++] = evalPreemtiveSJF();
                    compareAlgoNames[compareAlgoNumbers] = "Priority";
                    compareEvals[compareAlgoNumbers++] = evalPriority();
                    compareAlgoNames[compareAlgoNumbers] = "Preemtive Priority";
                    compareEvals[compareAlgoNumbers++] = evalPreemtivePriority();
                    compareAlgoNames[compareAlgoNumbers] = "Round Robin (T = 10)";
                    compareEvals[compareAlgoNumbers++] = evalRR(10);
                }
            }

            else if (strcmp(parsedCmd[1], "clear") == 0) {
                compareAlgoNumbers = 0;
                printf("Successfully cleared compare view.\n");
            }

            else {
                printf("Invalid argument '%s' for 'compare'\n", parsedCmd[1]);
                helpCompare();
                helpCompareAll();
            }
        }

        // ================
        //     UNKNOWN
        // ================

        else {
            printf("Unknown command : '%s' \n", parsedCmd[0]);
            printf("type 'help' for help\n");
        }


    get_next_command:
        printf(">");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0;
        cmdLen = split(parsedCmd, cmd, " ");

        for (int i = 0; i < cmdLen; i++) {
            parsedCmd[i] = toLowerCase(parsedCmd[i]);
        }
    }
}

void helpAdd() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "add",
           "Adds a new process to memory through an interactive dialogue sequence. "
           "Type -1 any time to suspend.");
}

void helpRemove() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "remove [-a | PID]",
           "Remove processes. Use -a to remove all, "
           "or specify a PID to remove a specific process.");
}

void helpRunRR() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "run RR [time_quantum]",
           "Runs the CPU scheduler with the Round Robin algorithm "
           "with the specified time quantum (default 10).");
}

void helpRunGeneral() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "run <FCFS|SJF|PRIORITY> [-p]",
           "Runs the CPU scheduler with the chosen algorithm. "
           "-p enables preemptive mode.");

    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "run -a",
           "Runs the CPU scheduler with all possible algorithms. "
           "(FCFS, SJF, PRIORITY, RR)");
}

void helpCompare() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "compare add <FCFS|SJF|PRIORITY> [-p]",
           "Adds or removes an algorithm to/from the compare view. "
           "[-p] option to enable preemtive mode (if possible).");
}

void helpCompareRR() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "compare add RR [time_quantum]",
           "Adds or removes Round Robin algorithm to/from the compare view. "
           "Time quantum can be set by the argument. (Default 10)");
}

void helpCompareAll() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "compare <add [-a]|clear>",
           "Adds or removes all algorithms to/from the compare view."
           "Round Robin is added with the default time quantum 10.");
}

void helpComparePrint() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "compare",
           "Displays compare view.");
}

void helpList() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "list",
           "Show all the processes in memory.");
}

void helpExit() {
    printf("\t%-*s%s\n\n", HELP_COMMAND_PRINT_SPACE,
           "exit",
           "Exit the CPU scheduler.");
}

void schedule() {
}
