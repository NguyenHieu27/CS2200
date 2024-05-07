#include "proc.h"
#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 3 --------------------------------------
 * Checkout PDF section 4 for this problem
 * 
 * This function gets called every time a new process is created.
 * You will need to allocate a frame for the process's page table using the
 * free_frame function. Then, you will need update both the frame table and
 * the process's PCB. 
 * 
 * @param proc pointer to process that is being initialized 
 * 
 * HINTS:
 *      - pcb_t: struct defined in pagesim.h that is a process's PCB.
 *      - You are not guaranteed that the memory returned by the free frame allocator
 *      is empty - an existing frame could have been evicted for our new page table.
 * ----------------------------------------------------------------------------------
 */
void proc_init(pcb_t *proc) {
    // TODO: initialize proc's page table.
    // Allocation a frame for the proc's page frame
    pfn_t pt_location = free_frame();
    // Initialize page table memory block
    memset(mem + pt_location * PAGE_SIZE, 0, PAGE_SIZE);
    // Update ptbr in process pcb struct
    proc->saved_ptbr = pt_location;

    fte_t * proc_fte = (frame_table + pt_location);
    // Refer the process to allocated frame in the frame table entry
    proc_fte->process = proc;
    proc_fte->protected = 1; // frame should not be evicted
    proc_fte->mapped = 1; // frame is mapped
}

/**
 * --------------------------------- PROBLEM 4 --------------------------------------
 * Checkout PDF section 5 for this problem
 * 
 * Switches the currently running process to the process referenced by the proc 
 * argument.
 * 
 * Every process has its own page table, as you allocated in proc_init. You will
 * need to tell the processor to use the new process's page table.
 * 
 * @param proc pointer to process to become the currently running process.
 * 
 * HINTS:
 *      - Look at the global variables defined in pagesim.h. You may be interested in
 *      the definition of pcb_t as well.
 * ----------------------------------------------------------------------------------
 */
void context_switch(pcb_t *proc) {
    // TODO: update any global vars and proc's PCB to match the context_switch.
    PTBR = proc -> saved_ptbr; // Update PTBR to refer to new process's page table
}

/**
 * --------------------------------- PROBLEM 8 --------------------------------------
 * Checkout PDF section 8 for this problem
 * 
 * When a process exits, you need to free any pages previously occupied by the
 * process.
 * 
 * HINTS:
 *      - If the process has swapped any pages to disk, you must call
 *      swap_free() using the page table entry pointer as a parameter.
 *      - If you free any protected pages, you must also clear their"protected" bits.
 * ----------------------------------------------------------------------------------
 */
void proc_cleanup(pcb_t *proc) {
    // TODO: Iterate the proc's page table and clean up each valid page
    pte_t * curr;
    pte_t * base = ((pte_t *) (mem + (proc -> saved_ptbr * PAGE_SIZE)));
    for (size_t i = 0; i < NUM_PAGES; i++) {
        // Calculaet address of current page table entry
        curr = ((pte_t *) base) + i;
        // If page is valid, mark it as invalid and unmap page for clean up
        if (curr -> valid == 1) {
            curr -> valid = 0;
            frame_table[curr -> pfn].mapped = 0;
        }
        // Free swap entries
        if (swap_exists(curr)) {
            swap_free(curr);
        }
    }
    // Get frame table entry corresponding to saved PTBR
    fte_t * old_proc_entry = frame_table + proc -> saved_ptbr;
    // Clear protection and unmap frame table entry
    old_proc_entry -> protected = 0;
    old_proc_entry -> mapped = 0;
}

#pragma GCC diagnostic pop
