#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
   // TODO: Get a new frame, then correctly update the page table and frame table
   vpn_t vpn = vaddr_vpn(addr);
   pfn_t pfn = free_frame();
   pte_t * page_entry = ((pte_t *) (mem + (PTBR * PAGE_SIZE))) + vpn;
   uint8_t * new_location = mem + pfn * PAGE_SIZE;
   // If swap entry exists, read the page in saved frame into new frame
   if(swap_exists(page_entry)){
      swap_read(page_entry, new_location);
   } else {
      // Otherwise, clear new frame
      memset(new_location, 0, PAGE_SIZE);
   }

   // Update mapping in current process's page table and appropriate flags for page table entry
   page_entry -> pfn = pfn;
   page_entry -> valid = 1;
   page_entry -> dirty = 0;
   page_entry -> referenced = 1;

   // Update appropriate flags in the corresponding frame table entry
   fte_t * ft_entry = &frame_table[pfn];
   ft_entry -> mapped = 1;
   ft_entry -> protected = 0;
   ft_entry -> process = current_process;
   ft_entry -> vpn = vpn;

   // Manually increment counts for page faults (for stats check)
   stats.page_faults++;
}

#pragma GCC diagnostic pop
