#ifndef RF_MODULE_ASCII_STRING
#define RF_MODULE_ASCII_STRING

struct rf_AsciiString {
	char *chars;
	u64 str_len;
	u64 capacity;
};

b32 rf_AsciiString_test(void);
void rf_AsciiString_from_const_cstr(struct rf_AsciiString *astr, char *const cstr, u64 cstr_len);

b32 rf_AsciiString_test(void) {
	struct rf_AsciiString test;
	b32 all_ok;
	b32 curr_ok;

	all_ok = true;
	curr_ok = true;	
	
	/* Elements must be one byte long */
	RF_TEST(all_ok, curr_ok, (sizeof *test.chars == 1));

	return all_ok;
}

#endif
