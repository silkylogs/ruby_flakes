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

b32 rf_Array_test(void);
void rf_Array_unchecked_init(
	struct rf_Array *rf_arr,
	u8 *new_byte_array,
	isize new_elem_length,
	isize new_elem_count);


b32 rf_Array_test() {
	b32 all_ok;
	b32 curr_ok;
	struct rf_Array test;

	all_ok = true;
	curr_ok = true;
	
	/* Type length check */
	RF_TEST(all_ok, curr_ok, (sizeof test.elem_len) == 4);
	RF_TEST(all_ok, curr_ok, (sizeof test.elem_cnt) == 4);
	RF_TEST(all_ok, curr_ok, (sizeof *test.mem_as_bytes) == 1);
        
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
	rf_Array_unchecked_init((RF_ARR_PTR), (NEW_BYTE_ARR), (NEW_ELEM_LEN), (NEW_ELEM_CNT));

#define RF_ARRAY_UNCHECKED_INDEX_ACCESS(RF_ARR, TYPE, IDX) 	\
	  RF_CAST((RF_ARR).mem_as_bytes, TYPE*)[(IDX)]


b32 rf_Array_check_index_access(
	/* Real arguments */
	struct rf_Array rf_arr, usize type_size, isize index,

	/* Macro args */
	const char *filename, isize line,
	const char *index_expression, const char *type_name,
	const char *rf_arr_expression);
b32 rf_Array_check_index_access(
	/* Real arguments */
	struct rf_Array rf_arr, usize type_size, isize index,

	/* Macro args */
	const char *filename, isize line,
	const char *index_expression, const char *type_name,
	const char *rf_arr_expression)
{
	isize upper_bound = rf_arr.elem_cnt;
	usize element_len = RF_CAST(rf_arr.elem_len, usize);

	if (!(index >= 0 && index < upper_bound)) {
		printf("%s:%d:0: Warning:"
		       "index expression (%s) eval-ing to %d is outside of bounds [0, %d]\n",
		       filename, RF_CAST(line, i32),
		       index_expression, RF_CAST(index, i32),
		       RF_CAST(upper_bound, i32));
		return false;
	}
	
	/* Write overflow check				
	 * Note:							
	 * I do not care that the write *might* be valid.		
	 * This is to ensure that the write WILL be valid, even if that	
	 * means i'm wasting memory if elem_len < sizeof T		
	 */
	if (!(element_len == type_size)) {
		printf("%s:%d:0: Warning: "
		       "size of type provided (sizeof %s == %d) != "
		       "size specified by expression (\"%s\" == %d)\n",
		       filename, RF_CAST(line, i32),
		       type_name, RF_CAST(type_size, i32),
		       rf_arr_expression, RF_CAST(rf_arr.elem_len, i32));
		return false;
	}
	
	return true;
}

#define RF_ARRAY_CHECK_INDEX_ACCESS(RF_ARR, TYPE, IDX)		\
	rf_Array_check_index_access(				\
		(RF_ARR), sizeof(TYPE), (IDX),			\
		__FILE__, __LINE__, #IDX, #TYPE, #RF_ARR)

#define RF_ARRAY_ASSERT_INDEX_ACCESS(RF_ARR, TYPE, IDX)			\
	RF_ASSERT(RF_ARRAY_CHECK_INDEX_ACCESS(RF_ARR, TYPE, IDX))

#endif
