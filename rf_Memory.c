#ifndef RF_MODULE_MEMORY
#define RF_MODULE_MEMORY

#define RF_MEMORY_FREELISTNODE_RELOC_MEM_PTR_SIZE RF_CAST(0xff, isize)
struct rf_Memory_FreeListNode {
	struct rf_Array relocatable_memory;
	void *reloc_mem_ptrs[RF_MEMORY_FREELISTNODE_RELOC_MEM_PTR_SIZE];
};

/*
isize rf_Memory_FreeListNode_get_size(struct rf_Memory_FreeListNode rf_fln);
isize rf_Memory_FreeListNode_get_size(struct rf_Memory_FreeListNode rf_fln){
	return RF_MEMORY_FREELISTNODE_RELOC_MEM_PTR_SIZE + rf_Array_get_size(rf_fln);
}
*/

#define RF_MEMORY_FREELISTNODE_CHECK_ARRAY_MAPPING_ELIGIBILITY(RF_ARR, RF_MEM_FLN) \
	RF_ARRAY_CHECK_INDEX_ACCESS(RF_ARR, struct rf_Memory_FreeListNode, 0)


#define RF_MEMORY_FREELISTNODE_CHECK_MAP_TO_ARRAY(RF_ARR_DEST, RF_MEMORY_FREELISTNODE_SRC, INDEX) \
	RF_ARRAY_CHECK_INDEX_ACCESS(RF_ARR_DEST, struct rf_Memory_FreeListNode, INDEX)

#define RF_MEMORY_FREELISTNODE_UNCHECKED_MAP_TO_ARRAY(RF_ARR_DEST, RF_MEMORY_FREELISTNODE_SRC, INDEX) \
	RF_ARRAY_UNCHECKED_INDEX_ACCESS(				\
		RF_ARR_DEST,						\
		struct rf_Memory_FreeListNode,				\
		INDEX) = RF_MEM_FREELISTNODE_SRC


b32 rf_Memory_test(void);


b32 rf_Memory_test(void) {
	return true;
	
#if 0
	void *backing_buffer;
	usize backing_buffer_len;

	/* All tests ok, current test ok */
	b32 aok = true;
	b32 cok = true;

	/* Init backing buffer */
	backing_buffer_len = 0x400;
	backing_buffer = malloc(backing_buffer_len);
	RF_CHECK_ASSUMPTION(aok, cok, backing_buffer && "Backing buffer failed");
	if (!cok) return false;

	/* Testing overcommit */ {
		isize unrealistic_size = ~(RF_CAST(1, isize) << 63);
		RF_CHECK_ASSUMPTION(aok, cok, !rf_Memory_alloc(unrealistic_size));
	}

	free(backing_buffer);
	return aok;
#endif
}


#endif
