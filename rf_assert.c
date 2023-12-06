#ifndef RF_MODULE_ASSERT
#define RF_MODULE_ASSERT

void rf_assert(
	b64 expr,
	const char *src_filename,
	u32 line_number,
	const char *expr_str);
void rf_assert(
	b64 expr,
	const char *src_filename,
	u32 line_number,
	const char *expr_str)
{
	if (!(expr)) {					
		printf(						
			"%s:%d:0: Error: Assertion failed: \"%s\"\n", 
			src_filename, line_number, expr_str);
		exit(1);
	}
}

#define RF_ASSERT(EXPR) rf_assert((EXPR), __FILE__, __LINE__, (#EXPR))
#define RF_TODO(MESSAGE) RF_ASSERT(!"Not implemented:" MESSAGE)

#endif
