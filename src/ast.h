#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TokenKind is an internal enum-like int used by the AST.
   It does NOT depend on Bison headers. */
typedef int TokenKind;

typedef enum {
    TYPE_UNKNOWN,
    TYPE_BITS8,
    TYPE_BITS16,
    TYPE_BITS32,
    TYPE_BITS64,
    TYPE_WORD1,
    TYPE_WORD2,
    TYPE_WORD4,
    TYPE_WORD8,
    TYPE_FLOAT4,
    TYPE_FLOAT8,
} TypeKind;

typedef enum {
    NODE_PROGRAM,
    NODE_LIST,
    NODE_FUNCTION,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_IF,
    NODE_GOTO,
    NODE_LABEL,
    NODE_RETURN,
    NODE_FOREIGN_CALL,
    NODE_INT_LITERAL,
    NODE_FLOAT_LITERAL,
    NODE_VAR_REF,
    NODE_BINARY_EXPR,
    NODE_EXPORT,
    NODE_IMPORT
} NodeType;

typedef struct ASTNode {
    int id;
    int line;
    NodeType type;
    TypeKind inferred_type;
    union {
        // Linked list node for statement and declaration lists
        struct {
            struct ASTNode *head;
            struct ASTNode *next;
        } list;

        // Function: name() { body }
        struct {
            char name[64];
            struct ASTNode *body;
        } func;

        // Variable Declaration: bits32 $var = expr;
        struct {
            char name[64];
            TypeKind var_type;
            struct ASTNode *initializer;
        } var_decl;

        // Assignment: $var = expr;
        struct {
            char name[64];
            struct ASTNode *value;
        } assign;

        // Conditional: if (expr) { then } else { else }
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_stmt;

        // Jump: goto label;
        struct {
            char label_name[64];
        } goto_stmt;

        // Label: label:
        struct {
            char name[64];
        } label_stmt;

        // Return statement: return expr;
        struct {
            struct ASTNode *expr;
        } return_stmt;

        // Foreign call: foreign "C" printf(...);
        struct {
            char abi[32];
            char func_name[64];
            struct ASTNode *arg;
        } foreign_call;

        // Integer Literal
        struct {
            int value;
        } int_literal;

        // Floating Literal
        struct {
            double value;
        } float_literal;

        // Variable Reference
        struct {
            char name[64];
        } var_ref;

        // Binary Expr
        struct {
            TokenKind op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary_expr;

        // Export/Import
        struct {
            char symbol[64];
        } symbol_decl;
    };
} ASTNode;

// AST Allocation functions
ASTNode* create_node(NodeType type);
ASTNode* create_list_node(ASTNode *head, ASTNode *next);
ASTNode* create_func_node(const char *name, ASTNode *body);
ASTNode* create_var_decl_node(TypeKind var_type, const char *name, ASTNode *init);
ASTNode* create_assign_node(const char *name, ASTNode *val);
ASTNode* create_if_node(ASTNode *cond, ASTNode *then_b, ASTNode *else_b);
ASTNode* create_float_node(double value);
ASTNode* create_goto_node(const char *label);
ASTNode* create_label_node(const char *name);
ASTNode* create_return_node(ASTNode *expr);
ASTNode* create_foreign_call_node(const char *abi, const char *func, ASTNode *arg);
ASTNode* create_binary_node(TokenKind op, ASTNode *left, ASTNode *right);
ASTNode* create_int_node(int value);
ASTNode* create_var_ref_node(const char *name);
ASTNode* create_export_node(const char *symbol);
ASTNode* create_import_node(const char *symbol);

const char* token_type_to_string(TokenKind type);
const char* type_kind_to_string(TypeKind type);

#endif
