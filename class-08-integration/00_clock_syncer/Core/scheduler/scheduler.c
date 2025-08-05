/*
 * scheduler.c
 *
 *  Created on: Aug 5, 2025
 *      Author: Orr
 */


#include <stdint.h>
#include <stdio.h>
#include "scheduler.h"
#include "stm32f411.h"

/* Task function prototypes */
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);
void idle_task(void);

/* Global variables */
uint32_t g_tick_count = 0;
Scheduler_t scheduler;

// LEDs 12 - 15
void init_leds();

GPIO_TypeDef * GPIO_D_port_desc = (GPIO_TypeDef *)GPIOD_BASE_ADDR;
uint32_t * rcc_ctrl_t = (uint32_t *)RCC_AHB1ENR;


int main(void)
{
    /* Initialize scheduler data structures */
    init_scheduler_stack(SCHED_STACK_START);

    /* Initialize task stacks with initial register values */
    init_tasks_stack();

    /* Load first task and start the scheduler */
    init_systick_timer(TICK_HZ);

    /* Start first task by switching to PSP */
    switch_sp_to_psp();

    /* Task1 execution will begin here */
    task1_handler();

    /* Should never reach here */
    for(;;);
}


/* Initialize scheduler stack */
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack)
{
    __asm volatile("MSR MSP, R0");
    __asm volatile("BX LR");
}

/* Initialize a single task's stack frame */
static uint32_t* init_task_stack_frame(uint32_t* pPSP, void (*task_handler)(void))
{
    /* Cortex-M Exception stack frame (hardware pushed) */
    pPSP--;
    *pPSP = 0x01000000;                    /* xPSR - Thumb state set */

    pPSP--;
    *pPSP = (uint32_t)task_handler;        /* PC - Task entry point */

    pPSP--;
    *pPSP = 0xFFFFFFFD;                    /* LR - Return to thread mode using PSP */

    /* R12, R3, R2, R1, R0 - Initialize to zero */
    for(int i = 0; i < 5; i++) {
        pPSP--;
        *pPSP = 0;
    }

    /* Software pushed registers: R11, R10, R9, R8, R7, R6, R5, R4 */
    for(int i = 0; i < 8; i++) {
        pPSP--;
        *pPSP = 0;
    }

    return pPSP;
}

void scheduler_create_task(uint32_t task_id, void (*task_handler)(void), uint32_t stack_start, Priority_e task_priority)
{
    /* Validate task_id */
    if(task_id >= MAX_TASKS) {
        return; /* Invalid task ID */
    }

    /* Initialize Task Control Block */
    scheduler.tasks[task_id].taskID = task_id;
    scheduler.tasks[task_id].state = TASK_READY;
    scheduler.tasks[task_id].taskFunction = task_handler;
    scheduler.tasks[task_id].task_priority = task_priority;

    /* Initialize task stack */
    uint32_t *pPSP = (uint32_t*)stack_start;
    pPSP = init_task_stack_frame(pPSP, task_handler);

    /* Save stack pointer to TCB */
    scheduler.tasks[task_id].pStackPtr = pPSP;
}

/**
 * @brief Initialize all tasks in the scheduler
 */
void init_tasks_stack(void)
{
    /* Create Task 1 */
    scheduler_create_task(0, task1_handler, TASK1_STACK_START, PRIORITY_LOW);

    /* Create Task 2 */
    scheduler_create_task(1, task2_handler, TASK2_STACK_START, PRIORITY_MED);

    /* Create Task 3 */
    scheduler_create_task(2, task3_handler, TASK3_STACK_START, PRIORITY_MED);

    /* Create Task 4 */
    scheduler_create_task(3, task4_handler, TASK4_STACK_START, PRIORITY_HIGH);

    /* Create Idle Task */
    scheduler_create_task(4, idle_task, IDLE_STACK_START, PRIORITY_IDLE);

    /* Set scheduler initial state */
    scheduler.currentTask = 0;
    scheduler.numTasks = MAX_TASKS;
}


/* Configure SysTick timer for context switching */
void init_systick_timer(uint32_t tick_hz)
{
    /* Calculate SysTick reload value */
    uint32_t reload_value = (SYSTICK_TIM_CLK/tick_hz) - 1; //16000000 / 1000

    /* Configure SysTick */
    /* Reload value */
    *SYST_RVR = reload_value;

    /* Clear current value */
    *SYST_CVR = 0;

    /* Enable SysTick with processor clock and interrupts */
    *SYST_CSR = (1 << 0) | (1 << 1) | (1 << 2);

    /* Configure PendSV priority to lowest (highest value) */
    *SHPR3 |= (0xFF << 16);  /* Set PendSV priority to 0xFF (lowest) */

    /* Configure SysTick priority to second lowest (just above PendSV) */
    *SHPR3 |= (0xFE << 24);  /* Set SysTick priority to 0xFE */
}

/* Switch from MSP to PSP */
__attribute__((naked)) void switch_sp_to_psp(void)
{
    //1. initialize the PSP with TASK1 stack start address

	//get the value of psp of current_task
	__asm volatile ("PUSH {LR}"); //preserve LR which connects back to main()
	__asm volatile ("BL get_psp_value");
	__asm volatile ("MSR PSP,R0"); //initialize psp
	__asm volatile ("POP {LR}");  //pops back LR value

	//2. change SP to PSP using CONTROL register
	__asm volatile ("MOV R0,#0X02");
	__asm volatile ("MSR CONTROL,R0");
	__asm volatile ("BX LR");
}


///* SysTick handler - triggers context switch */
//void SysTick_Handler(void)
//{
//    /* Increment global tick count */
//    g_tick_count++; // add implementation for g_tick_count overflow
//
//    /* Unblock tasks if their sleep time has expired */
//    unblock_tasks();
//
//    /* Trigger PendSV for context switch */
//    *ICSR |= (1 << 28);  /* Set PendSV pending bit */
//}

///* PendSV handler - performs context switching */
//__attribute__((naked)) void PendSV_Handler(void)
//{
//    /* Save the context of current task */
//    /* 1. Get current running task's PSP value */
//    __asm volatile("MRS R0, PSP");
//
//    /* 2. Using that PSP value store SF2(R4 to R11) */
//    __asm volatile("STMDB R0!, {R4-R11}");
//
//    /* 3. Save the current value of PSP */
//    __asm volatile("PUSH {LR}");
//    __asm volatile("BL save_psp_value");
//
//    /* Retrieve the context of next task */
//    /* 1. Decide next task to run */
//    __asm volatile("BL update_next_task");
//
//    /* 2. Get its past PSP value */
//    __asm volatile("BL get_psp_value");
//
//    /* 3. Using that PSP value retrieve SF2(R4 to R11) */
//    __asm volatile("LDMIA R0!, {R4-R11}");
//
//    /* 4. Update PSP and exit */
//    __asm volatile("MSR PSP, R0");
//    __asm volatile("POP {LR}");
//    __asm volatile("BX LR");
//}

/* Save current task's PSP value */
void save_psp_value(uint32_t current_psp)
{
    scheduler.tasks[scheduler.currentTask].pStackPtr = (uint32_t*)current_psp;
}

static inline uint32_t get_next_task(uint32_t current_task){
	current_task = (current_task+1) % (MAX_TASKS - 1);
	return current_task;
}

/* Update task selection - implements round-robin */
void update_next_task(void)
{
    int8_t next_task = -1, selected_task = scheduler.currentTask;
    int8_t highest_p = -1; // can be init to idles priority
	/* Change current task state to READY */
	if(TASK_RUNNING == scheduler.tasks[scheduler.currentTask].state){
		scheduler.tasks[scheduler.currentTask].state = TASK_READY;
	}

    /* Skip blocked tasks */
    uint8_t count = 0;
	while(count < MAX_TASKS -1){
		next_task = get_next_task(scheduler.currentTask);
		if(scheduler.tasks[next_task].state == TASK_READY){
			if(scheduler.tasks[next_task].task_priority > highest_p){
				highest_p = scheduler.tasks[next_task].task_priority;
				selected_task = next_task;
			}
		}
		count++;
	}

    /* Change new task state to RUNNING */
	scheduler.currentTask = selected_task;
    scheduler.tasks[scheduler.currentTask].state = TASK_RUNNING;
}

/* Get PSP of next task to run */
uint32_t get_psp_value(void)
{
    return (uint32_t)scheduler.tasks[scheduler.currentTask].pStackPtr;
}

/* Make task sleep for given ticks */
void task_sleep(uint32_t tick_count)
{
    /* Save wake-up tick count */
    uint32_t current_tick = g_tick_count;
    scheduler.tasks[scheduler.currentTask].wakeup_tick = current_tick + tick_count;

    /* Set task state to BLOCKED */
    scheduler.tasks[scheduler.currentTask].state = TASK_BLOCKED;

    /* Trigger scheduler */
    *ICSR |= (1 << 28);  /* Set PendSV pending bit */

    /* Wait for DSB (Data Synchronization Barrier) */
    __asm volatile("DSB");

    /* Wait for ISB (Instruction Synchronization Barrier) */
    __asm volatile("ISB");
}

/* Unblock tasks that have completed their sleep */
void unblock_tasks(void)
{
    for(int i = 0; i < MAX_TASKS; i++)
    {
        if(scheduler.tasks[i].state == TASK_BLOCKED)
        {
            if(g_tick_count >= scheduler.tasks[i].wakeup_tick)
            {
                /* Task has completed its sleep, mark it as READY */
                scheduler.tasks[i].state = TASK_READY;
            }
        }
    }
}

/* Task handler implementations */
void task1_handler(void)
{

}

void task2_handler(void)
{

}

void task3_handler(void)
{

}

void task4_handler(void)
{

}

void idle_task(void)
{
    static uint32_t idle_counter = 0;

    while(1)
    {
        idle_counter++;

        /* Occasionally print idle message (every 1000 iterations) */
        if(idle_counter % 1000 == 0) {
            printf("Idle task running - counter: %lu\r\n", idle_counter);
        }
    }
}
