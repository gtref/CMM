#include "codegen.h"
#include "parser.tab.h"   // for TOK_PLUS, TOK_MINUS, etc.
#include <stdint.h>

typedef struct Symbol {
    char name[64];
    int stack_offset;
    TypeKind type;
    struct Symbol *next;
} Symbol;

static Symbol *symbol_table = NULL;
static int local_stack_offset = 0;
static int label_counter = 0;

static Symbol* find_symbol(const char *name) {
    Symbol *curr = symbol_table;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

static void add_symbol(const char *name, int offset, TypeKind type) {
    Symbol *s = (Symbol*)malloc(sizeof(Symbol));
    snprintf(s->name, sizeof(s->name), "%s", name);
    s->stack_offset = offset;
    s->type = type;
    s->next = symbol_table;
    symbol_table = s;
}

static int get_symbol_offset(const char *name) {
    Symbol *sym = find_symbol(name);
    return sym ? sym->stack_offset : 0;
}

static TypeKind get_symbol_type(const char *name) {
    Symbol *sym = find_symbol(name);
    return sym ? sym->type : TYPE_UNKNOWN;
}

static int type_size(TypeKind type) {
    switch (type) {
        case TYPE_BITS8:
        case TYPE_WORD1:
            return 1;
        case TYPE_BITS16:
        case TYPE_WORD2:
            return 2;
        case TYPE_BITS32:
        case TYPE_WORD4:
        case TYPE_FLOAT4:
            return 4;
        case TYPE_BITS64:
        case TYPE_WORD8:
        case TYPE_FLOAT8:
            return 8;
        default:
            return 8;
    }
}

static const char* int_store_reg(TypeKind type) {
    switch (type) {
        case TYPE_BITS8:
        case TYPE_WORD1:
            return "%al";
        case TYPE_BITS16:
        case TYPE_WORD2:
            return "%ax";
        case TYPE_BITS32:
        case TYPE_WORD4:
            return "%eax";
        default:
            return "%rax";
    }
}

static const char* int_store_instr(TypeKind type) {
    switch (type) {
        case TYPE_BITS8:
        case TYPE_WORD1:
            return "movb";
        case TYPE_BITS16:
        case TYPE_WORD2:
            return "movw";
        case TYPE_BITS32:
        case TYPE_WORD4:
            return "movl";
        default:
            return "movq";
    }
}

static const char* int_load_instr(TypeKind type) {
    switch (type) {
        case TYPE_BITS8:
        case TYPE_WORD1:
            return "movsbq";
        case TYPE_BITS16:
        case TYPE_WORD2:
            return "movswq";
        case TYPE_BITS32:
        case TYPE_WORD4:
            return "movslq";
        default:
            return "movq";
    }
}

static const char* float_move_instr(TypeKind type) {
    return type == TYPE_FLOAT4 ? "movss" : "movsd";
}

static const char* float_compare_instr(TypeKind type) {
    return type == TYPE_FLOAT4 ? "ucomiss" : "ucomisd";
}

static int calculate_stack_size(ASTNode *node) {
    if (!node) return 0;
    switch (node->type) {
        case NODE_LIST:
            return calculate_stack_size(node->list.head) + calculate_stack_size(node->list.next);
        case NODE_VAR_DECL:
            return 8 + calculate_stack_size(node->var_decl.initializer);
        case NODE_IF:
            return calculate_stack_size(node->if_stmt.then_branch) + calculate_stack_size(node->if_stmt.else_branch);
        case NODE_FUNCTION:
            return calculate_stack_size(node->func.body);
        default:
            return 0;
    }
}

static void clear_symbols() {
    Symbol *curr = symbol_table;
    while (curr) {
        Symbol *tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    symbol_table = NULL;
    local_stack_offset = 0;
}

static void gen_float_literal(ASTNode *node, FILE *out) {
    if (node->inferred_type == TYPE_FLOAT4) {
        union { float f; uint32_t u; } v;
        v.f = (float)node->float_literal.value;
        fprintf(out, "    movl $0x%08x, %%eax\n", v.u);
        fprintf(out, "    movd %%eax, %%xmm0\n");
    } else {
        union { double f; uint64_t u; } v;
        v.f = node->float_literal.value;
        fprintf(out, "    movabsq $0x%016llx, %%rax\n", (unsigned long long)v.u);
        fprintf(out, "    movq %%rax, %%xmm0\n");
    }
}

static void gen_expr(ASTNode *node, FILE *out) {
    if (!node) return;

    if (node->type == NODE_INT_LITERAL) {
        fprintf(out, "    movq $%d, %%rax\n", node->int_literal.value);
    } else if (node->type == NODE_FLOAT_LITERAL) {
        gen_float_literal(node, out);
    } else if (node->type == NODE_VAR_REF) {
        int offset = get_symbol_offset(node->var_ref.name);
        TypeKind sym_type = node->inferred_type != TYPE_UNKNOWN ? node->inferred_type : get_symbol_type(node->var_ref.name);
        if (offset != 0) {
            if (sym_type == TYPE_FLOAT4 || sym_type == TYPE_FLOAT8) {
                fprintf(out, "    %s %d(%%rbp), %%xmm0\n", float_move_instr(sym_type), offset);
            } else {
                fprintf(out, "    %s %d(%%rbp), %%rax\n", int_load_instr(sym_type), offset);
            }
        }
    } else if (node->type == NODE_BINARY_EXPR) {
        if (node->inferred_type == TYPE_FLOAT4 || node->inferred_type == TYPE_FLOAT8) {
            gen_expr(node->binary_expr.right, out);
            fprintf(out, "    subq $8, %%rsp\n");
            fprintf(out, "    movq %%xmm0, (%%rsp)\n");
            gen_expr(node->binary_expr.left, out);
            fprintf(out, "    movq (%%rsp), %%xmm1\n");
            fprintf(out, "    addq $8, %%rsp\n");

            switch (node->binary_expr.op) {
                case TOK_PLUS:
                    fprintf(out, "    %s %%xmm1, %%xmm0\n", node->inferred_type == TYPE_FLOAT4 ? "addss" : "addsd");
                    break;
                case TOK_MINUS:
                    fprintf(out, "    %s %%xmm1, %%xmm0\n", node->inferred_type == TYPE_FLOAT4 ? "subss" : "subsd");
                    break;
                case TOK_STAR:
                    fprintf(out, "    %s %%xmm1, %%xmm0\n", node->inferred_type == TYPE_FLOAT4 ? "mulss" : "mulsd");
                    break;
                case TOK_SLASH:
                    fprintf(out, "    %s %%xmm1, %%xmm0\n", node->inferred_type == TYPE_FLOAT4 ? "divss" : "divsd");
                    break;
                case TOK_EQEQ:
                    fprintf(out, "    %s %%xmm1, %%xmm0\n", float_compare_instr(node->inferred_type));
                    fprintf(out, "    sete %%al\n");
                    fprintf(out, "    movzbq %%al, %%rax\n");
                    break;
                case TOK_LT:
                    fprintf(out, "    %s %%xmm1, %%xmm0\n", float_compare_instr(node->inferred_type));
                    fprintf(out, "    setb %%al\n");
                    fprintf(out, "    movzbq %%al, %%rax\n");
                    break;
                default:
                    break;
            }
        } else {
            gen_expr(node->binary_expr.right, out);
            fprintf(out, "    pushq %%rax\n");
            gen_expr(node->binary_expr.left, out);
            fprintf(out, "    popq %%rbx\n");

            switch (node->binary_expr.op) {
                case TOK_PLUS:
                    fprintf(out, "    addq %%rbx, %%rax\n");
                    break;
                case TOK_MINUS:
                    fprintf(out, "    subq %%rbx, %%rax\n");
                    break;
                case TOK_STAR:
                    fprintf(out, "    imulq %%rbx, %%rax\n");
                    break;
                case TOK_SLASH:
                    fprintf(out, "    cqto\n");
                    fprintf(out, "    idivq %%rbx\n");
                    break;
                case TOK_EQEQ:
                    fprintf(out, "    cmpq %%rbx, %%rax\n");
                    fprintf(out, "    sete %%al\n");
                    fprintf(out, "    movzbq %%al, %%rax\n");
                    break;
                case TOK_LT:
                    fprintf(out, "    cmpq %%rbx, %%rax\n");
                    fprintf(out, "    setl %%al\n");
                    fprintf(out, "    movzbq %%al, %%rax\n");
                    break;
                default:
                    break;
            }
        }
    }
}

static void gen_stmt(ASTNode *node, FILE *out) {
    if (!node) return;

    if (node->type == NODE_LIST) {
        gen_stmt(node->list.head, out);
        gen_stmt(node->list.next, out);
    } else if (node->type == NODE_VAR_DECL) {
        local_stack_offset -= 8;
        add_symbol(node->var_decl.name, local_stack_offset, node->var_decl.var_type);
        if (node->var_decl.initializer) {
            gen_expr(node->var_decl.initializer, out);
            if (node->var_decl.var_type == TYPE_FLOAT4 || node->var_decl.var_type == TYPE_FLOAT8) {
                fprintf(out, "    %s %%xmm0, %d(%%rbp)\n", float_move_instr(node->var_decl.var_type), local_stack_offset);
            } else {
                const char *instr = int_store_instr(node->var_decl.var_type);
                const char *reg = int_store_reg(node->var_decl.var_type);
                fprintf(out, "    %s %s, %d(%%rbp)\n", instr, reg, local_stack_offset);
            }
        }
    } else if (node->type == NODE_ASSIGN) {
        int offset = get_symbol_offset(node->assign.name);
        TypeKind sym_type = get_symbol_type(node->assign.name);
        gen_expr(node->assign.value, out);
        if (sym_type == TYPE_FLOAT4 || sym_type == TYPE_FLOAT8) {
            fprintf(out, "    %s %%xmm0, %d(%%rbp)\n", float_move_instr(sym_type), offset);
        } else {
            const char *instr = int_store_instr(sym_type);
            const char *reg = int_store_reg(sym_type);
            fprintf(out, "    %s %s, %d(%%rbp)\n", instr, reg, offset);
        }
    } else if (node->type == NODE_RETURN) {
        if (node->return_stmt.expr) {
            gen_expr(node->return_stmt.expr, out);
        }
        fprintf(out, "    movq %%rbp, %%rsp\n");
        fprintf(out, "    popq %%rbp\n");
        fprintf(out, "    ret\n");
    } else if (node->type == NODE_IF) {
        int lbl_else = label_counter++;
        int lbl_end = label_counter++;

        gen_expr(node->if_stmt.condition, out);
        if (node->if_stmt.condition->inferred_type == TYPE_FLOAT4 || node->if_stmt.condition->inferred_type == TYPE_FLOAT8) {
            fprintf(out, "    xorps %%xmm1, %%xmm1\n");
            fprintf(out, "    %s %%xmm1, %%xmm0\n", float_compare_instr(node->if_stmt.condition->inferred_type));
            fprintf(out, "    sete %%al\n");
            fprintf(out, "    movzbq %%al, %%rax\n");
        }
        fprintf(out, "    cmpq $0, %%rax\n");
        fprintf(out, "    je .L_else_%d\n", lbl_else);

        gen_stmt(node->if_stmt.then_branch, out);
        fprintf(out, "    jmp .L_end_%d\n", lbl_end);

        fprintf(out, ".L_else_%d:\n", lbl_else);
        if (node->if_stmt.else_branch) {
            gen_stmt(node->if_stmt.else_branch, out);
        }
        fprintf(out, ".L_end_%d:\n", lbl_end);
    } else if (node->type == NODE_GOTO) {
        fprintf(out, "    jmp %s\n", node->goto_stmt.label_name);
    } else if (node->type == NODE_LABEL) {
        fprintf(out, "%s:\n", node->label_stmt.name);
    } else if (node->type == NODE_FOREIGN_CALL) {
        TypeKind arg_type = node->foreign_call.arg ? node->foreign_call.arg->inferred_type : TYPE_BITS32;
        if (node->foreign_call.arg) {
            gen_expr(node->foreign_call.arg, out);
        }
        fprintf(out, "    leaq %s(%%rip), %%rcx\n", arg_type == TYPE_FLOAT4 || arg_type == TYPE_FLOAT8 ? "fmt_float" : "fmt_int");
        if (arg_type == TYPE_FLOAT4) {
            fprintf(out, "    cvtss2sd %%xmm0, %%xmm0\n");
        }
        if (arg_type == TYPE_BITS8 || arg_type == TYPE_BITS16 || arg_type == TYPE_BITS32 || arg_type == TYPE_BITS64 || arg_type == TYPE_WORD1 || arg_type == TYPE_WORD2 || arg_type == TYPE_WORD4 || arg_type == TYPE_WORD8) {
            fprintf(out, "    movq %%rax, %%rdx\n");
        }
        fprintf(out, "    call %s\n", node->foreign_call.func_name);
    }
}

static void generate_top_level(ASTNode *node, FILE *out);

static void generate_top_level(ASTNode *node, FILE *out) {
    if (!node) return;
    if (node->type == NODE_LIST) {
        generate_top_level(node->list.head, out);
        generate_top_level(node->list.next, out);
        return;
    }

    if (node->type == NODE_EXPORT) {
        fprintf(out, ".globl %s\n", node->symbol_decl.symbol);
    }
}

void generate_assembly(ASTNode *node, FILE *out) {
    if (!node) return;

    fprintf(out, ".section .rdata\n");
    fprintf(out, "fmt_int:\n");
    fprintf(out, "    .asciz \"%%d\\n\"\n");
    fprintf(out, "fmt_float:\n");
    fprintf(out, "    .asciz \"%%f\\n\"\n");
    fprintf(out, ".text\n");

    generate_top_level(node, out);

    if (node->type == NODE_LIST) {
        generate_assembly(node->list.head, out);
        generate_assembly(node->list.next, out);
    } else if (node->type == NODE_FUNCTION) {
        clear_symbols();
        int stack_bytes = calculate_stack_size(node->func.body);
        if (stack_bytes < 16) stack_bytes = 16;
        stack_bytes = (stack_bytes + 15) & ~15;

        fprintf(out, ".globl %s\n", node->func.name);
        fprintf(out, "%s:\n", node->func.name);
        fprintf(out, "    pushq %%rbp\n");
        fprintf(out, "    movq %%rsp, %%rbp\n");
        fprintf(out, "    subq $%d, %%rsp\n", stack_bytes);

        gen_stmt(node->func.body, out);

        fprintf(out, "    movq %%rbp, %%rsp\n");
        fprintf(out, "    popq %%rbp\n");
        fprintf(out, "    ret\n");
    }
}
