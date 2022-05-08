#ifndef DEADLOCK_H
#define DEADLOCK_H

struct RAG_Node {
    struct RAG_Node *next;
    int id;
};

/* RAG declaration */
//extern struct RAG_Node *RAG[];
extern unsigned char RAG[NLOCK + NPROC][NLOCK + NPROC];

/** Adds a request edge to the RAG from pid to lockid. */
void rag_request(pid32 pid, lid32 lockid);

/**
 * Adds an allocation edge to the RAG from lockid to pid. Removes the request
 * edge from pid to lockid.
 */
void rag_alloc(pid32 pid, lid32 lockid);

/** Removes the request or allocation edge from lockid to pid. */
void rag_dealloc(pid32 pid, lid32 lockid);

/** Prints the adjacency matrix or list representing the current RAG. */
void rag_print();

/**
 * Checks the RAG for cycles. For each cycle found, prints out DEADLOCK
 * followed by the nodes involved in the cycle. If no cycles are found, then
 * the system is deadlock-free.
 */
int deadlock_detect();

/** Recover from a deadlock, deleting any deadlocked processes */
void deadlock_recover();

#endif
