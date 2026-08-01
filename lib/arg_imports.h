#ifndef ARGS_IMPORTS_H
#define ARGS_IMPORTS_H

#ifdef _WIN32
#  define ARGS_API __declspec(dllimport)
#else
#  define ARGS_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Rule definition functions
ARGS_API void args_add_flag(const char* short_name, const char* long_name, const char* description);
ARGS_API void args_add_option(const char* short_name, const char* long_name, const char* description);

// Core parsing engine
ARGS_API int  args_parse(int argc, char** argv);

// Data retrieval functions
ARGS_API int         args_has_flag(const char* name);
ARGS_API const char* args_get_option(const char* name);
ARGS_API void        args_print_help(const char* app_name);

// Memory cleanup
ARGS_API void args_free(void);

#ifdef __cplusplus
}
#endif

#endif // ARGS_IMPORTS_H