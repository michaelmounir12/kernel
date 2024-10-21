/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (~(*curBlkMetaData) & 0x1) ;
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk)) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
    //==================================================================================
    //DON'T CHANGE THESE LINES==========================================================
    //==================================================================================
    {
        if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; // Ensure it's a multiple of 2
        if (initSizeOfAllocatedSpace == 0)
            return ;
        is_initialized = 1;
    }
    //==================================================================================
    //==================================================================================

    // Your Code is Here...
//    uint32 *cur = (uint32*) daStart;

    // Initialize the first and last addresses
    *((uint32*) daStart )=1; // Setting the first address to 1 (can denote allocated)
    *((uint32*) (daStart +  initSizeOfAllocatedSpace - sizeof(int) )) = 1; // Setting the last address to 1
    uint32* blkHeader = (uint32*) (daStart + sizeof(int));
    uint32* blkFooter = (uint32*) (daStart +  initSizeOfAllocatedSpace - 2*sizeof(int));
    *blkHeader = initSizeOfAllocatedSpace - 2*sizeof(int);
    *blkFooter = initSizeOfAllocatedSpace - 2*sizeof(int);
    // Initializing the first free block
    // Size boundaries
//    uint32 size = initSizeOfAllocatedSpace ; // Size for the first block
//    *(cur+1) = size ; // Set the size and allocated flag for the first block
//    *(cur+initSizeOfAllocatedSpace ) = size ; // Set the same for the last block
//
//    // Create the first free block structure
    struct BlockElement *blockptr = (struct BlockElement*)((daStart + 2*sizeof(int)));
//
//    // Initialize the BlockElement (make sure to set its fields correctly)
//
//    // Add the block to the free list
    LIST_INSERT_TAIL(&freeBlocksList, blockptr);
}

//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("set_block_data is not implemented yet");
	//Your Code is Here...
}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
		if (!is_initialized)
		{
			uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("alloc_block_FF is not implemented yet");
	//Your Code is Here...

}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("alloc_block_BF is not implemented yet");
	//Your Code is Here...

}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	// TODO: Implement free_block
	if(is_free_block(va)) return;

	uint32 size = get_block_size(va);  // Get the size of the block
	int8 is_pre_free = is_free_block((uint32 *)va - 1); // Check the previous block
	int8 is_next_free = is_free_block((uint32 *)((uint32 *)va + size)); // Check the next block
	uint32 *bgn_size_ptr = ((uint32*) va)-1;
    uint32 *end_size_ptr = (uint32*) bgn_size_ptr -(size-sizeof(int));
	// case 1: block doesn't have free block before or after
	if (!is_pre_free && !is_next_free) {
		//modify the size
		*bgn_size_ptr = size-sizeof(int);
		*end_size_ptr = size-sizeof(int);

		// Get the first element in the free list
		struct BlockElement *pre_free = freeBlocksList.lh_first;
		struct BlockElement *prev = NULL;

		// Traverse the free list to find the position to insert the block
		while (pre_free != NULL && (uint32 *)pre_free < (uint32 *)va) {
		    prev = pre_free;
		    pre_free = pre_free->prev_next_info.le_next; // Correct use of the pointer
		}

		// Initialize the block to free
		struct BlockElement *block = (struct BlockElement *)va;

		// Update the pointers in the linked list

		block->prev_next_info.le_next = pre_free;
		pre_free->prev_next_info.le_prev = block->prev_next_info.le_next;
		if (prev != NULL) {
			prev->prev_next_info.le_next = block;
			block->prev_next_info.le_prev = prev;
		} else {
			// If there's no previous, this is the first block in the free list
			freeBlocksList.lh_first = block;
		}


	}
	// Other cases to handle coalescing with previous and next free blocks
	if(is_pre_free && !is_next_free){
		uint32 new_size = *((uint32*)va -1) - sizeof(int) + *((uint32*)va -2);
		uint32 *new_begin_size_ptr = end_size_ptr-(new_size-sizeof(int));
		uint32 *new_end_size_ptr = end_size_ptr;
		struct BlockElement* pre_block_start =(struct BlockElement*) new_begin_size_ptr+sizeof(int);
		pre_block_start->prev_next_info.le_next = ((struct BlockElement*) va)->prev_next_info.le_next;
		(((struct BlockElement*) va)->prev_next_info.le_next)->prev_next_info.le_prev = pre_block_start;

		//free space
		*((uint32 *)va)=0;
		*((uint32*) va+1)=0;
		*((uint32*) va -1) =0;
		*((uint32*) va -2) =0;
		*(new_begin_size_ptr) = new_size;
		*(new_end_size_ptr)=new_size;
	}

	if(!is_pre_free && is_next_free){
		uint32 new_size = *((uint32*)va -1)-sizeof(int) + *(end_size_ptr+1);
		struct BlockElement *next_block_start =(struct BlockElement *) (end_size_ptr+2);
		uint32 *new_bgn_size_ptr = bgn_size_ptr;
		uint32 *new_end_size_ptr =  bgn_size_ptr+(new_size-sizeof(int));
		struct BlockElement *cur_block = (struct BlockElement*) va;
		cur_block->prev_next_info.le_next = next_block_start->prev_next_info.le_next;
		(next_block_start->prev_next_info.le_next)->prev_next_info.le_prev = cur_block;
		*new_bgn_size_ptr = new_size;
		*new_end_size_ptr = new_size;

		//free space
		*((uint32*)next_block_start) = 0;
		*((uint32*)next_block_start+1) = 0;
		*((uint32*)next_block_start-2) = 0;
		*((uint32*)next_block_start-1) = 0;








	}
	if(is_pre_free &&is_next_free){
		uint32 new_size = (*((uint32*)va - 1)) - sizeof(int) + *(end_size_ptr + 1) + (*(bgn_size_ptr - 1));
		uint32 *new_bgn_size_ptr = (bgn_size_ptr-1)- (*(bgn_size_ptr-1)-sizeof(int));
		uint32 *new_end_size_ptr = (end_size_ptr+1) + *(end_size_ptr+1)-sizeof(int);


		struct BlockElement* first_block_start_ptr =(struct BlockElement*) (new_bgn_size_ptr+1);
		struct BlockElement* last_block_start_ptr =(struct BlockElement*) (end_size_ptr+2);

		first_block_start_ptr->prev_next_info.le_next = last_block_start_ptr->prev_next_info.le_next;
		(last_block_start_ptr->prev_next_info.le_next)->prev_next_info.le_prev = first_block_start_ptr;
		*(end_size_ptr) =0;
		*(bgn_size_ptr) = 0;
		last_block_start_ptr->prev_next_info.le_next=0;
		last_block_start_ptr->prev_next_info.le_prev=0;
		*new_bgn_size_ptr =new_size;
		*new_end_size_ptr =new_size;





	}

}









//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}
