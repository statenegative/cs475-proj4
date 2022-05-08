#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NLOCK   10
#define NPROC   20

static unsigned char RAG[NLOCK + NPROC][NLOCK + NPROC];
static unsigned char visited[NLOCK + NPROC][NLOCK + NPROC];

/** Adds a request edge to the RAG from pid to lockid. */
void rag_request(int pid, int lockid);

/**
 * Adds an allocation edge to the RAG from lockid to pid. Removes the request
 * edge from pid to lockid.
 */
void rag_alloc(int pid, int lockid);

/** Removes the request or allocation edge from lockid to pid. */
void rag_dealloc(int pid, int lockid);

/** Prints the adjacency matrix or list representing the current RAG. */
void rag_print();

/** Checks the RAG for cycles. For each cycle found, prints out DEADLOCK
 * followed by the nodes involved in the cycle. If no cycles are found, then
 * the system is deadlock-free.
 */
int deadlock_detect();

static int has_cycle(int curr);

int main(void) {
    enum { BUF_LEN = 1024 };
    char buf[BUF_LEN];
    int len, i;

    scanf("%d\n", &len);
    for (i = 0; i < len; ++i) {
        int pid, lockid;
        char type;

        fgets(buf, BUF_LEN, stdin);
        sscanf(buf, "%d\t%c\t%d", &pid, &type, &lockid);

        if (type == 'R')
            rag_request(pid, lockid);
        else if (type == 'A')
            rag_alloc(pid, lockid);
        else
            rag_dealloc(pid, lockid);
    }

    rag_print();
    deadlock_detect();

    return 0;
}
void rag_request(int pid, int lockid)
{
    RAG[NLOCK + pid][lockid] = 1;
}

void rag_alloc(int pid, int lockid)
{
    RAG[NLOCK + pid][lockid] = 0;
    RAG[lockid][NLOCK + pid] = 1;
}

void rag_dealloc(int pid, int lockid)
{
    RAG[NLOCK + pid][lockid] = 0;
    RAG[lockid][NLOCK + pid] = 0;
}

void rag_print()
{
    int i, j;
    for (i = 0; i < NLOCK + NPROC; ++i) {
        for (j = 0; j < NLOCK + NPROC; ++j) {
        }
    }
}

int deadlock_detect()
{
    int i, j;

    // Set visited to zero
    for (i = 0; i < NLOCK + NPROC; ++i) {
        for (j = 0; j < NLOCK + NPROC; ++j) {
            visited[i][j] = 0;
        }
    }

    for (i = NLOCK; i < NLOCK + NPROC; ++i) {
        if (has_cycle(i)) {
            printf("\n");
            return 1;
        }
    }

    return 0;
}

static int has_cycle(int curr)
{
    int i;

    for (i = 0; i < NLOCK + NPROC; ++i) {
        if (RAG[curr][i]) {
            if (visited[curr][i]) {
                /* Deadlock */
                if (i < NLOCK) {
                    printf("DEADLOCK DETECTED:\tL%d", i);
                } else {
                    printf("DEADLOCK DETECTED:\tp%d", i);
                }

                return 1;
            } else {
                visited[curr][i] = 1;

                if (has_cycle(i)) {
                    if (i < NLOCK) {
                        printf(" <- L%d", i);
                    } else {
                        printf(" <- p%d", i - NLOCK);
                    }

                    return 1;
                }
            }
        }
    }
}
