
#pragma once

#ifndef _TSTRING_BUILDER_IMPL_INTERNAL__
	#error "can only be used internally"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202000) || __cplusplus
	#define NODISCARD [[nodiscard]]
	#define MAYBE_UNUSED [[maybe_unused]]
#else
    // see e.g. https://www.gnu.org/software/gnulib/manual/html_node/Attributes.html
	#define NODISCARD __attribute__((__warn_unused_result__))
	#define MAYBE_UNUSED __attribute__((__unused__))
#endif

// cool trick from here:
// https://stackoverflow.com/questions/777261/avoiding-unused-variables-warnings-when-using-assert-in-a-release-build
#ifdef NDEBUG
	#define assert(x) /* NOLINT(readability-identifier-naming) */ \
		do { \
			UNUSED((x)); \
		} while(false)

	#define UNREACHABLE() \
		do { \
			fprintf(stderr, "[%s %s:%d]: UNREACHABLE\n", __func__, __FILE__, __LINE__); \
			exit(EXIT_FAILURE); \
		} while(false)

	#define OOM_ASSERT(value, message) \
		do { \
			if(!(value)) { \
				fprintf(stderr, "[%s %s:%d]: OOM: %s\n", __func__, __FILE__, __LINE__, (message)); \
				exit(EXIT_FAILURE); \
			} \
		} while(false)

#else
	#include <assert.h>

	#define UNREACHABLE() \
		do { \
			assert(false && "UNREACHABLE"); /* NOLINT(cert-dcl03-c,misc-static-assert) */ \
		} while(false)

	#define OOM_ASSERT(value, message) \
		do { \
			assert((value) && (message)); /* NOLINT(cert-dcl03-c,misc-static-assert) */ \
		} while(false)

#endif

#define UNUSED(v) ((void)(v))

#ifdef __cplusplus
}
#endif
