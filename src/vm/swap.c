#include "vm/swap.h"
#include "threads/synch.h"
/* Initialize  the swap block. */
void
swap_init(){
    swap_block = block_get_role(BLOCK_SWAP);
	if (swap_block == NULL)
		return;
	/* Creat the bitmap to manage the swap slot. */
	swap_map = bitmap_create(block_size(swap_block) / SEC_NUM_PER_PAGE);
	if (swap_map == NULL)
		return;
	/* Set the bit to 1, indicate free to use. */
	bitmap_set_all(swap_map, true);
}


/* Write the content in the frame addr to the swap
   partition. First check the given pointer and 
   whether the block and map has been initialized.
   Return the */
unsigned int
swap_write_out(void* frame_addr){
	/* Get the index for swap. */
	size_t first_free = bitmap_scan_and_flip(swap_map, 0, 1, 1);
	/* Write to the block device. */
	for (unsigned int i = 0; i < SEC_NUM_PER_PAGE; i++)
		block_write(swap_block, first_free * SEC_NUM_PER_PAGE + i,
			frame_addr + i * BLOCK_SECTOR_SIZE);
	/* Return the index of write. */
	return first_free;
}


/* Read the data in the swap partition back to frame
   address. Return a boolean value indicate whether 
   it succeed to do so. */
void
swap_read_in(unsigned int idx, void* frame_addr){
	/* Read the data from block device. */
	for (unsigned int i = 0; i < SEC_NUM_PER_PAGE; i++)
		block_read(swap_block, idx*SEC_NUM_PER_PAGE+i,
						frame_addr + i*BLOCK_SECTOR_SIZE);
	/* Set the index back to free. */
	bitmap_set(swap_map, idx, true);
	return;
}