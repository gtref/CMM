#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"

// Generates target x86-64 AT&T Assembly for MinGW GCC
void generate_assembly(ASTNode *root, FILE *out);

#endif