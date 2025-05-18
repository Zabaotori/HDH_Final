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
 
 #include <string.h>
 #include <queue.h>
 #include <sched.h>
 
 int __sys_killall(struct pcb_t *caller, struct sc_regs *regs) {
     char proc_name[100];
     uint32_t data, memrg = regs->a1;
 
     /* TODO: Get name of the target proc */
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
 
     printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);
 
     /* TODO: Traverse proclist to terminate the proc
      *       stcmp to check the process match proc_name
      */
     struct pcb_t *k_process[MAX_PRIO];
     int index = 0;
 
     struct queue_t *run_list = caller->running_list;
     struct queue_t *mlq = caller->mlq_ready_queue;
 
     printf("run_list size: %d\n", run_list->size);
 
     // Traverse run_list
     for (int i = 0; i < run_list->size; ++i) {
         char *proc_get_name = strrchr(run_list->proc[i]->path, '/');
         if (proc_get_name)
             proc_get_name++;
         else
             proc_get_name = run_list->proc[i]->path;
 
         if (strcmp(proc_get_name, proc_name) == 0) {
             k_process[index++] = run_list->proc[i];
             printf("Found process name %s to kill in run list\n", proc_get_name);
 
             run_list->proc[i]->pc = run_list->proc[i]->code->size;
 
             for (int j = i; j < run_list->size - 1; j++)
             {
                 run_list->proc[j] = run_list->proc[j + 1];
             }
 
             run_list->size--;
             i--;
         }
     }
 
     // Traverse mlq
     for (int prio = 0; prio < MAX_PRIO; ++prio) {
         for (int j = 0; j < mlq[prio].size; ++j) {
             char *mlq_proc_get_name = strrchr(mlq[prio].proc[j]->path, '/');
             if (mlq_proc_get_name)
                 mlq_proc_get_name++;
             else
                 mlq_proc_get_name = mlq[prio].proc[j]->path;
 
             if (strcmp(mlq_proc_get_name, proc_name) == 0) {
                 printf("mlq[%d] size = %d\n", prio, mlq[prio].size);
                 k_process[index++] = mlq[prio].proc[j];
                 printf("Found process name %s to kill in mlq\n", mlq_proc_get_name);
 
                 mlq[prio].proc[j]->pc = mlq[prio].proc[j]->code->size;
 
                 for (int k = j; k < mlq[prio].size - 1; k++) {
                     mlq[prio].proc[k] = mlq[prio].proc[k + 1];
                 }
 
                 mlq[prio].size--;
                 j--;
             }
         }
     }
 
     /* TODO Maching and terminating
      *       all processes with given
      *        name in var proc_name
      */
     if (index > 0) {
         printf("Remove %d process from mlq and run queue, ready to free\n", index);
         for (int i = 0; i < index; i++) {
             printf("Free ALlocated region for process %s with ID=%d in kill list\n", proc_name, i);
             for (int j = 0; j < k_process[i]->code->size; j++) {
                 if (k_process[i]->code->text[j].opcode == ALLOC) {
                     __free(k_process[i], k_process[i]->mm->mmap->vm_id, k_process[i]->code->text[j].arg_0);
                 }
             }
         }
     }
     else {
         printf("Process with name %s does not exist\n", proc_name);
     }
     return 0;
 }