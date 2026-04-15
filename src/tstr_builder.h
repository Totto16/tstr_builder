
#pragma once

#include <stdlib.h>
#include <string.h>

#include <tstr.h>

// in here there are several utilities that are used across all .h and .c files
#include "./_impl/utils.h"

// simple String builder used in http_protocol, its super convenient, self implemented

typedef struct StringBuilderImpl StringBuilder;

NODISCARD StringBuilder* string_builder_init(void);

// macro for appending, to used variable argument length conveniently, it uses the formatString
// (snprintf) and string_builder_append_string method under the hood

#define STRING_BUILDER_APPENDF(string_builder, statement, format, ...) \
	{ \
		if(string_builder != NULL) { \
			char* __append_buf = NULL; \
			FORMAT_STRING(&__append_buf, statement, format, __VA_ARGS__); \
			string_builder_append_string(string_builder, __append_buf); \
		} \
	}

// manual "variant"
typedef struct {
	bool is_error;
	union {
		tstr_static error;
	} data;
} StringResult;

NODISCARD StringResult new_string_result_ok(void);

NODISCARD StringResult new_string_result_error(tstr_static error);

#define STRING_RES_OK() new_string_result_ok()
#define STRING_RES_ERR_RAW(err) new_string_result_error(err)
#define STRING_RES_ERR(err) STRING_RES_ERR_RAW(TSTR_STATIC_LIT(err))

#define STRING_RES_ERR_UNIQUE() STRING_RES_ERR("" __FILE__ ":" STRINGIFY(__LINE__))

// the actual append method, it accepts a string builder where to append and then appends the body
// string there
StringResult string_builder_append_string(StringBuilder* string_builder, char* string);

StringResult string_builder_append_string_builder(StringBuilder* string_builder,
                                                  StringBuilder** string_builder2);

// simple wrapper if just a constant string has to be appended
StringResult string_builder_append_single(StringBuilder* string_builder, const char* static_string);

StringResult string_builder_append_tstr_static(StringBuilder* string_builder,
                                               tstr_static static_string);

StringResult string_builder_append_tstr(StringBuilder* string_builder, const tstr* str);

NODISCARD tstr string_builder_release_into_tstr(StringBuilder** string_builder);

NODISCARD size_t string_builder_get_string_size(const StringBuilder* string_builder);

// free the stringbuilder
void free_string_builder(StringBuilder* string_builder);
