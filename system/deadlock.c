#include <xinu.h>
#include <stdlib.h>

static unsigned char visited[NLOCK + NPROC][NLOCK + NPROC];
static lid32 dl_id;

static int has_cycle(int32 curr);

void rag_request(pid32 pid, lid32 lockid)
{
    RAG[NLOCK + pid][lockid] = 1;
}

void rag_alloc(pid32 pid, lid32 lockid)
{
    RAG[NLOCK + pid][lockid] = 0;
    RAG[lockid][NLOCK + pid] = 1;
}

void rag_dealloc(pid32 pid, lid32 lockid)
{
    RAG[NLOCK + pid][lockid] = 0;
    RAG[lockid][NLOCK + pid] = 0;
}

void rag_print()
{
    kprintf("   ");
    for (int i = 0; i < NLOCK; ++i)
        kprintf(" L%2d", i);
    for (int i = 0; i < NPROC; ++i)
        kprintf(" p%2d", i);
    kprintf("\n");
    
    for (int i = 0; i < NLOCK + NPROC; ++i) {
        if (i < NLOCK)
            kprintf("L%2d", i);
        else
            kprintf("p%2d", i - NLOCK);

        for (int j = 0; j < NLOCK + NPROC; ++j) {
            kprintf("  %2d", RAG[i][j]);
        }

        kprintf("\n");
    }
}

int deadlock_detect()
{
    int32 i, j;

    // Set visited to zero
    for (i = 0; i < NLOCK + NPROC; ++i) {
        for (j = 0; j < NLOCK + NPROC; ++j) {
            visited[i][j] = 0;
        }
    }

    for (i = NLOCK; i < NLOCK + NPROC; ++i) {
        if (has_cycle(i)) {
            kprintf("\n");
            return 1;
        }
    }

    return 0;
}

static int has_cycle(int32 curr)
{
    int32 i;

    for (i = 0; i < NLOCK + NPROC; ++i) {
        if (RAG[curr][i]) {
            if (visited[curr][i]) {
                /* Deadlock */
                if (i < NLOCK) {
                    // i is lock id
                    dl_id = i;
                    kprintf("DEADLOCK DETECTED:\tL%d", i);
                } else {
                    // curr is lock id
                    dl_id = curr;
                    kprintf("DEADLOCK DETECTED:\tp%d", i);
                }

                return 1;
            } else {
                visited[curr][i] = 1;

                if (has_cycle(i)) {
                    if (i < NLOCK) {
                        kprintf(" <- L%d", i);
                    } else {
                        kprintf(" <- p%d", i - NLOCK);
                    }

                    return 1;
                }
            }
        }
    }

    return 0;
}

void deadlock_recover()
{
    int32 i;
    pid32 victim;

    /*
    kprintf("Before:\n");
    rag_print();
    */

    // Find process holding lock
    for (i = NLOCK; i < NLOCK + NPROC; ++i) {
        if (RAG[dl_id][i]) {
            // Kill process
            victim = i - 1;
            kill(victim - NLOCK);
            break;
        }
    }

    // Remove process from wait queues
    for (i = 0; i < NLOCK; ++i) {
        if (RAG[victim][i]) {
            remove(victim - NLOCK, locktab[i].wait_queue);
        }
    }

    mutex_unlock(&locktab[dl_id].lock);

    // Remove process from RAG
    for (i = 0; i < NLOCK + NPROC; ++i) {
        RAG[victim][i] = 0;
        RAG[i][victim] = 0;
    }

    /*
    kprintf("After:\n");
    rag_print();
    */

    kprintf("DEADLOCK RECOVER\tkilling pid=%d to release lockid=%d\n", victim - NLOCK, dl_id);
}
