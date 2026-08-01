/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PARSER_TAB_H_INCLUDED
# define YY_YY_SRC_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 2 "src\\parser.y"

    #include "ast.h"

#line 53 "src\\parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOK_IDENTIFIER = 258,          /* TOK_IDENTIFIER  */
    TOK_STRING_LITERAL = 259,      /* TOK_STRING_LITERAL  */
    TOK_CHAR_LITERAL = 260,        /* TOK_CHAR_LITERAL  */
    TOK_NUMBER = 261,              /* TOK_NUMBER  */
    TOK_FLOAT_LITERAL = 262,       /* TOK_FLOAT_LITERAL  */
    TOK_IF = 263,                  /* TOK_IF  */
    TOK_ELSE = 264,                /* TOK_ELSE  */
    TOK_SWITCH = 265,              /* TOK_SWITCH  */
    TOK_GOTO = 266,                /* TOK_GOTO  */
    TOK_RETURN = 267,              /* TOK_RETURN  */
    TOK_CALL = 268,                /* TOK_CALL  */
    TOK_JUMP = 269,                /* TOK_JUMP  */
    TOK_FOREIGN = 270,             /* TOK_FOREIGN  */
    TOK_IMPORT = 271,              /* TOK_IMPORT  */
    TOK_EXPORT = 272,              /* TOK_EXPORT  */
    TOK_DATA = 273,                /* TOK_DATA  */
    TOK_STACK = 274,               /* TOK_STACK  */
    TOK_NOALIAS = 275,             /* TOK_NOALIAS  */
    TOK_SECTION = 276,             /* TOK_SECTION  */
    TOK_PRAGMA = 277,              /* TOK_PRAGMA  */
    TOK_WORD1 = 278,               /* TOK_WORD1  */
    TOK_WORD2 = 279,               /* TOK_WORD2  */
    TOK_WORD4 = 280,               /* TOK_WORD4  */
    TOK_WORD8 = 281,               /* TOK_WORD8  */
    TOK_FLOAT4 = 282,              /* TOK_FLOAT4  */
    TOK_FLOAT8 = 283,              /* TOK_FLOAT8  */
    TOK_BITS8 = 284,               /* TOK_BITS8  */
    TOK_BITS16 = 285,              /* TOK_BITS16  */
    TOK_BITS32 = 286,              /* TOK_BITS32  */
    TOK_BITS64 = 287,              /* TOK_BITS64  */
    TOK_ALIGN1 = 288,              /* TOK_ALIGN1  */
    TOK_ALIGN2 = 289,              /* TOK_ALIGN2  */
    TOK_ALIGN4 = 290,              /* TOK_ALIGN4  */
    TOK_ALIGN8 = 291,              /* TOK_ALIGN8  */
    TOK_LPAREN = 292,              /* TOK_LPAREN  */
    TOK_RPAREN = 293,              /* TOK_RPAREN  */
    TOK_LBRACE = 294,              /* TOK_LBRACE  */
    TOK_RBRACE = 295,              /* TOK_RBRACE  */
    TOK_LBRACKET = 296,            /* TOK_LBRACKET  */
    TOK_RBRACKET = 297,            /* TOK_RBRACKET  */
    TOK_COMMA = 298,               /* TOK_COMMA  */
    TOK_SEMICOLON = 299,           /* TOK_SEMICOLON  */
    TOK_COLON = 300,               /* TOK_COLON  */
    TOK_COLONCOLON = 301,          /* TOK_COLONCOLON  */
    TOK_ASSIGN = 302,              /* TOK_ASSIGN  */
    TOK_PLUS = 303,                /* TOK_PLUS  */
    TOK_MINUS = 304,               /* TOK_MINUS  */
    TOK_STAR = 305,                /* TOK_STAR  */
    TOK_SLASH = 306,               /* TOK_SLASH  */
    TOK_PERCENT = 307,             /* TOK_PERCENT  */
    TOK_EQEQ = 308,                /* TOK_EQEQ  */
    TOK_NEQ = 309,                 /* TOK_NEQ  */
    TOK_LT = 310,                  /* TOK_LT  */
    TOK_LTE = 311,                 /* TOK_LTE  */
    TOK_GT = 312,                  /* TOK_GT  */
    TOK_GTE = 313,                 /* TOK_GTE  */
    TOK_AMP = 314,                 /* TOK_AMP  */
    TOK_PIPE = 315,                /* TOK_PIPE  */
    TOK_CARET = 316,               /* TOK_CARET  */
    TOK_TILDE = 317,               /* TOK_TILDE  */
    TOK_ERROR = 318,               /* TOK_ERROR  */
    TOK_EOF = 319                  /* TOK_EOF  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 21 "src\\parser.y"

    int int_val;
    double float_val;
    char *str_val;
    TokenKind token_kind;
    struct ASTNode *node;

#line 142 "src\\parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_PARSER_TAB_H_INCLUDED  */
