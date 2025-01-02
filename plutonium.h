#ifndef __plutonium_h__
#define __plutonium_h__

#define _PL_RAW_BYTES_TO_TP(final, raw, type) \
        type final;\
        memcpy(&final, raw, sizeof(type));

#define _PL_DYNAMIC_ARR(type) \
    struct {                  \
        type* handle;         \
        size_t size;          \
    }
    
#define _PL_ITERATOR(type) \
    struct {               \
        type* start;       \
        type* end;         \
    }

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

// Struct for handling strings with multiple formats
typedef struct {
    /* We also want other character
        formats other than ASCII */
    void *handle; // Pointer to character data
    size_t size;  // Size of the string or data
} _pl_str_t;

// Type structure
typedef struct {
    const char *name; // Type name
    size_t size;      // Size of the type
} _pl_type_t;

// Array of types
typedef struct {
    _pl_type_t *handle; // Pointer to an array of types
    size_t size;        // Number of types
} _pl_types_array_t;

// Variable structure (incomplete, left as is)
typedef struct {
    // Define variable-related data here
    const char* name;
    size_t type_index; // types[type_index]

    void* ptr;
} _pl_var_t;

typedef struct {
    _pl_var_t* handle;
    size_t size;
} _pl_stack_t;

// Name for the dynamically allocated variables
typedef struct {
    const char* name;
    size_t type_index; // types[type_index]

    void* ptr;
    size_t size;
} _pl_arena_t;

typedef struct {
    _pl_arena_t* handle;
    size_t size;
} _pl_heap_t;

typedef struct {
    void *ptr;
    size_t type_index;
} _pl_ret_t;

typedef enum {
    _PL_ARG_VARIABLE,
    _PL_ARG_REFFERENCE,
    _PL_ARG_ARENA
} _pl_arg_state_t;

typedef struct {
    void* ptr;
    size_t type_index;
} _pl_prog_ptr_t;

typedef struct {
    void* handle;
    size_t type_index;
    size_t size;
} _pl_prog_arena_t;

typedef struct {
    void* ptr;
    size_t type_index;
    _pl_arg_state_t state;
} _pl_arg_t;

typedef struct {
    _pl_arg_t *handle;
    size_t size;
} _pl_args_t;

typedef struct {
    bool empty;
    _PL_DYNAMIC_ARR(int) int_args;
    _PL_DYNAMIC_ARR(const char*) str_args;
    _PL_DYNAMIC_ARR(double) float_args;
    _PL_DYNAMIC_ARR(_pl_prog_ptr_t) ptrs;
    _PL_DYNAMIC_ARR(_pl_prog_arena_t) arenas;
} _pl_usable_args_t;

typedef struct {
    void* ptr;
    size_t type_index;
} _pl_local_var_t;

// emulated kind of fake stack thing
typedef struct {
    void* ptr;
    size_t* type_index_array;
    size_t size;
} _pl_local_stack_t;

typedef enum {
    _PL_INTENT_SET, // set a local or global variable
    _PL_INTENT_RUN_FUNC // or run a function imported from C
} _pl_ins_intention_t;

typedef void (*_pl_imported_fn_t)(_pl_ret_t* ret, _pl_usable_args_t args);

typedef struct {
    _pl_ins_intention_t intent;

    void* ptr;
    void* to;

    _pl_imported_fn_t import; // you will have to import this function
    // else if you don't it will stay NULL
} _pl_instruction_t;

typedef struct {
    _pl_instruction_t *handle;
    size_t size;
} _pl_ins_array_t;

// Function structure
typedef struct {
    const char *name;        // Function name
    _pl_args_t args;

    // function contents
    _pl_ins_array_t instructions;

    _pl_local_stack_t local_stack; // gets deleted on function end
    _pl_ret_t ret;
} _pl_fn_t;

// Array of functions
typedef struct {
    _pl_fn_t *handle; // Pointer to an array of functions
    size_t size;     // Number of functions
} _pl_fn_array_t;

typedef struct {
    _PL_ITERATOR(_pl_fn_t)  fns;
    _PL_ITERATOR(_pl_var_t) vars;
} _pl_module_t;

typedef struct {
    _pl_module_t* handle;
    size_t size;
} _pl_modules_t;

// Program structure
typedef struct {
    _pl_fn_array_t fn_array; // Array of functions
    _pl_types_array_t types; // Array of types
    _pl_modules_t   modules; // Array of modules

    _pl_stack_t stack; // Collection of all the variables
    _pl_heap_t  heap;  // Collection of all the dynamically allocated arrays
    
    _pl_ret_t ret; // Return value
} _pl_program_t;

// Function declarations
void _pl_types_array_set(_pl_types_array_t *ptr, _pl_types_array_t to);

_pl_types_array_t _pl_types_init();

_pl_ret_t _pl_init_retv(_pl_program_t program, size_t type_index);

_pl_local_stack_t _pl_local_stack_append(_pl_local_stack_t stack, void *ptr, size_t type_index);
void _pl_local_stack_clear(_pl_local_stack_t *stack);
_pl_local_var_t _pl_access_local_var(_pl_program_t program, _pl_local_stack_t stack, size_t index);

_pl_program_t pl_load_script(const char *src);

// the way the ptr is used is determined by the intent enum
// if RUN_FUNC IS used x should always be set to NULL
_pl_instruction_t _pl_instruction_init(_pl_ins_intention_t intent, void* ptr, void* x);

double _pl_bytes_to_double(void* bytes);

void _pl_start_fn(_pl_program_t program, _pl_fn_t *fn);

void pl_end_fn(_pl_program_t *program, _pl_fn_t fn);

size_t pl_sizeof(_pl_program_t program, _pl_var_t var);

_pl_usable_args_t _pl_get_usable_args(_pl_args_t *args, _pl_types_array_t *types);

void _pl_free_usable_args(_pl_usable_args_t *usable_args);

void _pl_append_ins(_pl_fn_t* fn, _pl_instruction_t ins);
void _pl_append_fn(_pl_program_t *program, _pl_fn_t fn);
void _pl_append_type(_pl_program_t *program, _pl_type_t type);
void _pl_append_module(_pl_program_t *program, _pl_module_t module);
void _pl_append_stack(_pl_program_t *program, _pl_var_t var);
void _pl_append_heap(_pl_program_t *program, _pl_arena_t arena);

#endif // __plutonium_h__