#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct SemanticSymbol {
    char name[64];
    TypeKind type;
    struct SemanticSymbol *next;
} SemanticSymbol;

static SemanticSymbol *symbols = NULL;
static int semantic_errors = 0;

static void report_type_error(int line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Type Error on line %d: ", line);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    semantic_errors++;
}

static void add_symbol(const char *name, TypeKind type, int line) {
    SemanticSymbol *curr = symbols;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            report_type_error(line, "redeclaration of '%s'", name);
            return;
        }
        curr = curr->next;
    }

    SemanticSymbol *sym = (SemanticSymbol*)malloc(sizeof(SemanticSymbol));
    if (!sym) {
        fprintf(stderr, "Out of memory in semantic checker\n");
        exit(1);
    }
    strncpy(sym->name, name, sizeof(sym->name) - 1);
    sym->name[sizeof(sym->name) - 1] = '\0';
    sym->type = type;
    sym->next = symbols;
    symbols = sym;
}

static TypeKind lookup_symbol(const char *name, int line) {
    SemanticSymbol *curr = symbols;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return curr->type;
        }
        curr = curr->next;
    }
    report_type_error(line, "undeclared variable '%s'", name);
    return TYPE_UNKNOWN;
}

static int is_float_kind(TypeKind k) {
    return k == TYPE_FLOAT4 || k == TYPE_FLOAT8;
}

static int is_int_kind(TypeKind k) {
    switch (k) {
        case TYPE_BITS8: case TYPE_BITS16: case TYPE_BITS32: case TYPE_BITS64:
        case TYPE_WORD1: case TYPE_WORD2: case TYPE_WORD4: case TYPE_WORD8:
            return 1;
        default:
            return 0;
    }
}

static int type_rank(TypeKind k) {
    switch (k) {
        case TYPE_BITS8: case TYPE_WORD1: return 8;
        case TYPE_BITS16: case TYPE_WORD2: return 16;
        case TYPE_BITS32: case TYPE_WORD4: case TYPE_FLOAT4: return 32;
        case TYPE_BITS64: case TYPE_WORD8: case TYPE_FLOAT8: return 64;
        default: return 64;
    }
}

static TypeKind common_type(TypeKind a, TypeKind b) {
    if (a == TYPE_UNKNOWN) return b;
    if (b == TYPE_UNKNOWN) return a;
    if (is_float_kind(a) || is_float_kind(b)) {
        /* prefer float8 if either is float4 or mixing int+float */
        if (a == TYPE_FLOAT4 || b == TYPE_FLOAT4) return TYPE_FLOAT8;
        return TYPE_FLOAT8;
    }
    /* integer promotion: choose the wider bit-width and prefer BITS kinds */
    int ra = type_rank(a);
    int rb = type_rank(b);
    int r = ra > rb ? ra : rb;
    switch (r) {
        case 8: return TYPE_BITS8;
        case 16: return TYPE_BITS16;
        case 32: return TYPE_BITS32;
        case 64: return TYPE_BITS64;
        default: return TYPE_BITS32;
    }
}

static TypeKind check_expr(ASTNode *node) {
    if (!node) return TYPE_UNKNOWN;

    switch (node->type) {
        case NODE_INT_LITERAL:
            node->inferred_type = TYPE_BITS32;
            return TYPE_BITS32;
        case NODE_FLOAT_LITERAL:
            node->inferred_type = TYPE_FLOAT8;
            return TYPE_FLOAT8;
        case NODE_VAR_REF: {
            TypeKind t = lookup_symbol(node->var_ref.name, node->line);
            node->inferred_type = t;
            return t;
        }
        case NODE_BINARY_EXPR: {
            TypeKind left = check_expr(node->binary_expr.left);
            TypeKind right = check_expr(node->binary_expr.right);
            if (left == TYPE_UNKNOWN || right == TYPE_UNKNOWN) {
                node->inferred_type = TYPE_UNKNOWN;
                return TYPE_UNKNOWN;
            }
            TypeKind ct = common_type(left, right);
            node->inferred_type = ct;
            return ct;
        }
        default:
            node->inferred_type = TYPE_UNKNOWN;
            return TYPE_UNKNOWN;
    }
}

static void check_node(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_LIST:
            check_node(node->list.head);
            check_node(node->list.next);
            break;
        case NODE_VAR_DECL: {
            add_symbol(node->var_decl.name, node->var_decl.var_type, node->line);
            if (node->var_decl.initializer) {
                TypeKind init_type = check_expr(node->var_decl.initializer);
                if (init_type != TYPE_UNKNOWN) {
                    /* allow widening: init narrower -> declared wider */
                    int r_init = type_rank(init_type);
                    int r_decl = type_rank(node->var_decl.var_type);
                    if (r_init > r_decl) {
                        /* allow integer literal constants that fit into narrower targets */
                        if (node->var_decl.initializer->type == NODE_INT_LITERAL) {
                            int v = node->var_decl.initializer->int_literal.value;
                            int fits = 0;
                            switch (node->var_decl.var_type) {
                                case TYPE_BITS8: fits = (v >= -128 && v <= 127); break;
                                case TYPE_BITS16: fits = (v >= -32768 && v <= 32767); break;
                                case TYPE_BITS32: fits = 1; break;
                                case TYPE_BITS64: fits = 1; break;
                                default: fits = 0; break;
                            }
                            if (fits) {
                                /* allowed */
                            } else {
                                report_type_error(node->line,
                                    "cannot initialize '%s' of type %s with wider type %s",
                                    node->var_decl.name,
                                    type_kind_to_string(node->var_decl.var_type),
                                    type_kind_to_string(init_type));
                            }
                        } else {
                        report_type_error(node->line,
                            "cannot initialize '%s' of type %s with wider type %s",
                            node->var_decl.name,
                            type_kind_to_string(node->var_decl.var_type),
                            type_kind_to_string(init_type));
                        }
                    }
                }
            }
            break;
        }
        case NODE_ASSIGN: {
            TypeKind var_type = lookup_symbol(node->assign.name, node->line);
            TypeKind value_type = check_expr(node->assign.value);
            if (var_type != TYPE_UNKNOWN && value_type != TYPE_UNKNOWN) {
                int r_var = type_rank(var_type);
                int r_val = type_rank(value_type);
                if (r_val > r_var) {
                    report_type_error(node->line,
                        "cannot assign wider type %s to '%s' of type %s",
                        type_kind_to_string(value_type),
                        node->assign.name,
                        type_kind_to_string(var_type));
                }
            }
            break;
        }
        case NODE_IF:
            check_expr(node->if_stmt.condition);
            check_node(node->if_stmt.then_branch);
            check_node(node->if_stmt.else_branch);
            break;
        case NODE_RETURN:
            if (node->return_stmt.expr) {
                check_expr(node->return_stmt.expr);
            }
            break;
        case NODE_FOREIGN_CALL:
            if (node->foreign_call.arg) {
                check_expr(node->foreign_call.arg);
            }
            break;
        case NODE_EXPORT: {
            /* Treat export as a declaration of an external symbol so
               later references don't trigger undeclared errors. Default
               to bits32 when no type information exists. */
            add_symbol(node->symbol_decl.symbol, TYPE_BITS32, node->line);
            break;
        }
        case NODE_IMPORT: {
            /* Import declares an external symbol; assume bits32 by default. */
            add_symbol(node->symbol_decl.symbol, TYPE_BITS32, node->line);
            break;
        }
        case NODE_FUNCTION:
            check_node(node->func.body);
            break;
        default:
            break;
    }
}

static void free_symbols(void) {
    SemanticSymbol *curr = symbols;
    while (curr) {
        SemanticSymbol *next = curr->next;
        free(curr);
        curr = next;
    }
    symbols = NULL;
}

int semantic_check(ASTNode *root) {
    semantic_errors = 0;
    check_node(root);
    free_symbols();
    return semantic_errors == 0;
}
