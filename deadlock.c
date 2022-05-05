#include <stdio.h>
#include <stdlib.h>

/** Max number of locks. */
//#define NLOCK   10
#define NLOCK   3
/** Max number of processes in the OS. */
//#define NPROC   20
#define NPROC   3

static unsigned char RAG[NLOCK + NPROC][NLOCK + NPROC] = { 0 };

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
void deadlock_detect();

static int has_cycle(int originID, int currID);

int main(void) {
    RAG[1][5] = 1;
    RAG[2][4] = 1;
    RAG[3][0] = 1;
    RAG[4][0] = 1;
    RAG[4][1] = 1;
    RAG[5][4] = 1;

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
    RAG[lockid][NLOCK + pid] = 1;
}

void rag_dealloc(int pid, int lockid)
{
    RAG[lockid][NLOCK + pid] = 0;
}

void rag_print()
{
    printf("   ");
    for (size_t i = 0; i < NLOCK; ++i)
        printf(" L%2lu", i);
    for (size_t i = 0; i < NPROC; ++i)
        printf(" p%2lu", i);
    printf("\n");
    
    for (size_t i = 0; i < NLOCK + NPROC; ++i) {
        if (i < NLOCK)
            printf("L%2lu", i);
        else
            printf("p%2lu", i - NLOCK);

        for (size_t j = 0; j < NLOCK + NPROC; ++j) {
            printf("  %2lu", RAG[i][j]);
        }

        printf("\n");
    }
}

void deadlock_detect()
{
    for (size_t i = 0; i < NLOCK + NPROC; ++i) {
        /* Printing occurs in has_cycle, just terminate here */
        if (has_cycle(i, i)) {
            printf("\n");
            break;
        }
    }
}

static int has_cycle(int originID, int currID)
{
    /* Check each item in the row of the matrix */
    for (size_t i = 0; i < NLOCK + NPROC; ++i) {
        if (RAG[currID][i] == 1) {
            if (i == originID) {
                /* Deadlock found */
                printf("DEADLOCK %lu", i);
                return 1;
            } else {
                /* Recursion */
                int result = has_cycle(originID, i);
                if (result == 1) {
                    printf(", %lu", i);
                    return 1;
                }
            }
        }
    }

    return 0;
}
