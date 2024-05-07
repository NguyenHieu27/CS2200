/*
 * student.c
 * Multithreaded OS Simulation for CS 2200
 * Spring 2023
 *
 * This file contains the CPU scheduler for the simulation.
 */

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "student.h"
#include <unistd.h>
#include <limits.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/** Function prototypes **/
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);

static unsigned int cpu_count;

/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 *
 * rq is a pointer to a struct you should use for your ready queue
 * implementation. The head of the queue corresponds to the process
 * that is about to be scheduled onto the CPU, and the tail is for
 * convenience in the enqueue function. See student.h for the
 * relevant function and struct declarations.
 *
 * Similar to current[], rq is accessed by multiple threads,
 * so you will need to use a mutex to protect it. ready_mutex has been
 * provided for that purpose.
 *
 * The condition variable queue_not_empty has been provided for you
 * to use in conditional waits and signals.
 *
 * Please look up documentation on how to properly use pthread_mutex_t
 * and pthread_cond_t.
 *
 * A scheduler_algorithm variable and sched_algorithm_t enum have also been
 * supplied to you to keep track of your scheduler's current scheduling
 * algorithm. You should update this variable according to the program's
 * command-line arguments. Read student.h for the definitions of this type.
 */
static pcb_t **current;
static queue_t *rq;

static pthread_mutex_t current_mutex;
static pthread_mutex_t queue_mutex;
static pthread_cond_t queue_not_empty;

static sched_algorithm_t scheduler_algorithm;
static unsigned int cpu_count;
static unsigned int age_weight;
static int time_slice;

/** ------------------------Problem 3-----------------------------------
 * Checkout PDF Section 5 for this problem
 * 
 * priority_with_age() is a helper function to calculate the priority of a process
 * taking into consideration the age of the process.
 * 
 * It is determined by the formula:
 * Priority With Age = Priority + (Current Time - Enqueue Time) * Age Weight
 * 
 * @param current_time current time of the simulation
 * @param process process that we need to calculate the priority with age
 * 
 */
extern double priority_with_age(unsigned int current_time, pcb_t *process) {
    /* FIX ME */
    return process -> priority + (current_time - process -> enqueue_time) * age_weight;
}

/** ------------------------Problem 0 & 3-----------------------------------
 * Checkout PDF Section 2 and 5 for this problem
 * 
 * enqueue() is a helper function to add a process to the ready queue.
 * 
 * NOTE: For Priority and FCFS scheduling, you will need to have additional logic
 * in this function and/or the dequeue function to account for enqueue time
 * and age to pick the process with the smallest age priority.
 * 
 * We have provided a function get_current_time() which is prototyped in os-sim.h.
 * Look there for more information.
 *
 * @param queue pointer to the ready queue
 * @param process process that we need to put in the ready queue
 */
void enqueue(queue_t *queue, pcb_t *process)
{
    /* FIX ME */

    // Original code
    // if (is_empty(queue)) {
    //     queue->head = process;
    //     queue->tail = process;
    // } else {
    //     // just add to tail
    //     queue->tail->next = process;
    //     queue->tail = process;
    // }

    process->enqueue_time = get_current_time(); // Get current time 
    pthread_mutex_lock(&queue_mutex); // Lock queue mutex

    // if queue is empty, set new process as head and tail
    if (is_empty(queue)) {
        queue->head = process;
        queue->tail = process;
        process->next = NULL; // Set next pointer for running process to null
    } else {
        /* FCFS */
        if (scheduler_algorithm == FCFS) {
            if (queue->head == NULL || process->arrival_time < queue->head->arrival_time) {
                // if process has earlier arrival time, set the process as head
                process->next = queue->head;
                queue->head = process;
                // if queue ios empty, set tail to the process
                if (queue->tail == NULL) {
                    queue->tail = process;
                }
            } else {
                pcb_t *curr = queue->head; // set current process to head
                pcb_t *prev = NULL; // set previous process to null
                // find the first process with later arrival time and insert before it
                while (curr != NULL && curr->arrival_time <= process->arrival_time) {
                    prev = curr;
                    curr = curr->next;
                }
                // if the current process is null, the new process is inserted at the end of queue
                if (curr == NULL) {
                    prev->next = process;
                    process->next = NULL; // set next pointer to null
                    queue->tail = process;
                } else {
                    // otherwise, we will insert the new process before the current one
                    prev->next = process;
                    process->next = curr;
                }
            }
        /* Round Robin (and possibly for other algorithms as well) */
        } else if (scheduler_algorithm == RR) {
            // set new process as tail and set next pointer to null
            queue->tail->next = process;
            queue->tail = process;
            process->next = NULL;
        /* Priority Aging */
        } else if (scheduler_algorithm == PA) {
            // Calculating function priority
            double func_prior = priority_with_age(get_current_time(), process);
            // Calculating current head priority
            double curr_prior = priority_with_age(get_current_time(), queue->head);
            // if queue is empty or new process has higher priority, insert new process at head
            if (queue->head == NULL || func_prior > curr_prior) {
                process->next = queue->head;
                queue->head = process;
                // if queue is empty, the new process is inserted at the end of queue
                if (queue->head->next == NULL) {
                    queue->tail = process;
                }
            } else {
                pcb_t *curr = queue->head; // set current process to head
                // traverse the queue to find the lowest priority
                while (curr->next && priority_with_age(get_current_time(), curr->next) > func_prior) {
                    curr = curr->next;
                }
                // insert the process before the current process
                process->next = curr->next;
                curr->next = process;
                // if the inserted process is the last in the queue, update tail
                if (curr->next == NULL) {
                    queue->tail = process;
                }
            }
        }
    }
    pthread_cond_signal(&queue_not_empty); // Signal that the queue is not empty
    pthread_mutex_unlock(&queue_mutex); // Unlock queue mutex
}

/**
 * dequeue() is a helper function to remove a process to the ready queue.
 *
 * NOTE: For Priority and FCFS scheduling, you will need to have additional logic
 * in this function and/or the enqueue function to account for enqueue time
 * and age to pick the process with the smallest age priority.
 * 
 * We have provided a function get_current_time() which is prototyped in os-sim.h.
 * Look there for more information.
 * 
 * @param queue pointer to the ready queue
 */
pcb_t *dequeue(queue_t *queue)
{
    /* FIX ME */

    // Original code
    // pcb_t *process = queue->head;
    // queue->head = queue->head->next;
    // if (queue->tail == process) {
    //     queue->tail = NULL;
    // }
    // process->next = NULL;
    // return process;
    

    pthread_mutex_lock(&queue_mutex); // Lock queue mutex
    if (is_empty(queue) && !pthread_mutex_unlock(&queue_mutex)) {
        // if queue is empty and mutex is not unlocked, return null
        return NULL;
    }
    pcb_t *process = queue->head; // set process to head
    queue->head = queue->head->next;
    // if tail is the same as the process, set tail to null
    if (queue->tail == process) {
        queue->tail = NULL;
    }
    // set next pointer of running process to null
    process->next = NULL;
    pthread_mutex_unlock(&queue_mutex); // Unlock queue mutex

    return process;
    
}

/** ------------------------Problem 0-----------------------------------
 * Checkout PDF Section 2 for this problem
 * 
 * is_empty() is a helper function that returns whether the ready queue
 * has any processes in it.
 * 
 * @param queue pointer to the ready queue
 * 
 * @return a boolean value that indicates whether the queue is empty or not
 */
bool is_empty(queue_t *queue)
{
    /* FIX ME */
    // if head is null, queue is empty
    return queue->head == NULL;
}

/** ------------------------Problem 1B-----------------------------------
 * Checkout PDF Section 3 for this problem
 * 
 * schedule() is your CPU scheduler.
 * 
 * Remember to specify the timeslice if the scheduling algorithm is Round-Robin
 * 
 * @param cpu_id the target cpu we decide to put our process in
 */
static void schedule(unsigned int cpu_id)
{
    /* FIX ME */
    // Dequeue process from the ready queue
    pcb_t *process = dequeue(rq);
    if (process != NULL) {
        // if process exists, set state to running
        process->state = PROCESS_RUNNING;
    }
    pthread_mutex_lock(&current_mutex); // Lock current mutex
    // Set current process to the dequeued process
    current[cpu_id] = process;
    pthread_mutex_unlock(&current_mutex); // Unlock current mutex
    context_switch(cpu_id, process, time_slice);
}

/**  ------------------------Problem 1A-----------------------------------
 * Checkout PDF Section 3 for this problem
 * 
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled. This function should block until a process is added
 * to your ready queue.
 *
 * @param cpu_id the cpu that is waiting for process to come in
 */
extern void idle(unsigned int cpu_id)
{
   /* FIX ME */
    /* idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
    pthread_mutex_lock(&queue_mutex); // Lock queue mutex
    while(rq->head == NULL) {
        /* Wait on a condition variable that is signaled whenever
        a process is added into ready queue */
        pthread_cond_wait(&queue_not_empty, &queue_mutex);
    }
    pthread_mutex_unlock(&queue_mutex); // Unlock queue mutex

    schedule(cpu_id);
}

/** ------------------------Problem 2 & 3-----------------------------------
 * Checkout Section 4 and 5 for this problem
 * 
 * preempt() is the handler used in Round-robin and Preemptive Priority 
 * Scheduling
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 * 
 * @param cpu_id the cpu in which we want to preempt process
 */
extern void preempt(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&current_mutex); // Lock current mutex
    // Retrieve PCB of the current process
    pcb_t *process = current[cpu_id];
    // Set state to ready
    process->state = PROCESS_READY;
    // Enqueue process into ready queue
    enqueue(rq, process);
    pthread_mutex_unlock(&current_mutex); // Unlock current mutex

    schedule(cpu_id);
}

/**  ------------------------Problem 1A-----------------------------------
 * Checkout PDF Section 3 for this problem
 * 
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * @param cpu_id the cpu that is yielded by the process
 */
extern void yield(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&current_mutex); // Lock current mutex
    // Retrieve PCB of the current process
    pcb_t *process = current[cpu_id];
    pthread_mutex_unlock(&current_mutex); // Unlock current mutex
    if (process != NULL) {
        // Set state to waiting
        process->state = PROCESS_WAITING;
    }
    schedule(cpu_id);
}

/**  ------------------------Problem 1A-----------------------------------
 * Checkout PDF Section 3
 * 
 * terminate() is the handler called by the simulator when a process completes.
 * 
 * @param cpu_id the cpu we want to terminate
 */
extern void terminate(unsigned int cpu_id)
{
    /* FIX ME */
    /* Same structure as yield() except we terminate
    a process if it exists */
    pthread_mutex_lock(&current_mutex); // Lock current mutex
    // Retrieve PDC of the current process
    pcb_t *process = current[cpu_id];
    // Remove process from currrent[]
    current[cpu_id] = 0;
    pthread_mutex_unlock(&current_mutex); // Unlock current mutex
    if (process != NULL){
        // Set state to terminating
        process->state = PROCESS_TERMINATED;
    }

    schedule(cpu_id);
}

/**  ------------------------Problem 1A & 3---------------------------------
 * Checkout PDF Section 3 and 5 for this problem
 * 
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes. This method will also need to handle priority, 
 * Look in section 5 of the PDF for more info.
 * 
 * We have provided a function get_current_time() which is prototyped in os-sim.h.
 * Look there for more information.
 * 
 * @param process the process that finishes I/O and is ready to run on CPU
 */
extern void wake_up(pcb_t *process)
{
    /* FIX ME */
    // Set state to ready
    process->state = PROCESS_READY;
    enqueue(rq, process);

    pthread_cond_signal(&queue_not_empty); // Signal that queue is not empty
    // pthread_mutex_unlock(&queue_mutex); // Unlock mutex

    // Priority Aging
    if (scheduler_algorithm == PA) {
        int lowest = INT_MAX; // initialize lowest priority to max int
        unsigned int sched_CPU = 0; // initialize CPU being scheduled to 0
        unsigned int i = 0;
        pthread_mutex_lock(&current_mutex); // Lock current mutex
        while (i < cpu_count) {
            // Check if current process exists. If not, unlock current mutex and return
            if (current[i] == NULL) {
                pthread_mutex_unlock(&current_mutex);
                return;
            } else {
                // Search for the lowest priority process
                if (priority_with_age(get_current_time(), current[i]) < lowest) {
                    lowest = priority_with_age(get_current_time(), current[i]);
                    sched_CPU = i;
                }
            }
            i++;
        }
        pthread_mutex_unlock(&current_mutex); // Unlock current mutex
        // If the lowest priority process is lower than the process that just woke up,
        // preempt the process
        if (lowest < priority_with_age(get_current_time(), process)) {
            force_preempt(sched_CPU);
        }
    }
}

/**
 * main() simply parses command line arguments, then calls start_simulator().
 * Add support for -r and -p parameters. If no argument has been supplied, 
 * you should default to FCFS.
 * 
 * HINT:
 * Use the scheduler_algorithm variable (see student.h) in your scheduler to 
 * keep track of the scheduling algorithm you're using.
 */
int main(int argc, char *argv[])
{
    /* FIX ME */
    scheduler_algorithm = FCFS;
    age_weight = 0;
    time_slice = 0; // -1 for FCFS

    /* Command line argument parsing
    (must have 2-4 arguments)*/
    if (argc < 2 || argc > 4)
    {
        fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
                        "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p <age weight> ]\n"
                        "    Default : FCFS Scheduler\n"
                        "         -r : Round-Robin Scheduler\n1\n"
                        "         -p : Priority Aging Scheduler\n");
        return -1;
    }

    /* Parse the command line arguments */
    cpu_count = strtoul(argv[1], NULL, 0);

    /* Command line option */
    // Default: FCFS
    // -r: Round-Robin
    // -p: Priority Aging
    char opt = getopt(argc, argv, "rp");
    switch(opt) {
        case 'r':
            scheduler_algorithm = RR;
            time_slice = strtoul(argv[3], NULL, 10);
            age_weight = 0;
            break;
        case 'p':
            scheduler_algorithm = PA;
            time_slice = -1;
            age_weight = strtoul(argv[3], NULL, 10);
            break;
        default:
            scheduler_algorithm = FCFS;
            time_slice = -1;
            age_weight = 0;
            break;
    }

    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t *) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_not_empty, NULL);
    rq = (queue_t *)malloc(sizeof(queue_t));
    assert(rq != NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}

#pragma GCC diagnostic pop
