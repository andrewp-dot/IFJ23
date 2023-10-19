/***
 * @file parser.h
 * @brief Syntax / Semantics analysis header file
 * @author Simon Cagala <xcagal00@stud.fit.vutbr.cz>
 */

#include "error.h"
#include "symtable.h"
#include "debug.h"
#include <stdbool.h>
#include "lexical_analyzer.h"

/**
 * Structure containing the current state of the parser
 */
typedef struct parser_t {
    bool in_function;           /// Parser is inside a function call
    bool in_declaration;        /// Parser is inside a function declaration
    bool in_cond;               /// Parser is inside a condition statement
    bool in_loop;               /// Parser is inside a while loop
    token_T *curr_tok;          /// Currently processed token
    symtab_item_t *current_id;  /// Identifier of currently processed function
    symtab_item_t *left_id;     /// Identifier of left-side variable
    symtab_item_t *right_id;    /// Identifier of right-side function
    symtab_t global_symtab;     /// Global symbol table
    symtab_t local_symtab;      /// Local symbol table
    int err_ret_code;           /// Exit code of the program
} Parser;

/**
 * Initializes a new Parser object
 * @param Parser Parser data structure
 * @return int error code
 */
int parser_init(Parser *p);

/**
 * Free data allocated by the parser
 * @param p 
 * @return int 
 */
int parser_dispose(Parser *p);

/**
 * @brief Parser entry point
 * 
 * @return int ret_code
 */
int parse();

