#ifndef RF_MODULE_ARRAY
#define RF_MODULE_ARRAY

struct rf_Array {
	u8 *mem_as_bytes;
	isize elem_len;
	isize elem_cnt;
};

/*
 * TODO: maybe use ranged pointers to express subarrays?
 * Note: do this only when theres a demonstratable need for it
 */
struct rf_Memory_fat_ptr;

b32 rf_Array_check_assumptions(void);
void rf_Array_unchecked_init(
	struct rf_Array *rf_arr,
	u8 *new_byte_array,
	isize new_elem_length,
	isize new_elem_count);


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

#define RF_ARRAY_CHECKED_INIT(RF_ARR_PTR, NEW_BYTE_ARR, NEW_BYTE_ARR_LEN, NEW_ELEM_LEN, NEW_ELEM_CNT) \
	RF_ASSERT((RF_ARR_PTR) != NULL);				\
	RF_ASSERT((NEW_BYTE_ARR) != NULL);				\
	RF_ASSERT((NEW_BYTE_ARR_LEN) > 0);				\
	RF_ASSERT((NEW_ELEM_LEN) > 0);					\
	RF_ASSERT((NEW_ELEM_CNT) > 0);					\
	RF_ASSERT((NEW_BYTE_ARR_LEN) <= (NEW_ELEM_LEN) * (NEW_ELEM_CNT)); \
	rf_Array_unchecked_init((RF_ARR_PTR), (NEW_BYTE_ARR), (NEW_ELEM_LEN), (NEW_ELEM_CNT)); \

#define RF_ARRAY_UNCHECKED_INDEX_ACCESS(RF_ARR, TYPE, IDX) 	\
	  RF_CAST((RF_ARR).mem_as_bytes, TYPE*)[(IDX)]


#define RF_ARRAY_CHECK_INDEX_ACCESS(RF_ARR, TYPE, IDX)			\
	/* Index bound checks */					\
	RF_ASSERT((IDX) >= 0);						\
	RF_ASSERT((IDX) < (RF_ARR).elem_cnt);				\
									\
	/* Write overflow check						\
	 * Note:							\
	 * I do not care that the write *might* be valid.		\
	 * This is to ensure that the write WILL be valid, even if that	\
	 * means i'm wasting memory if elem_len < sizeof T		\
	 */								\
	RF_ASSERT( (RF_ARR).elem_len == sizeof(TYPE) );			



#endif
