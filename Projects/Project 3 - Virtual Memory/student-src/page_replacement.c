#include "types.h"
#include "pagesim.h"
#include "mmu.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);

pfn_t last_evicted = 0;

/**
 * --------------------------------- PROBLEM 7 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Make a free frame for the system to use. You call the select_victim_frame() method
 * to identify an "available" frame in the system (already given). You will need to 
 * check to see if this frame is already mapped in, and if it is, you need to evict it.
 * 
 * @return victim_pfn: a phycial frame number to a free frame be used by other functions.
 * 
 * HINTS:
 *      - When evicting pages, remember what you checked for to trigger page faults 
 *      in mem_access
 *      - If the page table entry has been written to before, you will need to use
 *      swap_write() to save the contents to the swap queue.
 * ----------------------------------------------------------------------------------
 */
pfn_t free_frame(void) {
    pfn_t victim_pfn;
    victim_pfn = select_victim_frame();

    // TODO: evict any mapped pages.
    if (frame_table[victim_pfn].mapped) {
        vpn_t vpn = frame_table[victim_pfn].vpn;
        pte_t * evicted_frame = (pte_t *) (mem + frame_table[victim_pfn].process -> saved_ptbr * PAGE_SIZE) + vpn;

        if (evicted_frame -> dirty == 1) {
            // If evicted frame is dirty, write contents to swap space
            swap_write(evicted_frame, mem + (victim_pfn * PAGE_SIZE));
            // Manually increment writeback stats (for stats check)
            stats.writebacks++;
            // Clear the dirty bit
            evicted_frame -> dirty = 0;
        }
        // Unmap frame table and mark it as invalid
        frame_table[victim_pfn].mapped = 0;
        evicted_frame -> valid = 0;   
    }

    return victim_pfn;
}



/**
 * --------------------------------- PROBLEM 9 --------------------------------------
 * Checkout PDF section 7, 9, and 11 for this problem
 * 
 * Finds a free physical frame. If none are available, uses either a
 * randomized, FCFS, or clocksweep algorithm to find a used frame for
 * eviction.
 * 
 * @return The physical frame number of a victim frame.
 * 
 * HINTS: 
 *      - Use the global variables MEM_SIZE and PAGE_SIZE to calculate
 *      the number of entries in the frame table.
 *      - Use the global last_evicted to keep track of the pointer into the frame table
 * ----------------------------------------------------------------------------------
 */
pfn_t select_victim_frame() {
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped) {
            return i;
        }
    }

    // RANDOM implemented for you.
    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t unprotected_found = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                unprotected_found = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (unprotected_found < NUM_FRAMES) {
            return unprotected_found;
        }


    } else if (replacement == FIFO) {
        // TODO: Implement the FIFO algorithm here
        pfn_t i = last_evicted;
        while (1) {
            if (!frame_table[i].protected) {
                // Update last_evicted to next page frame number
                last_evicted = (i + 1) % NUM_FRAMES;
            return i;
            }
            // Update index to next page frame number circularly within frame table
            i = (i >= NUM_FRAMES - 1) ? 0 : i + 1;
        }

    } else if (replacement == CLOCKSWEEP) {
        // TODO: Implement the clocksweep page replacement algorithm here
        // Initialize frame pointer for traversal
        static pfn_t pointer = 0;
        while (1) {
            // Iterate through pages from pointer
            for (pfn_t i = pointer; i < NUM_FRAMES; i++) {
                uint8_t ref_check; // Initialize reference bit checker

                if (frame_table[i].process == NULL) {
                    // If page frame does not have a process, mark it as unreferenced
                    ref_check = 0;
                } else {
                    // Get page table entry corresponding to page frame
                    pte_t * pt = (pte_t *) (mem + frame_table[i].process->saved_ptbr * PAGE_SIZE);
                    if (frame_table[i].vpn >= NUM_PAGES) {
                        // Set reference bit checker to 0 if VPN is not within bounds
                        ref_check = 0;
                    } else {
                        // Otherwise, get page table entry and retrieve reference bit
                        pte_t * pt_entry = &pt[frame_table[i].vpn];
                        ref_check = pt_entry->referenced;
                    }
                }

                if (frame_table[i].protected == 0) {
                    if (ref_check == 1 &&
                        frame_table[i].vpn < NUM_PAGES && 
                        frame_table[i].process != NULL) {
                            // If reference bit is set, reset reference bit and continue iteration
                            pte_t * pt = (pte_t *) (mem + frame_table[i].process->saved_ptbr * PAGE_SIZE);
                            pte_t * pt_entry = &pt[frame_table[i].vpn];
                            pt_entry->referenced = 0;
                    } else {
                        // If reference bit is not set or the frame table is not protected, mark the pointer and return page frame
                        pointer = (i != NUM_FRAMES - 1) ? i + 1 : 0;
                        return i;
                    }
                }
                // Update index circularly within frame table if the end is reached
                if (i == NUM_FRAMES - 1) {
                    i = 0;
                }
            }
        }
    }

    /* If every frame is protected, give up. This should never happen
       on the traces we provide you. */
    panic("System ran out of memory\n");
    exit(1);
}
