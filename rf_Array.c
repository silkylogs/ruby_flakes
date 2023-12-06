#ifndef RF_MODULE_ARRAY
#define RF_MODULE_ARRAY

struct rf_Array {
	u8 *mem_as_bytes;
	isize elem_len;
	isize elem_cnt;
};

/* TODO: maybe use ranged pointers to express subarrays? */
struct rf_Memory_fat_ptr;

b32 rf_Array_check_assumptions(void);
void rf_Array_unchecked_init(
	struct rf_Array *rf_arr,
	u8 *new_byte_array,
	isize new_elem_length,
	isize new_elem_count);
void rf_Array_unchecked_index_write(
	struct rf_Array *rf_arr,
	u8 *const elem_bytes,
	isize elem_length,
	isize index);

b32 rf_Array_check_assumptions() {
	b32 all_ok;
	b32 curr_ok;
	struct rf_Array test;

	all_ok = true;
	curr_ok = true;
	
	/* Type length check */
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof test.elem_len) == 8);
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof test.elem_cnt) == 8);
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof *test.mem_as_bytes) == 1);
        
	return all_ok;
}

void rf_Array_unchecked_init(
	struct rf_Array *rf_arr,
	u8 *new_byte_array,
	isize new_elem_length,
	isize new_elem_count)
{
	rf_arr->mem_as_bytes = new_byte_array;
	rf_arr->elem_len = new_elem_length;
	rf_arr->elem_cnt = new_elem_count;
}

void rf_Array_unchecked_index_write(
	struct rf_Array *rf_arr,
	u8 *const src_bytes,
	isize src_length,
	isize index)
{
	isize bytes_to_write, index_start, index_end, i;
	
        bytes_to_write = src_length;
	index_start = src_length * index;
	index_end = index_start + bytes_to_write;
	
	/* rf_Array_copy_bytes(source, source_len, dest, dest_len); */
	for (i = index_start; i < index_end; ++i)
		rf_arr->mem_as_bytes[i] = src_bytes[i];
}

/*void rf_Array_unchecked_index_read()*/

#define RF_ARRAY_CHECKED_INIT(RF_ARR_PTR, NEW_BYTE_ARR, NEW_BYTE_ARR_LEN, NEW_ELEM_LEN, NEW_ELEM_CNT) \
	do {								\
		RF_ASSERT((RF_ARR_PTR) != NULL);			\
		RF_ASSERT((NEW_BYTE_ARR) != NULL);			\
		RF_ASSERT((NEW_BYTE_ARR_LEN) > 0);			\
		RF_ASSERT((NEW_ELEM_LEN) > 0);				\
		RF_ASSERT((NEW_ELEM_CNT) > 0);				\
		RF_ASSERT((NEW_BYTE_ARR_LEN) <= (NEW_ELEM_LEN) * (NEW_ELEM_CNT)); \
		rf_Array_unchecked_init((RF_ARR_PTR), (NEW_BYTE_ARR), (NEW_ELEM_LEN), (NEW_ELEM_CNT)); \
	} while(0)

#define RF_ARRAY_CHECKED_INDEX_WRITE(RF_ARR_PTR, SRC_BYTE_PTR, SRC_LEN, IDX) \
	do {								\
		RF_ASSERT((RF_ARR_PTR) != NULL);			\
		RF_ASSERT((SRC_BYTE_PTR) != NULL);			\
		RF_ASSERT((SRC_LEN) > 0);				\
		RF_ASSERT((IDX) > 0);					\
		{							\
			/* Upper bound index check */			\
			isize backing_memory_last_idx =			\
				(RF_ARR_PTR)->elem_len * (RF_ARR_PTR)->elem_cnt - 1; \
			RF_ASSERT(backing_memory_lat_idx <= (IDX));	\
									\
			/* On-write memory overflow check */		\
			isize last_idx_of_last_byte_of_source =	(IDX) + (SRC_LEN); \
			RF_ASSERT(					\
				last_idx_of_last_byte_of_source <=	\
				backing_memory_last_idx);		\
		}							\
	} while(0)

#endif
