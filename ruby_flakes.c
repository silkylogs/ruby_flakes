int puts(const char *);
int printf(const char *, ...);

#include <stdint.h>
#include <stdbool.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;

typedef size_t usize;
typedef ssize_t isize;
typedef uintptr_t uintptr;
typedef u32 b32;

#define RF_CHECK_ASSUMPTION(ACCUMULATOR_BOOL, CURR_BOOL, EXPRESSION) \
(CURR_BOOL) = (EXPRESSION); \
if (!(CURR_BOOL)) { \
	printf("%s:%d:0: Warning: Expression evals to false: %s\n", __FILE__, __LINE__, #EXPRESSION); \
} \
(ACCUMULATOR_BOOL) &= (CURR_BOOL);

#define RF_CAST(VARIABLE, CAST_TO) ((CAST_TO) (VARIABLE))
#define RF_LAZY_ASSERT(EXPR) do { \
		b32 place, holder; \
		place = holder = 0; \
		RF_CHECK_ASSUMPTION(place, holder, EXPR); \
	} while(0)

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

/* ----- rf_Memory ----- */

/*
  This is implemented as a free list allocator
  reference: https://www.gingerbill.org/article/2021/11/30/memory-allocation-strategies-005/
*/

struct rf_Memory_FreeListAllocationHeader {
	usize block_size;
	usize padding;
};

struct rf_Memory_FreeListNode {
	struct rf_Memory_FreeListNode *next;
	usize block_size;
};

enum rf_Memory_PlacementPolicy {
	RF_MEMORY_PLACEMENT_POLICY_FIND_FIRST,
	RF_MEMORY_PLACEMENT_POLICY_FIND_BEST
};

struct rf_Memory_FreeList {
        void *data;
	usize used;
        usize size;

	struct rf_Memory_FreeListNode *head;
	enum rf_Memory_PlacementPolicy policy;
};

void rf_Memory_free_list_free_all(struct rf_Memory_FreeList *rf_mem);
void rf_Memory_free_list_free_all(struct rf_Memory_FreeList *rf_mem) {
	struct rf_Memory_FreeListNode *first_node;
	
	rf_mem->used = 0;
	first_node = RF_CAST(rf_mem->data, struct rf_Memory_FreeListNode *);
	first_node->block_size = rf_mem->size;
	first_node->next = NULL;
	rf_mem->head = first_node;
}

void rf_Memory_free_list_init(struct rf_Memory_FreeList *rf_mem, void *new_data, usize new_data_len);
void rf_Memory_free_list_init(struct rf_Memory_FreeList *rf_mem, void *new_data, usize new_data_len) {
	rf_mem->data = new_data;
	rf_mem->size = new_data_len;
	rf_Memory_free_list_free_all(rf_mem);
}

u64 rf_Memory_calculate_padding_with_header(uintptr ptr, uintptr alignment, u64 header_size);
u64 rf_Memory_calculate_padding_with_header(uintptr ptr, uintptr alignment, u64 header_size) {
	uintptr_t p, a, modulo, padding, needed_space;

	/* assert alignment is power of 2 */
	if (alignment & 0x1)
		return 0;
	
	p = ptr;
	a = alignment;
	modulo = p & (a - 1);
	
	padding = 0;
	needed_space = 0;
	
	if (modulo != 0)
		padding = a - modulo;

	needed_space = header_size;

	if (padding < needed_space) {
		needed_space -= padding;
		if (((needed_space & (a - 1)) != 0)) {
			padding += a * (1 + (needed_space / a));
		} else {
			padding += a * (needed_space / a);
		}
	}

	return padding;
}

struct rf_Memory_FreeListNode *rf_Memory_free_list_find_first(
	struct rf_Memory_FreeList *rf_mem,
	usize size,
	usize alignment,
	usize *arg_padding,
	struct rf_Memory_FreeListNode **arg_prev_node);
struct rf_Memory_FreeListNode *rf_Memory_free_list_find_first(
	struct rf_Memory_FreeList *rf_mem,
	usize size,
	usize alignment,
	usize *arg_padding,
	struct rf_Memory_FreeListNode **arg_prev_node)
{
	/* Iterate the list and find the first free block with enough space */
	struct rf_Memory_FreeListNode *node, *prev_node;
	u64 padding, required_space;

	node = rf_mem->head;
	prev_node = NULL;
	padding = 0;

	while (node != NULL) {
		padding = rf_Memory_calculate_padding_with_header(
			RF_CAST(node, uintptr_t),
			RF_CAST(alignment, uintptr_t),
			sizeof(struct rf_Memory_FreeListAllocationHeader));
		required_space = size + padding;
		
		if (node->block_size >= required_space) {
			// i probably shouldnt have copypasted something i have no idea of
			// todo: create your own
			printf(
				"Found node of block size 0x%.8x%.8x\n",
				*(RF_CAST(&node->block_size, u32*) + 1),
				*(RF_CAST(&node->block_size, u32*) + 0));
			break;
		}
		
		prev_node = node;
		node = node->next;
	}

	if (arg_padding) *arg_padding = padding;
	if (arg_prev_node) *arg_prev_node = prev_node;
	return node;
}

struct rf_Memory_FreeListNode *rf_Memory_free_list_find_best(
	struct rf_Memory_FreeList *rf_mem,
	usize size,
	usize alignment,
	usize *arg_padding,
	struct rf_Memory_FreeListNode **arg_prev_node);
struct rf_Memory_FreeListNode *rf_Memory_free_list_find_best(
	struct rf_Memory_FreeList *rf_mem,
	usize size,
	usize alignment,
	usize *arg_padding,
	struct rf_Memory_FreeListNode **arg_prev_node)
{
	struct rf_Memory_FreeListNode *node, *prev_node, *best_node;
	usize smallest_diff, padding, required_space;

	smallest_diff = ~(RF_CAST(0, u64)); /* -1 */

	node = rf_mem->head;
	prev_node = NULL;
	best_node = NULL;
	
	padding = 0;

	while (node != NULL) {
		padding = rf_Memory_calculate_padding_with_header(
			RF_CAST(node, uintptr_t),
			RF_CAST(alignment, uintptr_t),
			sizeof(struct rf_Memory_FreeListAllocationHeader));
		required_space = size + padding;
		if (
			node->block_size >= required_space &&
			/* it.block_size? */
			(node->block_size - required_space < smallest_diff))
		{
			best_node = node;
		}
		prev_node = node;
		node = node->next;
	}

	if (arg_padding) *arg_padding = padding;
	if (arg_prev_node) *arg_prev_node = prev_node;
	return best_node;
}

void rf_Memory_free_list_node_insert(
	struct rf_Memory_FreeListNode **p_head,
	struct rf_Memory_FreeListNode *prev_node,
	struct rf_Memory_FreeListNode *new_node);
void rf_Memory_free_list_node_remove(
	struct rf_Memory_FreeListNode **p_head,
	struct rf_Memory_FreeListNode *prev_node,
	struct rf_Memory_FreeListNode *del_node);
void *rf_Memory_free_list_alloc(struct rf_Memory_FreeList *rf_mem, usize size, usize alignment);
void *rf_Memory_free_list_alloc(struct rf_Memory_FreeList *rf_mem, usize size, usize alignment) {
	usize padding, alignment_padding, required_space, remaining;
	struct rf_Memory_FreeListAllocationHeader *header_ptr;
	struct rf_Memory_FreeListNode *node, *prev_node;

	padding = 0;
	prev_node = NULL;
	node = NULL;

	if (size < sizeof(struct rf_Memory_FreeListNode))
		size = sizeof(struct rf_Memory_FreeListNode);
	if (alignment < 8)
		alignment = 8;

	if (rf_mem->policy == RF_MEMORY_PLACEMENT_POLICY_FIND_BEST)
		node = rf_Memory_free_list_find_best(rf_mem, size, alignment, &padding, &prev_node);
	else
		node = rf_Memory_free_list_find_first(rf_mem, size, alignment, &padding, &prev_node);
	if (node == NULL) {
		/* TODO: replace with REAL assert */
		RF_LAZY_ASSERT(!"Warning: Free list has no free memory");
		return NULL;
	}

	alignment_padding = padding - sizeof(struct rf_Memory_FreeListAllocationHeader);
	required_space = size + padding;
	remaining = node->block_size - required_space;

	if (remaining > 0) {
		struct rf_Memory_FreeListNode *new_node;
		new_node = RF_CAST(
			(RF_CAST(node, char*) + required_space),
			struct rf_Memory_FreeListNode*);
		new_node->block_size = remaining; /* rest? */
		rf_Memory_free_list_node_insert(&rf_mem->head, node, new_node);
	}
	
	rf_Memory_free_list_node_remove(&rf_mem->head, prev_node, node);

	header_ptr = RF_CAST(
		(RF_CAST(node, char*) + alignment_padding),
		struct rf_Memory_FreeListAllocationHeader*);
	header_ptr->block_size = required_space;
	header_ptr->padding = alignment_padding;

	rf_mem->used += required_space;

	return RF_CAST(
		(RF_CAST(header_ptr, char*) + sizeof(struct rf_Memory_FreeListAllocationHeader)),
		void*);
}

void rf_Memory_free_list_coalescence(
	struct rf_Memory_FreeList *rf_mem,
	struct rf_Memory_FreeListNode *prev_node,
	struct rf_Memory_FreeListNode *free_node);
void rf_Memory_free_list_free(struct rf_Memory_FreeList *rf_mem, void *ptr);
void rf_Memory_free_list_free(struct rf_Memory_FreeList *rf_mem, void *ptr) {
	struct rf_Memory_FreeListAllocationHeader *header;
	struct rf_Memory_FreeListNode *free_node, *node, *prev_node;

	prev_node = NULL;
	if (ptr == NULL) return;

	header = RF_CAST(
		(RF_CAST(ptr, char *) - sizeof (struct rf_Memory_FreeListAllocationHeader)),
		 struct rf_Memory_FreeListAllocationHeader *);
	free_node = RF_CAST(header, struct rf_Memory_FreeListNode *);
	free_node->block_size = header->block_size + header->padding;
	free_node->next = NULL;

	node = rf_mem->head;
	while (node != NULL) {
		if (ptr < RF_CAST(node, void *)) {
			rf_Memory_free_list_node_insert(&rf_mem->head, prev_node, free_node);
			break;
		}
		prev_node = node;
		node = node->next;
	}

	rf_mem->used -= free_node->block_size;
	rf_Memory_free_list_coalescence(rf_mem, prev_node, free_node);
}

void rf_Memory_free_list_coalescence(
	struct rf_Memory_FreeList *rf_mem,
	struct rf_Memory_FreeListNode *prev_node,
	struct rf_Memory_FreeListNode *free_node)
{
	b32 free_node_next_neq_null, free_node_plus_block_size_eq_next;
	b32 prev_node_next_neq_null, prev_node_plus_block_size_eq_free_node;
	
	free_node_next_neq_null = free_node->next != NULL;
	free_node_plus_block_size_eq_next = 
		RF_CAST((RF_CAST(free_node, char*) + free_node->block_size), void *) ==
		free_node->next;
	if (free_node_next_neq_null && free_node_plus_block_size_eq_next) {
		free_node->block_size += free_node->next->block_size;
		rf_Memory_free_list_node_remove(&rf_mem->head, free_node, free_node->next);
	}

	prev_node_next_neq_null = prev_node->next != NULL;
	prev_node_plus_block_size_eq_free_node =
		RF_CAST((RF_CAST(prev_node, char*) + prev_node->block_size), void*) ==
		free_node;
	if (prev_node_next_neq_null && prev_node_plus_block_size_eq_free_node) {
		prev_node->block_size += free_node->next->block_size;
		rf_Memory_free_list_node_remove(&rf_mem->head, prev_node, free_node);
	}
}

void rf_Memory_free_list_node_insert(
	struct rf_Memory_FreeListNode **p_head,
	struct rf_Memory_FreeListNode *prev_node,
	struct rf_Memory_FreeListNode *new_node)
{
	if (!prev_node) {
		if (*p_head != NULL) new_node->next = *p_head;
		else *p_head = new_node;
	} else {
		if (prev_node->next == NULL) {
			prev_node->next = new_node;
			new_node->next = NULL;
		} else {
			new_node->next = prev_node->next;
			prev_node->next = new_node;
		}
	}
	
}

void rf_Memory_free_list_node_remove(
	struct rf_Memory_FreeListNode **p_head,
	struct rf_Memory_FreeListNode *prev_node,
	struct rf_Memory_FreeListNode *del_node)
{
	if (!prev_node) *p_head = del_node->next;
	else prev_node->next = del_node->next;
}

void *malloc(usize size);
void free(void *mem_block);
b32 rf_Memory_test(void);
b32 rf_Memory_test(void) {
	struct rf_Memory_FreeList test_fl;
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
	
	/* Check initialization details */
	rf_Memory_free_list_init(&test_fl, backing_buffer, backing_buffer_len);
	RF_CHECK_ASSUMPTION(aok, cok, test_fl.data == backing_buffer);
	RF_CHECK_ASSUMPTION(aok, cok, test_fl.size == backing_buffer_len);
	RF_CHECK_ASSUMPTION(aok, cok, test_fl.head == test_fl.data);

	/* Testing wether find_first works */ /*{
		u8 vbyte, *pbyte;
		u16 vword, *pword;
		u32 vdword, *pdword;
		u64 vqword, *pqword;
		usize alignment;

		vbyte = 0xaa;
		vword = 0xbbcc;
		vdword = 0x11003322;
		*RF_CAST(&vqword + 0, u32*) = 0x44556677;
		*RF_CAST(&vqword + 1, u32*) = 0x8899eeff;
		alignment = 0;
		
		test_fl.policy = RF_MEMORY_PLACEMENT_POLICY_FIND_FIRST;
		
		pbyte = rf_Memory_free_list_alloc(&test_fl, sizeof vbyte, alignment);
		*pbyte = vbyte;
		RF_CHECK_ASSUMPTION(aok, cok, *pbyte == vbyte);

		pword = rf_Memory_free_list_alloc(&test_fl, sizeof vword, alignment);
		*pword = vword;
		RF_CHECK_ASSUMPTION(aok, cok, *pword == vword);
		
		pdword = rf_Memory_free_list_alloc(&test_fl, sizeof vdword, alignment);
		*pdword = vdword;
		RF_CHECK_ASSUMPTION(aok, cok, *pdword == vdword);

		pqword = rf_Memory_free_list_alloc(&test_fl, sizeof vdword, alignment);
		*pqword = vqword;
		RF_CHECK_ASSUMPTION(aok, cok, *pqword == vqword);
	}*/

	/* Testing overcommit */ {
		usize len;
		void *should_be_null;
		
		len = 0;
		len = ~len;
		
		printf(
			"requested length: %x%x\n",
			*(RF_CAST(&len, u32*) + 0),
			*(RF_CAST(&len, u32*) + 1));
		
		should_be_null = rf_Memory_free_list_alloc(&test_fl, len, 0);
		RF_CHECK_ASSUMPTION(aok, cok, should_be_null == NULL);
	}

	free(backing_buffer);
	return aok;
}

/* ----- rf_Memory ----- */

/* ----- rf_AsciiString ----- */

struct rf_AsciiString {
	char *chars;
	u64 str_len;
	u64 capacity;
};

b32 rf_AsciiString_check_assumptions(void);
void rf_AsciiString_from_const_cstr(struct rf_AsciiString *astr, char *const cstr, u64 cstr_len);

b32 rf_AsciiString_check_assumptions(void) {
	struct rf_AsciiString test;
	b32 all_ok;
	b32 curr_ok;

	all_ok = true;
	curr_ok = true;	
	
	/* Elements must be one byte long */
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof *test.chars == 1));

	return all_ok;
}

/* ----- rf_AsciiString ----- */

/* ----- rf_main ----- */
b32 rf_check_all_assumptions(void);
b32 rf_run_all_tests(void);
b32 rf_program_main(void);

b32 rf_check_all_assumptions(void) {
	b32 all_ok;
	b32 curr_ok;
	
	all_ok = true;
	curr_ok = true;

#define RF_ASS(EXPR) RF_CHECK_ASSUMPTION(all_ok, curr_ok, (EXPR))
	RF_ASS(rf_Array_check_assumptions());
	RF_ASS(rf_AsciiString_check_assumptions());
	RF_ASS(rf_Memory_test());
#undef RF_ASS
			    
 	return all_ok;		    
}


b32 rf_program_main(void) {
	/* Main dogma:
	 * 1. Acquire resources
	 * 2. Use resources in game loop
	 * 3. Free resources and return
	 */

	struct rf_Array char_array;
	char text[] = "Goodbye, cruel world!\n";
	
	if (!rf_check_all_assumptions()) {
		puts("Error: Assumption(s) were violated\n");
		return false;
	}

	/* Try printing a string, the hard way */
	rf_Array_init(
		&char_array,
		text,
		sizeof text[0],
		sizeof text);
	puts(char_array.elems);
	
	return true;
}

/* ----- rf_main ----- */

int main(void) {
	b32 retval;
	retval = rf_program_main();
       	puts("Exiting program\n");

	/* Converting true/false value into proper return value */
	retval = (retval == true)? 0 : retval;
	return retval;
}
