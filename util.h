#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>

#define max(x, y) ((x > y) ? x : y)
#define min(x, y) ((x < y) ? x : y)

int *getNRandomNumbers(int N, int start, int end, int sorted, int scale) {
    // returns N distinct random numbers in [start, end]
    // unit is 5
    int len = (end - start) / scale + 1;
    int *arr = malloc(len * sizeof(int));
    for (int i = 0; i < len; i++) {
        arr[i] = start + i * scale;
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
    for (char *p = str; *p; p++) {
        if ('A' <= *p && *p <= 'Z')
            *p -= 'A' - 'a';
    }

    return str;
}
#endif //UTIL_H
