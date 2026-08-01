#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "../lib/arg_imports.h"
#include "ast.h"
#include "codegen.h"
#include "parser.tab.h"
#include "executable_formats/exec.h"

extern ASTNode *root;
extern FILE *yyin;

typedef int (*PFN_args_parse)(int argc, char **argv);
typedef void (*PFN_args_add_flag)(const char *short_name, const char *long_name, const char *description);
typedef void (*PFN_args_add_option)(const char *short_name, const char *long_name, const char *description);
typedef int (*PFN_args_has_flag)(const char *name);
typedef const char *(*PFN_args_get_option)(const char *name);
typedef void (*PFN_args_print_help)(const char *app_name);
typedef void (*PFN_args_free)(void);

static HMODULE g_args_dll = NULL;
static PFN_args_add_flag g_args_add_flag = NULL;
static PFN_args_add_option g_args_add_option = NULL;
static PFN_args_parse g_args_parse = NULL;
static PFN_args_has_flag g_args_has_flag = NULL;
static PFN_args_get_option g_args_get_option = NULL;
static PFN_args_print_help g_args_print_help = NULL;
static PFN_args_free g_args_free = NULL;

static int load_args_library(void) {
    static const char *candidates[] = {
        "args.dll",
        ".\\args.dll",
        ".\\build\\args.dll",
        ".\\lib\\args.dll",
        "lib\\args.dll",
        "build\\args.dll",
        NULL
    };

    if (g_args_dll) {
        return 1;
    }

    for (int i = 0; candidates[i]; ++i) {
        g_args_dll = LoadLibraryA(candidates[i]);
        if (g_args_dll) {
            g_args_add_flag = (PFN_args_add_flag)GetProcAddress(g_args_dll, "args_add_flag");
            g_args_add_option = (PFN_args_add_option)GetProcAddress(g_args_dll, "args_add_option");
            g_args_parse = (PFN_args_parse)GetProcAddress(g_args_dll, "args_parse");
            g_args_has_flag = (PFN_args_has_flag)GetProcAddress(g_args_dll, "args_has_flag");
            g_args_get_option = (PFN_args_get_option)GetProcAddress(g_args_dll, "args_get_option");
            g_args_print_help = (PFN_args_print_help)GetProcAddress(g_args_dll, "args_print_help");
            g_args_free = (PFN_args_free)GetProcAddress(g_args_dll, "args_free");

            if (g_args_add_flag && g_args_add_option && g_args_parse &&
                g_args_has_flag && g_args_get_option && g_args_print_help && g_args_free) {
                return 1;
            }

            FreeLibrary(g_args_dll);
            g_args_dll = NULL;
        }
    }

    fprintf(stderr, "Failed to load args.dll\n");
    return 0;
}

static const char *find_input_file(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            ++i;
            continue;
        }
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            continue;
        }
        if (argv[i][0] == '-') {
            continue;
        }
        return argv[i];
    }
    return NULL;
}

static int is_help_requested(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            return 1;
        }
    }
    return 0;
}

static int parse_cli_with_library(int argc, char **argv) {
    char **filtered_argv = (char **)malloc(sizeof(char *) * argc);
    if (!filtered_argv) {
        fprintf(stderr, "Out of memory while parsing arguments.\n");
        return 0;
    }

    int filtered_argc = 0;
    filtered_argv[filtered_argc++] = argv[0];

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            filtered_argv[filtered_argc++] = argv[i];
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            filtered_argv[filtered_argc++] = argv[i];
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                filtered_argv[filtered_argc++] = argv[i + 1];
                i++;
            }
        }
    }

    int ok = g_args_parse(filtered_argc, filtered_argv);
    free(filtered_argv);
    return ok;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf(
" ██████╗ ███╗   ███╗███╗   ███╗ ██████╗\n"
"██╔═══██╗████╗ ████║████╗ ████║██╔═══██╗\n"
"██║   ██║██╔████╔██║██╔████╔██║██║   ██║\n"
"██║   ██║██║╚██╔╝██║██║╚██╔╝██║██║   ██║\n"
"╚██████╔╝██║ ╚═╝ ██║██║ ╚═╝ ██║╚██████╔╝\n"
" ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝ ╚═════╝\n"
"\n"
"      > CMMC v0.0.1-pre-alpha\n"
"      > A compiler held together with caffeine\n"
"      > Memory safety sold separately\n"
"\n"
"Usage: cmmc.exe <input.cmm> [-o output.exe]\n"
);

        return 1;
    }

    if (!load_args_library()) {
        return 1;
    }

    g_args_add_flag("-h", "--help", "Show help");
    g_args_add_option("-o", "--output", "Set output executable path");

    if (is_help_requested(argc, argv)) {
        g_args_print_help("cmmc");
        g_args_free();
        return 0;
    }

    if (!parse_cli_with_library(argc, argv)) {
        fprintf(stderr, "Invalid command line arguments.\n");
        g_args_free();
        return 1;
    }

    const char *input_file = find_input_file(argc, argv);
    const char *output_exe = "a.exe";
    const char *output_from_arg = g_args_get_option("-o");
    if (!output_from_arg) {
        output_from_arg = g_args_get_option("--output");
    }
    if (output_from_arg && output_from_arg[0] != '\0') {
        output_exe = output_from_arg;
    }

    if (!input_file) {
        fprintf(stderr, "No input file provided.\n");
        g_args_free();
        return 1;
    }

    yyin = fopen(input_file, "r");
    if (!yyin) {
        perror("Failed to open source file");
        g_args_free();
        return 1;
    }

    if (yyparse() != 0) {
        fprintf(stderr, "Compilation failed due to parse errors.\n");
        fclose(yyin);
        g_args_free();
        return 1;
    }
    fclose(yyin);

    FILE *asm_out = fopen("temp_out.s", "w");
    if (!asm_out) {
        perror("Failed to open temporary assembly file");
        g_args_free();
        return 1;
    }

    generate_assembly(root, asm_out);
    fclose(asm_out);

    int res = build_executable("temp_out.s", output_exe);

    if (res == 0) {
        printf("Successfully built: %s\n", output_exe);
    } else {
        fprintf(stderr, "Linking failed.\n");
        g_args_free();
        return 1;
    }

    g_args_free();
    return 0;
}