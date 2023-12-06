/* ----- rf_Array ----- */

struct rf_Array {
	void *elems;
	u64 elem_len;
	u64 arr_len;
};

/* TODO: maybe use ranged pointers to express subarrays? */
struct rf_Memory_fat_ptr;

b32 rf_Array_check_assumptions(void);
void rf_Array_init(struct rf_Array *array, void *new_elems, u64 new_elem_length, u64 new_arr_length);

b32 rf_Array_check_assumptions() {
	b32 all_ok;
	b32 curr_ok;
	struct rf_Array test;

	all_ok = true;
	curr_ok = true;
	
	/* Type length check */
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof test.elem_len == 8));
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof test.arr_len == 8));
        
	return all_ok;
}

void rf_Array_init(struct rf_Array *array, void *new_elems, u64 new_elem_length, u64 new_arr_length) {
	array->elems = new_elems;
	array->elem_len = new_elem_length;
	array->arr_len = new_arr_length;
}

/* ----- rf_Array ----- */
