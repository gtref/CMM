#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "../../lib/libtcc.h"

typedef TCCState *(*PFN_tcc_new)(void);
typedef void (*PFN_tcc_delete)(TCCState *);
typedef int (*PFN_tcc_set_output_type)(TCCState *, int);
typedef int (*PFN_tcc_add_file)(TCCState *, const char *);
typedef int (*PFN_tcc_output_file)(TCCState *, const char *);
typedef int (*PFN_tcc_set_options)(TCCState *, const char *);

static HMODULE g_libtcc = NULL;
static PFN_tcc_new g_tcc_new = NULL;
static PFN_tcc_delete g_tcc_delete = NULL;
static PFN_tcc_set_output_type g_tcc_set_output_type = NULL;
static PFN_tcc_add_file g_tcc_add_file = NULL;
static PFN_tcc_output_file g_tcc_output_file = NULL;
static PFN_tcc_set_options g_tcc_set_options = NULL;

static int load_libtcc(void) {
    if (g_libtcc) return 1;

    g_libtcc = LoadLibraryA("lib\\libtcc.dll");
    if (!g_libtcc) {
        g_libtcc = LoadLibraryA(".\\lib\\libtcc.dll");
    }
    if (!g_libtcc) {
        fprintf(stderr, "Failed to load libtcc.dll\n");
        return 0;
    }

    g_tcc_new = (PFN_tcc_new)GetProcAddress(g_libtcc, "tcc_new");
    g_tcc_delete = (PFN_tcc_delete)GetProcAddress(g_libtcc, "tcc_delete");
    g_tcc_set_output_type = (PFN_tcc_set_output_type)GetProcAddress(g_libtcc, "tcc_set_output_type");
    g_tcc_add_file = (PFN_tcc_add_file)GetProcAddress(g_libtcc, "tcc_add_file");
    g_tcc_output_file = (PFN_tcc_output_file)GetProcAddress(g_libtcc, "tcc_output_file");
    g_tcc_set_options = (PFN_tcc_set_options)GetProcAddress(g_libtcc, "tcc_set_options");

    if (!g_tcc_new || !g_tcc_delete || !g_tcc_set_output_type || !g_tcc_add_file || !g_tcc_output_file || !g_tcc_set_options) {
        fprintf(stderr, "Failed to resolve libtcc symbols\n");
        FreeLibrary(g_libtcc);
        g_libtcc = NULL;
        return 0;
    }

    return 1;
}

int build_executable(const char *asm_path, const char *output_exe) {
    if (!load_libtcc()) {
        return 1;
    }

    TCCState *s = g_tcc_new();
    if (!s) {
        fprintf(stderr, "tcc_new failed\n");
        return 1;
    }

    g_tcc_set_options(s, "-nostdlib");
    g_tcc_set_options(s, "-Llib");

    if (g_tcc_set_output_type(s, TCC_OUTPUT_OBJ) < 0) {
        fprintf(stderr, "tcc_set_output_type failed\n");
        g_tcc_delete(s);
        return 1;
    }

    if (g_tcc_add_file(s, asm_path) < 0) {
        fprintf(stderr, "Failed to add assembly file: %s\n", asm_path);
        g_tcc_delete(s);
        return 1;
    }

    const char *obj_path = "temp_out.obj";
    if (g_tcc_output_file(s, obj_path) < 0) {
        fprintf(stderr, "Failed to write object file\n");
        g_tcc_delete(s);
        return 1;
    }

    g_tcc_delete(s);

    char command[512];
    snprintf(command, sizeof(command),
        "C:\\Users\\ronan\\mingw64\\bin\\gcc %s -o %s",
        obj_path, output_exe);

    return system(command);
}