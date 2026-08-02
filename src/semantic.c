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

static TypeKind check_expr(ASTNode *node);

static TypeKind check_expr(ASTNode *node) {
    if (!node) return TYPE_UNKNOWN;

    switch (node->type) {
        case NODE_INT_LITERAL:
            return TYPE_BITS32;
        case NODE_VAR_REF:
            return lookup_symbol(node->var_ref.name, node->line);
        case NODE_BINARY_EXPR: {
            TypeKind left = check_expr(node->binary_expr.left);
            TypeKind right = check_expr(node->binary_expr.right);
            if (left == TYPE_UNKNOWN || right == TYPE_UNKNOWN) {
                return TYPE_UNKNOWN;
            }
            if (left != right) {
                report_type_error(node->line, "type mismatch in binary expression");
                return TYPE_UNKNOWN;
            }
            return left;
        }
        default:
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
                if (init_type != TYPE_UNKNOWN && init_type != node->var_decl.var_type) {
                    report_type_error(node->line,
                        "cannot initialize '%s' of type %s with type %s",
                        node->var_decl.name,
                        type_kind_to_string(node->var_decl.var_type),
                        type_kind_to_string(init_type));
                }
            }
            break;
        }
        case NODE_ASSIGN: {
            TypeKind var_type = lookup_symbol(node->assign.name, node->line);
            TypeKind value_type = check_expr(node->assign.value);
            if (var_type != TYPE_UNKNOWN && value_type != TYPE_UNKNOWN && var_type != value_type) {
                report_type_error(node->line,
                    "cannot assign type %s to '%s' of type %s",
                    type_kind_to_string(value_type),
                    node->assign.name,
                    type_kind_to_string(var_type));
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
