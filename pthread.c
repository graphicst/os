#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

#define MAX_BUFFER_SIZE 15
#define TOTAL_THREADS 5 // 타이머 스레드를 추가하여 전체 스레드 수를 5로 설정
#define LOOPS 50
#define SHUTDOWN_TIME 5 // 5분을 초로 표현한 종료 시간

HANDLE threads[TOTAL_THREADS];
CRITICAL_SECTION mutex; // 공유 자원 보호를 위한 크리티컬 섹션
CONDITION_VARIABLE cond; // 조건 변수
int buffer[MAX_BUFFER_SIZE]; // 버퍼 배열
int count = 0; // 현재 버퍼에 있는 아이템 수
int g_shutdown = 0; // 전역 종료 플래그

// 버퍼에 아이템 추가
void put(char item) {
    if (count < MAX_BUFFER_SIZE) {
        buffer[count++] = item;
    }
}

// 버퍼에서 아이템 가져오기
char get() {
    char item = ' ';
    if (count > 0) {
        item = buffer[--count];
    }
    return item;
}

// A에서 Z까지 알파벳을 버퍼에 추가하는 프로듀서 스레드 A
DWORD WINAPI producerA(LPVOID arg) {
    while (!g_shutdown) {
        for (int c = 65; c <= 90; ++c) {
            EnterCriticalSection(&mutex); // 크리티컬 섹션 진입
            put(c);
            WakeConditionVariable(&cond); // 조건 변수 시그널
            LeaveCriticalSection(&mutex); // 크리티컬 섹션 빠져나옴
        }
    }
    return 0;
}

// a에서 z까지 알파벳을 버퍼에 추가하는 프로듀서 스레드 B
DWORD WINAPI producerB(LPVOID arg) {
    while (!g_shutdown) {
        for (int c = 97; c <= 122; ++c) {
            EnterCriticalSection(&mutex);
            put(c);
            WakeConditionVariable(&cond);
            LeaveCriticalSection(&mutex);
        }
    }
    return 0;
}

// 1에서 50까지 숫자를 버퍼에 추가하는 프로듀서 스레드 C
DWORD WINAPI producerC(LPVOID arg) {
    while (!g_shutdown) {
        for (int num = 1; num <= 50; ++num) {
            EnterCriticalSection(&mutex);
            put(num);
            WakeConditionVariable(&cond);
            LeaveCriticalSection(&mutex);
        }
    }
    return 0;
}

// 버퍼에서 아이템을 소비하는 컨슈머 스레드
DWORD WINAPI consumer(LPVOID arg) {
    int AtoZ_count = 0, atoz_count = 0, oneTo50_count = 0;

    while (!g_shutdown) {
        EnterCriticalSection(&mutex);
        while (count == 0 && !g_shutdown) {
            SleepConditionVariableCS(&cond, &mutex, INFINITE); // 조건 변수 대기
        }

        if (count > 0) {
            char item = get();
            WakeConditionVariable(&cond); // 조건 변수 시그널
            LeaveCriticalSection(&mutex);

            // 아이템의 종류에 따라 카운트 증가
            if (item >= 65 && item <= 90) {
                AtoZ_count++;
            }
            else if (item >= 97 && item <= 122) {
                atoz_count++;
            }
            else if (item >= 1 && item <= 50) {
                oneTo50_count++;
            }

            // 아이템 출력
            if (item >= 1 && item <= 50)
                printf("%d\t", item);
            else
                printf("%c\t", item);

            // 10개 아이템마다 개행
            int Full_count = AtoZ_count + atoz_count + oneTo50_count;
            if (Full_count % 10 == 0) {
                printf("\n");
            }
        }
        else {
            LeaveCriticalSection(&mutex);
        }
    }

    // 종료 시 카운트 출력
    printf("\n\nAtoZ: %d\natoz: %d\n1to50: %d\n", AtoZ_count, atoz_count, oneTo50_count);

    return 0;
}

// 타이머 스레드
DWORD WINAPI timerThread(LPVOID arg) {
    Sleep(SHUTDOWN_TIME * 1000); // 5분 동안 대기
    g_shutdown = 1; // 종료 플래그 설정

    return 0;
}

// 프로그램 진입점
int main() {
    InitializeCriticalSection(&mutex); // 크리티컬 섹션 초기화
    InitializeConditionVariable(&cond); // 조건 변수 초기화

    // 각각의 스레드 생성
    threads[0] = CreateThread(NULL, 0, timerThread, NULL, 0, NULL); // 타이머 스레드
    threads[1] = CreateThread(NULL, 0, producerB, NULL, 0, NULL);
    threads[2] = CreateThread(NULL, 0, producerC, NULL, 0, NULL);
    threads[3] = CreateThread(NULL, 0, consumer, NULL, 0, NULL);
    threads[4] = CreateThread(NULL, 0, producerA, NULL, 0, NULL);

    WaitForMultipleObjects(TOTAL_THREADS, threads, TRUE, INFINITE); // 모든 스레드의 종료 대기

    DeleteCriticalSection(&mutex); // 크리티컬 섹션 삭제

    return 0;
}