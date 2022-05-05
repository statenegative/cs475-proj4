#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NLOCK   10
#define NPROC   20

struct RAG_Node {
    struct RAG_Node *next;
    int id;
};

static struct RAG_Node *RAG[NLOCK + NPROC];
static unsigned char visited[NLOCK + NPROC];

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

/**
 * Checks the RAG for cycles. For each cycle found, prints out DEADLOCK
 * followed by the nodes involved in the cycle. If no cycles are found, then
 * the system is deadlock-free.
 */
void deadlock_detect();

/** Removes the edge from src to dest. */
static int dealloc_ord(int dest, int src);

/** Checks whether curr is a part of a cycle. */
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
    /* Allocate and emplace node */
    struct RAG_Node *node = malloc(sizeof *node);
    node->id = lockid;
    node->next = RAG[NLOCK + pid];
    RAG[NLOCK + pid] = node;
}

void rag_alloc(int pid, int lockid)
{
    struct RAG_Node *node, *remove;
    size_t i;

    /* Allocate and emplace node */
    node = malloc(sizeof *node);
    node->id = NLOCK + pid;
    node->next = RAG[lockid];
    RAG[lockid] = node;

    /* Remove request edge from pid */
    dealloc_ord(lockid, NLOCK + pid);
}

void rag_dealloc(int pid, int lockid)
{
    /* First search for a request edge, and if not found search for an alloc edge */
    if (!dealloc_ord(lockid, NLOCK + pid))
        dealloc_ord(NLOCK + pid, lockid);
}

void rag_print()
{
    size_t i;
    struct RAG_Node *node;

    for (i = 0; i < NLOCK + NPROC; ++i) {
        /* Print node label */
        if (i < NLOCK)
            printf("L%lu", i);
        else
            printf("p%lu", i - NLOCK);
        
        /* Print connected edges */
        node = RAG[i];
        if (node == NULL) {
            printf(" -> NULL");
        } else {
            for (; node != NULL; node = node->next) {
                if (node->id < NLOCK)
                    printf(" -> L%lu", node->id);
                else
                    printf(" -> p%lu", node->id - NLOCK);
            }
        }

        printf("\n");
    }
}

void deadlock_detect()
{
    size_t i, j;

    for (i = 0; i < NLOCK; ++i) {
        /* Clear visited array */
        for (j = 0; j < NLOCK + NPROC; ++j)
            visited[j] = 0;

        if (has_cycle(i)) {
            printf("\n");
            break;
        }
    }
}

static int dealloc_ord(int dest, int src)
{
    struct RAG_Node *node, *remove;
    int found = 0;

    if (RAG[src] == NULL)
        return found;

    /* Remove request edge from pid */
    if (RAG[src]->id == dest) {
        remove = RAG[src];
        RAG[src] = RAG[src]->next;
        free(remove);
    } else {
        /* If request edge wasn't at the head, search the list */
        for (node = RAG[src]; node->next != NULL; node = node->next) {
            if (node->next->id == dest) {
                remove = node->next;
                node->next = node->next->next;
                free(remove);
                found = 1;
                break;
            }
        }
    }

    return found;
}

static int has_cycle(int curr)
{
    struct RAG_Node *node;

    /* Set curr as visited */
    visited[curr] = 1;

    /* Check each connected edge */
    for (node = RAG[curr]; node != NULL; node = node->next) {
        /* If origin was found, a cycle has occurred */
        if (visited[node->id]) {
            if (node->id < NLOCK)
                printf("DEADLOCK: L%lu", node->id);
            else
                printf("DEADLOCK: p%lu", node->id - NLOCK);
            
            return 1;
        } else {
            /* Recursively check for cycles */
            if (has_cycle(node->id)) {
                if (node->id < NLOCK)
                    printf(" <- L%lu", node->id);
                else
                    printf(" <- p%lu", node->id - NLOCK);
                
                return 1;
            }
        }
    }

    return 0;
}
