#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>

#define max(x, y) ((x > y) ? x : y)
#define min(x, y) ((x < y) ? x : y)


void printCompareAlgorithms(char *algoName[], struct Evaluation *eval[], int len) {

    int width[] = {25, 15, 15, 15};

    printf("[Algorithm Compare View]\n");

    for (int i = 0; i < 4; i++) {
        printf("+");
        for (int j = 0; j < width[i]; j++) printf("-");
    }
    printf("+\n");

    printf("| %-*s", width[0] - 1, "Algorithm");
    printf("| %-*s", width[1] - 1, "Turnaround");
    printf("| %-*s", width[2] - 1, "Waiting");
    printf("| %-*s|\n", width[3] - 1, "Max. Waiting");

    for (int i = 0; i < 4; i++) {
        printf("+");
        for (int j = 0; j < width[i]; j++) printf("-");
    }
    printf("+\n");

    for (int i = 0; i < len; i++) {
        printf("| %-*s", width[0] - 1, algoName[i]);
        printf("| %*.2f ", width[1] - 2, eval[i]->averageTurnaroundTime);
        printf("| %*.2f ", width[2] - 2, eval[i]->averageWaitingTime);
        printf("| %*.2f |\n", width[3] - 2, eval[i]->maxWaitingTime);
    }

    for (int i = 0; i < 4; i++) {
        printf("+");
        for (int j = 0; j < width[i]; j++) printf("-");
    }
    printf("+\n");
}

int *getNRandomNumbers(int N, int start, int end, int sorted, int scale) {
    // returns N distinct random numbers in [start, end]
    // unit is <scale>
    int len = (end - start) / scale + 1;
    int *arr = malloc(len * sizeof(int));
    for (int i = 0; i < len; i++) {
        arr[i] = start + i * scale;
    }

    if (len == 1) {
        return arr;
    }

    int iter = rand() % 6 + 1;
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < N; j++) {
            int index = rand() % (len - 1);
            if (index >= j)
                index++;

            int temp = arr[j];
            arr[j] = arr[index];
            arr[index] = temp;
        }
    }

    int *result = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        result[i] = arr[i];
    }

    if (!sorted) {
        free(arr);
        return result;
    }

    // insertion sort
    for (int i = 1; i < N; i++) {
        int cur = result[i];
        for (int j = i - 1; j >= -1; j--) {
            if (result[j] > cur)
                result[j + 1] = result[j];
            else {
                result[j + 1] = cur;
                break;
            }
            if (j == -1)
                result[j + 1] = cur;
        }
    }

    free(arr);
    return result;
}

int split(char *parsedCmd[], char *str, char *delimeter) {
    char *ptr = strtok(str, delimeter);
    int i = 0;
    for (; ptr; i++) {
        parsedCmd[i] = ptr;
        ptr = strtok(NULL, delimeter);
    }

    parsedCmd[i] = NULL;

    return i;
}

int strIsDigit(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if ('0' <= str[i] && str[i] <= '9');
        else return 1;
    }
    return 0;
}

int strToInt(char *str) {
    int result = 0;
    for (int i = 0; i < strlen(str); i++) {
        int digit = str[i] - '0';
        result = result * 10 + digit;
    }

    return result;
}

char *toLowerCase(char *str) {
    char *result = malloc(strlen(str) + 1);
    if (!result) return NULL;

    for (int i = 0; str[i]; i++) {
        result[i] = ('A' <= str[i] && str[i] <= 'Z') ? str[i] - 'A' + 'a' : str[i];
    }
    result[strlen(str)] = '\0';

    return result;
}

#endif //UTIL_H

