#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// struct queue_t {
// 	struct pcb_t * proc[MAX_QUEUE_SIZE];
// 	int size;
// };
int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
        /* TODO: put a new process to queue [q] */
        if (q == NULL || proc == NULL) return;
        
        if(q -> size < MAX_QUEUE_SIZE){
                q -> proc[q -> size] = proc;
                q -> size++;    
        }

}

struct pcb_t * dequeue(struct queue_t * q) {
        if(q == NULL || empty(q)) return NULL;

        uint32_t max_prio = UINT32_MAX;

        struct pcb_t * max_proc = NULL;
        int idx = 0;
        for (int i = 0; i < q->size; i++) {
                if (q->proc[i]->prio < max_prio) {
                        max_prio = q->proc[i]->prio;
                        max_proc = q->proc[i];
                        idx = i;
                }
        }
        for(int i = idx; i < q->size - 1; i++) {
                q->proc[i] = q->proc[i + 1];
        }
        q->size--;
        return max_proc;
}