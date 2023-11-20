#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

#define MAX_BUFFER_SIZE 15
#define TOTAL_THREADS 5 // Ÿ�̸� �����带 �߰��Ͽ� ��ü ������ ���� 5�� ����
#define LOOPS 50
#define SHUTDOWN_TIME 5 // 5���� �ʷ� ǥ���� ���� �ð�

HANDLE threads[TOTAL_THREADS];
CRITICAL_SECTION mutex; // ���� �ڿ� ��ȣ�� ���� ũ��Ƽ�� ����
CONDITION_VARIABLE cond; // ���� ����
int buffer[MAX_BUFFER_SIZE]; // ���� �迭
int count = 0; // ���� ���ۿ� �ִ� ������ ��
int g_shutdown = 0; // ���� ���� �÷���

// ���ۿ� ������ �߰�
void put(char item) {
    if (count < MAX_BUFFER_SIZE) {
        buffer[count++] = item;
    }
}

// ���ۿ��� ������ ��������
char get() {
    char item = ' ';
    if (count > 0) {
        item = buffer[--count];
    }
    return item;
}

// A���� Z���� ���ĺ��� ���ۿ� �߰��ϴ� ���ε༭ ������ A
DWORD WINAPI producerA(LPVOID arg) {
    while (!g_shutdown) {
        for (int c = 65; c <= 90; ++c) {
            EnterCriticalSection(&mutex); // ũ��Ƽ�� ���� ����
            put(c);
            WakeConditionVariable(&cond); // ���� ���� �ñ׳�
            LeaveCriticalSection(&mutex); // ũ��Ƽ�� ���� ��������
        }
    }
    return 0;
}

// a���� z���� ���ĺ��� ���ۿ� �߰��ϴ� ���ε༭ ������ B
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

// 1���� 50���� ���ڸ� ���ۿ� �߰��ϴ� ���ε༭ ������ C
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

// ���ۿ��� �������� �Һ��ϴ� ������ ������
DWORD WINAPI consumer(LPVOID arg) {
    int AtoZ_count = 0, atoz_count = 0, oneTo50_count = 0;

    while (!g_shutdown) {
        EnterCriticalSection(&mutex);
        while (count == 0 && !g_shutdown) {
            SleepConditionVariableCS(&cond, &mutex, INFINITE); // ���� ���� ���
        }

        if (count > 0) {
            char item = get();
            WakeConditionVariable(&cond); // ���� ���� �ñ׳�
            LeaveCriticalSection(&mutex);

            // �������� ������ ���� ī��Ʈ ����
            if (item >= 65 && item <= 90) {
                AtoZ_count++;
            }
            else if (item >= 97 && item <= 122) {
                atoz_count++;
            }
            else if (item >= 1 && item <= 50) {
                oneTo50_count++;
            }

            // ������ ���
            if (item >= 1 && item <= 50)
                printf("%d\t", item);
            else
                printf("%c\t", item);

            // 10�� �����۸��� ����
            int Full_count = AtoZ_count + atoz_count + oneTo50_count;
            if (Full_count % 10 == 0) {
                printf("\n");
            }
        }
        else {
            LeaveCriticalSection(&mutex);
        }
    }

    // ���� �� ī��Ʈ ���
    printf("\n\nAtoZ: %d\natoz: %d\n1to50: %d\n", AtoZ_count, atoz_count, oneTo50_count);

    return 0;
}

// Ÿ�̸� ������
DWORD WINAPI timerThread(LPVOID arg) {
    Sleep(SHUTDOWN_TIME * 1000); // 5�� ���� ���
    g_shutdown = 1; // ���� �÷��� ����

    return 0;
}

// ���α׷� ������
int main() {
    InitializeCriticalSection(&mutex); // ũ��Ƽ�� ���� �ʱ�ȭ
    InitializeConditionVariable(&cond); // ���� ���� �ʱ�ȭ

    // ������ ������ ����
    threads[0] = CreateThread(NULL, 0, timerThread, NULL, 0, NULL); // Ÿ�̸� ������
    threads[1] = CreateThread(NULL, 0, producerB, NULL, 0, NULL);
    threads[2] = CreateThread(NULL, 0, producerC, NULL, 0, NULL);
    threads[3] = CreateThread(NULL, 0, consumer, NULL, 0, NULL);
    threads[4] = CreateThread(NULL, 0, producerA, NULL, 0, NULL);

    WaitForMultipleObjects(TOTAL_THREADS, threads, TRUE, INFINITE); // ��� �������� ���� ���

    DeleteCriticalSection(&mutex); // ũ��Ƽ�� ���� ����

    return 0;
}