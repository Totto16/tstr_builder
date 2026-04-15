
#pragma once

#include <stdlib.h>
#include <string.h>

#include <tstr.h>

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202000) || __cplusplus
	#define TSTR_BUILDER_NODISCARD [[nodiscard]]
#else
    // see e.g. https://www.gnu.org/software/gnulib/manual/html_node/Attributes.html
	#define TSTR_BUILDER_NODISCARD __attribute__((__warn_unused_result__))
#endif

// simple String builder used in http_protocol, its super convenient, self implemented

typedef struct StringBuilderImpl StringBuilder;

TSTR_BUILDER_NODISCARD StringBuilder* string_builder_init(void);

// uses snprintf feature with passing NULL,0 as first two arguments to automatically determine the
// required buffer size, for more read man page
// for variadic functions its easier to use macro
// magic, attention, use this function in the right way, you have to prepare a char* that is set to
// null, then it works best! snprintf is safer then sprintf, since it guarantees some things, also
// it has a failure indicator
#define STRING_BUILDER_FORMAT_IMPL(toStore, statement, logger_fn, format, ...) \
	{ \
		char* internalBuffer = *toStore; \
		if(internalBuffer != NULL) { \
			free(internalBuffer); \
		} \
		const LibCInt toWrite = snprintf(NULL, 0, format, __VA_ARGS__) + 1; \
		internalBuffer = (char*)malloc(toWrite * sizeof(char)); \
		if(!internalBuffer) { \
			logger_fn("Couldn't allocate memory for %d bytes!\n", toWrite); \
			statement \
		} \
		const LibCInt written = snprintf(internalBuffer, toWrite, format, __VA_ARGS__); \
		if(written >= toWrite) { \
			logger_fn("snprintf did write more bytes then it had space in the buffer, available " \
			          "space: '%d', actually written: '%d'!\n", \
			          (toWrite) - 1, written); \
			free(internalBuffer); \
			statement \
		} \
		*toStore = internalBuffer; \
	} \
	if(*toStore == NULL) { \
		logger_fn("snprintf Macro gone wrong: '%s' is pointing to NULL!\n", #toStore); \
		statement \
	}

#define STRING_BUILDER_FORMAT_STRING(toStore, statement, format, ...) \
	STRING_BUILDER_FORMAT_IMPL(toStore, statement, IMPL_STDERR_LOGGER, format, __VA_ARGS__)

#define IMPL_STDERR_LOGGER(format, ...) fprintf(stderr, format, __VA_ARGS__)

// macro for appending, to used variable argument length conveniently, it uses the formatString
// (snprintf) and string_builder_append_string method under the hood

#define STRING_BUILDER_APPENDF(string_builder, statement, format, ...) \
	{ \
		if(string_builder != NULL) { \
			char* __append_buf = NULL; \
			STRING_BUILDER_FORMAT_STRING(&__append_buf, statement, format, __VA_ARGS__); \
			string_builder_append_string(string_builder, __append_buf); \
		} \
	}

// manual "variant"
typedef struct {
	bool is_error;
	union {
		tstr_static error;
	} data;
} StringBuilderResult;

TSTR_BUILDER_NODISCARD StringBuilderResult new_string_builder_result_ok(void);

TSTR_BUILDER_NODISCARD StringBuilderResult new_string_builder_result_error(tstr_static error);

#define STRING_BUILDER_RES_OK() new_string_builder_result_ok()
#define STRING_BUILDER_RES_ERR_RAW(err) new_string_builder_result_error(err)
#define STRING_BUILDER_RES_ERR(err) STRING_BUILDER_RES_ERR_RAW(TSTR_STATIC_LIT(err))

#define STRING_BUILDER_STRINGIFY(a) STRING_BUILDER_STR_IMPL(a)
#define STRING_BUILDER_STR_IMPL(a) #a

#define STRING_BUILDER_RES_ERR_UNIQUE() \
	STRING_BUILDER_RES_ERR("" __FILE__ ":" STRING_BUILDER_STRINGIFY(__LINE__))

// the actual append method, it accepts a string builder where to append and then appends the body
// string there
StringBuilderResult string_builder_append_string(StringBuilder* string_builder, char* string);

StringBuilderResult string_builder_append_string_builder(StringBuilder* string_builder,
                                                         StringBuilder** string_builder2);

// simple wrapper if just a constant string has to be appended
StringBuilderResult string_builder_append_single(StringBuilder* string_builder,
                                                 const char* static_string);

StringBuilderResult string_builder_append_tstr_static(StringBuilder* string_builder,
                                                      tstr_static static_string);

StringBuilderResult string_builder_append_tstr(StringBuilder* string_builder, const tstr* str);

TSTR_BUILDER_NODISCARD tstr string_builder_release_into_tstr(StringBuilder** string_builder);

TSTR_BUILDER_NODISCARD size_t string_builder_get_string_size(const StringBuilder* string_builder);

// free the stringbuilder
void free_string_builder(StringBuilder* string_builder);
