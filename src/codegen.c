#include "codegen.h"
#include "parser.tab.h"   // for TOK_PLUS, TOK_MINUS, etc.

typedef struct Symbol {
    char name[64];
    int stack_offset;
    struct Symbol *next;
} Symbol;

static Symbol *symbol_table = NULL;
static int local_stack_offset = 0;
static int label_counter = 0;

static void add_symbol(const char *name, int offset) {
    Symbol *s = (Symbol*)malloc(sizeof(Symbol));
    snprintf(s->name, sizeof(s->name), "%s", name);
    s->stack_offset = offset;
    s->next = symbol_table;
    symbol_table = s;
}

static int get_symbol_offset(const char *name) {
    Symbol *curr = symbol_table;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return curr->stack_offset;
        }
        curr = curr->next;
    }
    return 0;
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

static void gen_expr(ASTNode *node, FILE *out) {
    if (!node) return;

    if (node->type == NODE_INT_LITERAL) {
        fprintf(out, "    movq $%d, %%rax\n", node->int_literal.value);
    } else if (node->type == NODE_VAR_REF) {
        int offset = get_symbol_offset(node->var_ref.name);
        if (offset != 0) {
            fprintf(out, "    movq %d(%%rbp), %%rax\n", offset);
        }
    } else if (node->type == NODE_BINARY_EXPR) {
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

static void gen_stmt(ASTNode *node, FILE *out) {
    if (!node) return;

    if (node->type == NODE_LIST) {
        gen_stmt(node->list.head, out);
        gen_stmt(node->list.next, out);
    } else if (node->type == NODE_VAR_DECL) {
        local_stack_offset -= 8;
        add_symbol(node->var_decl.name, local_stack_offset);
        if (node->var_decl.initializer) {
            gen_expr(node->var_decl.initializer, out);
            fprintf(out, "    movq %%rax, %d(%%rbp)\n", local_stack_offset);
        }
    } else if (node->type == NODE_ASSIGN) {
        int offset = get_symbol_offset(node->assign.name);
        gen_expr(node->assign.value, out);
        fprintf(out, "    movq %%rax, %d(%%rbp)\n", offset);
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
       // Evaluate argument into RAX
    gen_expr(node->foreign_call.arg, out);

    // Windows x64 ABI:
    // RCX = first argument (format string)
    // RDX = second argument (value)

    fprintf(out, "    movq %%rax, %%rdx\n");          // value → RDX
    fprintf(out, "    leaq fmt_int(%%rip), %%rcx\n"); // fmt_int → RCX
    fprintf(out, "    call %s\n", node->foreign_call.func_name);
    }
}

void generate_assembly(ASTNode *node, FILE *out) {
    if (!node) return;

    // Emit data section ONCE at the top
    fprintf(out, ".section .rdata\n");
    fprintf(out, "fmt_int:\n");
    fprintf(out, "    .asciz \"%%d\\n\"\n");
    fprintf(out, ".text\n");

    // Now emit all functions
    if (node->type == NODE_LIST) {
        generate_assembly(node->list.head, out);
        generate_assembly(node->list.next, out);
    } else if (node->type == NODE_FUNCTION) {
        clear_symbols();
        fprintf(out, ".globl %s\n", node->func.name);
        fprintf(out, "%s:\n", node->func.name);
        fprintf(out, "    pushq %%rbp\n");
        fprintf(out, "    movq %%rsp, %%rbp\n");
        fprintf(out, "    subq $64, %%rsp\n");

        gen_stmt(node->func.body, out);

        fprintf(out, "    movq %%rbp, %%rsp\n");
        fprintf(out, "    popq %%rbp\n");
        fprintf(out, "    ret\n");
    }
}
