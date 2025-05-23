/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

#include "common.h"
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"

#include "string.h"
#include "queue.h"
#include "sched.h"
#include "pthread.h"
#include "stdlib.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int __sys_killall(struct pcb_t *caller, struct sc_regs *regs) {
    char proc_name[100];
    uint32_t data, memrg = regs->a1;
    //@memrg: ID of mem region that store proc name need to kill
    //@proc_name: name of proc need
    
    // Read process name from user memory
    int i = 0;
    data = 0;
    while (1) {
        libread(caller, memrg, i, &data);
        if (data == (uint32_t)-1 || i >= sizeof(proc_name) - 1) {
            proc_name[i] = '\0';
            break;
        }
        proc_name[i++] = (char)data;
    }
    proc_name[i] = '\0'; 
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    // avoid kill itself
    char *proc_get_name = strrchr(caller->path, '/');
    printf("The syskill name is %s\n", proc_get_name + 1);
    if (strcmp(proc_name, proc_get_name + 1) == 0) {
        printf("Don't let syskillall kill itself.\n");
        pthread_mutex_unlock(&lock);
        return 0;
    }

    struct pcb_t *kill_process[MAX_PRIO];
    //@pcb_t: the list of processes were found
    int index = 0;

//     struct queue_t *run_list = caller->running_list;
//     struct queue_t *mlq = caller->mlq_ready_queue;

    // Remove from run_list
    for (int i = 0; i < run_list->size; i++) {
        char *name = strrchr(run_list->proc[i]->path, '/');
        if (name && strcmp(name + 1, proc_name) == 0){
            int dup = 0;
            for (int k = 0; k < index; k++) {
                if (kill_process[k] == run_list->proc[i]) {
                    dup = 1;
                    break;
                }
            }
            if (!dup) {
                kill_process[index++] = run_list->proc[i];
            }

            // Shift remaining elements
            for (int j = i; j < run_list->size - 1; j++) {
                run_list->proc[j] = run_list->proc[j + 1];
            }
            run_list->size--;
            i--;
        }
    }

    // Remove from mlq_ready_queue
    for (int i = 0; i < MAX_PRIO; i++) {
        for (int j = 0; j < mlq[i].size; j++) {
            char *name = strrchr(mlq[i].proc[j]->path, '/');
            if (name && strcmp(name + 1, proc_name) == 0) {
                int dup = 0;
                for (int k = 0; k < index; k++) {
                    if (kill_process[k] == mlq[i].proc[j]) {
                        dup = 1;
                        break;
                    }
                }
                if (!dup) {
                    kill_process[index++] = mlq[i].proc[j];
                }

                // Shift remaining elements
                for (int k = j; k < mlq[i].size - 1; k++) {
                    mlq[i].proc[k] = mlq[i].proc[k + 1];
                }
                mlq[i].size--;
                j--;
            }
        }
    }

    // free the found processes
    if (index > 0) {
        printf("Process with name %s was killed\n", proc_name);
        for (int i = 0; i < index; i++){
            if (kill_process[i] != NULL && kill_process[i]->pc != kill_process[i]->code->size) {
                free(kill_process[i]);
            }
        }
    }
    else {
        printf("Process with name %s does not exist\n", proc_name);
    }

    pthread_mutex_unlock(&lock);
    return 0;
}
