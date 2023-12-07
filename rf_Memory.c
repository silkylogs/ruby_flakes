#ifndef RF_MODULE_MEMORY
#define RF_MODULE_MEMORY

// TODO: Recreate free list memory allocator, but better than last time

/* Generic placeholder functions */
b32 rf_Memory_alloc(isize);
b32 rf_Memory_free(void *block);

b32 rf_Memory_test(void);
b32 rf_Memory_test(void) {
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
}

#endif
