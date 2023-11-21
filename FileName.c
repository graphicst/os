#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "plist.dat"

void printResult(int pnum, int* pid, int* at, int* bt, int* tt, int* wt) {
    int i, sum_tt = 0, sum_wt = 0;

    printf("%15s %15s %15s %15s %15s\n", "Process id", "Arrival time", "Burst time", "Turnaround time", "Waiting time");
    for (i = 0; i < pnum; ++i) {
        printf("%15d %15d %15d %15d %15d\n", pid[i], at[i], bt[i], tt[i], wt[i]);
        sum_tt += tt[i];
        sum_wt += wt[i];
    }

    printf("  avg. of TT: %.2f\n", (float)sum_tt / pnum);
    printf("  avg. of WT: %.2f\n", (float)sum_wt / pnum);
}

void FIFO(int pnum, int* pid, int* at, int* bt, int* tt, int* wt) {
    printf("  Scheme: First-In-First-Out\n");

    int currentTime = 0; // 현재 시간

    for (int i = 0; i < pnum; ++i) {
        // 현재 시간이 도착 시간보다 작으면 대기
        if (currentTime < at[i]) {
            currentTime = at[i];
        }

        // 현재 프로세스 실행
        tt[i] = currentTime + bt[i] - at[i]; // Turnaround time 계산
        wt[i] = tt[i] - bt[i]; // Waiting time 계산

        currentTime += bt[i]; // 현재 시간 업데이트
    }
}

int main(void) {
    FILE* fp;
    int pnum, i;
    int* pid, * at, * bt, * tt, * wt;

    fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }

    // read the number of processes
    fscanf(fp, "%d", &pnum);

    pid = (int*)calloc(pnum, sizeof(int));
    at = (int*)calloc(pnum, sizeof(int));
    bt = (int*)calloc(pnum, sizeof(int));
    tt = (int*)calloc(pnum, sizeof(int));
    wt = (int*)calloc(pnum, sizeof(int));

    for (i = 0; i < pnum; ++i) {
        fscanf(fp, "%d %d %d", &pid[i], &at[i], &bt[i]);
    }

    fclose(fp);

    FIFO(pnum, pid, at, bt, tt, wt);

    printResult(pnum, pid, at, bt, tt, wt);

    memset(tt, 0, sizeof(int) * pnum);
    memset(wt, 0, sizeof(int) * pnum);

    return 0;
}