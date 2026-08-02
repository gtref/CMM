#include "ast.h"

extern int current_line;
static int node_id_counter = 1;

ASTNode* create_node(NodeType type) {
    ASTNode *node = (ASTNode*)calloc(1, sizeof(ASTNode));
    node->id = node_id_counter++;
    node->type = type;
    node->line = current_line;
    node->inferred_type = TYPE_UNKNOWN;
    return node;
}

ASTNode* create_list_node(ASTNode *head, ASTNode *next) {
    ASTNode *node = create_node(NODE_LIST);
    node->list.head = head;
    node->list.next = next;
    return node;
}

ASTNode* create_func_node(const char *name, ASTNode *body) {
    ASTNode *node = create_node(NODE_FUNCTION);
    snprintf(node->func.name, sizeof(node->func.name), "%s", name);
    node->func.body = body;
    return node;
}

ASTNode* create_var_decl_node(TypeKind var_type, const char *name, ASTNode *init) {
    ASTNode *node = create_node(NODE_VAR_DECL);
    node->var_decl.var_type = var_type;
    snprintf(node->var_decl.name, sizeof(node->var_decl.name), "%s", name);
    node->var_decl.initializer = init;
    return node;
}

ASTNode* create_assign_node(const char *name, ASTNode *val) {
    ASTNode *node = create_node(NODE_ASSIGN);
    snprintf(node->assign.name, sizeof(node->assign.name), "%s", name);
    node->assign.value = val;
    return node;
}

ASTNode* create_if_node(ASTNode *cond, ASTNode *then_b, ASTNode *else_b) {
    ASTNode *node = create_node(NODE_IF);
    node->if_stmt.condition = cond;
    node->if_stmt.then_branch = then_b;
    node->if_stmt.else_branch = else_b;
    return node;
}

ASTNode* create_goto_node(const char *label) {
    ASTNode *node = create_node(NODE_GOTO);
    snprintf(node->goto_stmt.label_name, sizeof(node->goto_stmt.label_name), "%s", label);
    return node;
}

ASTNode* create_label_node(const char *name) {
    ASTNode *node = create_node(NODE_LABEL);
    snprintf(node->label_stmt.name, sizeof(node->label_stmt.name), "%s", name);
    return node;
}

ASTNode* create_return_node(ASTNode *expr) {
    ASTNode *node = create_node(NODE_RETURN);
    node->return_stmt.expr = expr;
    return node;
}

ASTNode* create_foreign_call_node(const char *abi, const char *func, ASTNode *arg) {
    ASTNode *node = create_node(NODE_FOREIGN_CALL);
    snprintf(node->foreign_call.abi, sizeof(node->foreign_call.abi), "%s", abi);
    snprintf(node->foreign_call.func_name, sizeof(node->foreign_call.func_name), "%s", func);
    node->foreign_call.arg = arg;
    return node;
}

ASTNode* create_binary_node(TokenKind op, ASTNode *left, ASTNode *right) {
    ASTNode *node = create_node(NODE_BINARY_EXPR);
    node->binary_expr.op = op;
    node->binary_expr.left = left;
    node->binary_expr.right = right;
    return node;
}

ASTNode* create_int_node(int value) {
    ASTNode *node = create_node(NODE_INT_LITERAL);
    node->int_literal.value = value;
    return node;
}

ASTNode* create_var_ref_node(const char *name) {
    ASTNode *node = create_node(NODE_VAR_REF);
    snprintf(node->var_ref.name, sizeof(node->var_ref.name), "%s", name);
    return node;
}

ASTNode* create_export_node(const char *symbol) {
    ASTNode *node = create_node(NODE_EXPORT);
    snprintf(node->symbol_decl.symbol, sizeof(node->symbol_decl.symbol), "%s", symbol);
    return node;
}

ASTNode* create_import_node(const char *symbol) {
    ASTNode *node = create_node(NODE_IMPORT);
    snprintf(node->symbol_decl.symbol, sizeof(node->symbol_decl.symbol), "%s", symbol);
    return node;
}

const char* token_type_to_string(TokenKind type) {
    static const char *names[] = {
        "IDENTIFIER", "NUMBER", "FLOAT_LITERAL", "STRING_LITERAL", "CHAR_LITERAL",
        "IF", "ELSE", "SWITCH", "GOTO", "RETURN", "CALL", "JUMP", "FOREIGN", "IMPORT", "EXPORT",
        "DATA", "STACK", "NOALIAS", "SECTION", "PRAGMA",
        "WORD1", "WORD2", "WORD4", "WORD8", "FLOAT4", "FLOAT8",
        "BITS8", "BITS16", "BITS32", "BITS64",
        "ALIGN1", "ALIGN2", "ALIGN4", "ALIGN8",
        "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACKET", "RBRACKET",
        "COMMA", "SEMICOLON", "COLON", "COLONCOLON",
        "ASSIGN", "PLUS", "MINUS", "STAR", "SLASH", "PERCENT",
        "EQEQ", "NEQ", "LT", "LTE", "GT", "GTE",
        "AMP", "PIPE", "CARET", "TILDE",
        "ERROR", "EOF"
    };
    return names[type];
}

const char* type_kind_to_string(TypeKind type) {
    switch (type) {
        case TYPE_BITS8: return "bits8";
        case TYPE_BITS16: return "bits16";
        case TYPE_BITS32: return "bits32";
        case TYPE_BITS64: return "bits64";
        default: return "unknown";
    }
}
