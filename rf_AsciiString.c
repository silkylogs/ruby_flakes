#ifndef RF_MODULE_ASCII_STRING
#define RF_MODULE_ASCII_STRING

struct rf_AsciiString {
	char *chars;
	i64 str_len;
	i64 capacity;
};

b32 rf_AsciiString_test(void);

void rf_AsciiString_init_from_backing_buffer_unchecked(
	struct rf_AsciiString *rf_str,
	u8 *backing_buffer,
	isize backing_buffer_len);
void rf_AsciiString_init_from_backing_buffer_unchecked(
	struct rf_AsciiString *rf_str,
	u8 *backing_buffer,
	isize backing_buffer_len)
{
	rf_str->chars = RF_CAST(backing_buffer, char *);
	rf_str->str_len = 0;
	rf_str->capacity = backing_buffer_len;
}

b32 rf_AsciiString_init_from_backing_buffer_check(
	/* Real args */
	struct rf_AsciiString *rf_str,
	u8 *backing_buffer,
	isize backing_buffer_len,

	/* Macro args */
	const char *rf_str_expr,
	const char *backing_buffer_expr,
	const char *backing_buffer_len_expr,
	const char *file,
	int line);
b32 rf_AsciiString_init_from_backing_buffer_check(
	/* Real args */
	struct rf_AsciiString *rf_str,
	u8 *backing_buffer,
	isize backing_buffer_len,

	/* Macro args */
	const char *rf_str_expr,
	const char *backing_buffer_expr,
	const char *backing_buffer_len_expr,
	const char *file,
	int line)
{
	if (!rf_str) {
		printf("%s:%d:0: Warning: "
		       "rf_str pointer (%s) is null\n",
		       file, line, rf_str_expr);
		return false;
	} if (backing_buffer == NULL){
		printf("%s:%d:0: Warning: "
		       "backing buffer pointer (%s) is null\n",
		       file, line, backing_buffer_expr);
		return false;
	} if (backing_buffer_len <= 0) {
		printf("%s:%d:0: Warning: "
		       "Length of backing buffer (%s) is most probably invalid (%d bytes)\n",
		       file, line, backing_buffer_len_expr, RF_CAST(backing_buffer_len, i32));
		return false;
	}
	
	return true;
}
#define RF_ASCII_STRING_INIT_FROM_BACKING_BUFFER_CHECK(RF_STR_PTR, BUFFER, LEN) \
	rf_AsciiString_init_from_backing_buffer_check(			\
		(RF_STR_PTR), (BUFFER), (LEN),				\
		#RF_STR_PTR, #BUFFER, #LEN,				\
		__FILE__, __LINE__)


#if 0
struct rf_AsciiString rf_AsciiString_from_cstr_unchecked(const char *cstr, i64 cstr_len);
struct rf_AsciiString rf_AsciiString_from_cstr_unchecked(const char *cstr, i64 cstr_len) {
	struct rf_AsciiString retval;
	
	retval.chars = RF_CAST(cstr, char *);
	retval.str_len = cstr_len;
	
	return retval;
}
#endif

b32 rf_AsciiString_test(void) {
	struct rf_AsciiString test;
	b32 all_ok;
	b32 curr_ok;

	all_ok = true;
	curr_ok = true;	
	
	/* Elements must be one byte long */
	RF_TEST(all_ok, curr_ok, (sizeof *test.chars == 1));

	{
		u8 *backing_buffer;
		isize backing_buffer_len;
		struct rf_AsciiString message;

		backing_buffer_len = 16;
		backing_buffer = 0;
		RF_TEST(all_ok, curr_ok,
			!RF_ASCII_STRING_INIT_FROM_BACKING_BUFFER_CHECK(
				&message, backing_buffer, backing_buffer_len)
			&& "Should error out on a null backing buffer");
		
		backing_buffer_len = 16;
		backing_buffer = malloc(backing_buffer_len);
		RF_TEST(all_ok, curr_ok,
			RF_ASCII_STRING_INIT_FROM_BACKING_BUFFER_CHECK(
				&message, backing_buffer, backing_buffer_len)
			&& "Should init properly with proper size");

		backing_buffer_len = -16;
		RF_TEST(all_ok, curr_ok,
			!RF_ASCII_STRING_INIT_FROM_BACKING_BUFFER_CHECK(
				&message, backing_buffer, backing_buffer_len)
			&& "Should error out with an invalid size");
	}

	return all_ok;
}

#endif
