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

/**
 * System call handler: sys_killallhandler
 * Description: Sample system call that prints the first parameter passed from user program.
 * Parameters:
 *    - caller: pointer to process control block of calling process
 *    - regs: system call registers containing arguments (a1, a2, a3)
 * Returns:
 *    - 0 always (success)
 */
int __sys_killallhandler(struct pcb_t *caller, struct sc_regs *regs) {
    printf("sys_killallhandler invoked!\n");
    printf("The first system call parameter is: %d\n", regs->a1);
    return 0;
}