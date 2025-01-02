#include "plutonium.h"

#define __PL_APPEND(arr, x, tp)                         \
    do {                                                \
        tp* __new_handle = (tp*)realloc((arr).handle,   \
            ((arr).size + 1) * sizeof(tp));             \
        if (__new_handle == NULL) {                     \
            fprintf(stderr,                            \
                "PLUTONIUM APPEND: Memory allocation failed\n"); \
            exit(EXIT_FAILURE);                         \
        }                                               \
        (arr).handle = __new_handle;                    \
        (arr).handle[(arr).size] = (x);                 \
        (arr).size += 1;                                \
    } while (0)


#define __PL_APPEND_STR(arr, x) __PL_APPEND(arr, x, const char*)

// Define default types
const static char *default_types[] = {
    "void",
    "str",
    "int", // i32
    "u8",
    "u16",
    "u32",
    "u64",
    "i8",
    "i16",
    "i32",
    "i64",
    "f32",
    "f64",
};

// Function to map type names to sizes
static size_t _pl_get_type_size(const char *type_name)
{
    if (strcmp(type_name, "void") == 0) return 0;
    if (strcmp(type_name, "str") == 0) return sizeof(void*); // Pointer size
    if (strcmp(type_name, "int") == 0 || strcmp(type_name, "i32") == 0) return 4;
    if (strcmp(type_name, "u8") == 0 || strcmp(type_name, "i8") == 0) return 1;
    if (strcmp(type_name, "u16") == 0 || strcmp(type_name, "i16") == 0) return 2;
    if (strcmp(type_name, "u32") == 0) return 4;
    if (strcmp(type_name, "u64") == 0 || strcmp(type_name, "i64") == 0) return 8;
    if (strcmp(type_name, "f32") == 0) return 4;
    if (strcmp(type_name, "f64") == 0) return 8;
    fprintf(stderr, "Unknown type: %s\n", type_name);
    exit(EXIT_FAILURE);
}

/*static const char* _pl_get_va_list_format(int i) {
    if (i < 0 || i >= (sizeof(default_types) / sizeof(default_types[0]))) {
        fprintf(stderr, "Invalid index: %d. Returning NULL.\n", i);
        return NULL; // Return NULL for invalid indices
    }

    // Map default_types to va_list format specifiers
    const char* format = NULL;
    if (strcmp(default_types[i], "void") == 0) {
        format = "";
    } else if (strcmp(default_types[i], "str") == 0) {
        format = "%s"; // String
    } else if (strcmp(default_types[i], "int") == 0 || strcmp(default_types[i], "i32") == 0) {
        format = "%d"; // Integer
    } else if (strcmp(default_types[i], "u8") == 0 || strcmp(default_types[i], "u16") == 0 ||
               strcmp(default_types[i], "u32") == 0 || strcmp(default_types[i], "u64") == 0) {
        format = "%u"; // Unsigned integer
    } else if (strcmp(default_types[i], "i8") == 0 || strcmp(default_types[i], "i16") == 0 ||
               strcmp(default_types[i], "i32") == 0 || strcmp(default_types[i], "i64") == 0) {
        format = "%d"; // Signed integer
    } else if (strcmp(default_types[i], "f32") == 0 || strcmp(default_types[i], "f64") == 0) {
        format = "%f"; // Floating point
    } else {
        fprintf(stderr, "Unknown type: %s. Returning NULL.\n", default_types[i]);
    }

    return format;
}*/

// Set elements of one type array to another
void _pl_types_array_set(_pl_types_array_t *ptr, _pl_types_array_t to)
{
    if (ptr->size > to.size || ptr->size == 0 || to.size == 0) {
        fprintf(stderr, "_pl_types_array_set error!\n");
        return;
    }

    for (size_t i = 0; i < to.size; ++i) {
        ptr->handle[i] = to.handle[i];
    }
}

// Initialize a types array
_pl_types_array_t _pl_types_init()
{
    _pl_types_array_t self;

    self.size = sizeof(default_types) / sizeof(default_types[0]);

    self.handle = (_pl_type_t *)malloc(self.size * sizeof(_pl_type_t));
    if (self.handle == NULL) {
        fprintf(stderr, "Memory allocation failed for _pl_types_init\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < self.size; ++i) {
        self.handle[i].name = default_types[i];
        self.handle[i].size = _pl_get_type_size(default_types[i]);
    }

    return self;
}

_pl_ret_t _pl_init_retv(_pl_program_t program, size_t type_index)

{
    fprintf(stderr, "%d Debug: Function %s, Line %d\n", type_index, __func__, __LINE__);
    if (program.types.handle == NULL) {
        fprintf(stderr, "Program types handle is NULL.\n");
        exit(EXIT_FAILURE);
    }

    _pl_ret_t self;
    self.type_index = type_index;
    fprintf(stderr, "%d Debug: Function %s, Line %d\n", type_index, __func__, __LINE__);
    self.ptr = malloc(program.types.handle[type_index].size);
    if(!self.ptr) {
        fprintf(stderr, "Memory allocation failed for _pl_init_retv\n");
        exit(EXIT_FAILURE);
    }
fprintf(stderr, "%d Debug: Function %s, Line %d\n", type_index, __func__, __LINE__);
    return self;
}

_pl_local_stack_t _pl_local_stack_append(_pl_local_stack_t stack, void *ptr, size_t type_index) {
    // Resize the stack's memory to accommodate the new entry
    stack.ptr = realloc(stack.ptr, (stack.size + 1) * sizeof(void *));
    if (stack.ptr == NULL) {
        fprintf(stderr, "Memory allocation failed for local stack\n");
        exit(EXIT_FAILURE);
    }

    stack.type_index_array = realloc(stack.type_index_array, (stack.size + 1) * sizeof(size_t));
    if (stack.type_index_array == NULL) {
        fprintf(stderr, "Memory allocation failed for type index array\n");
        exit(EXIT_FAILURE);
    }

    // Add the new item
    ((void **)stack.ptr)[stack.size] = ptr;
    stack.type_index_array[stack.size] = type_index;
    stack.size++;

    return stack;
}

// Example to clear the local stack (to simulate end of function cleanup)
void _pl_local_stack_clear(_pl_local_stack_t *stack) {
    free(stack->ptr);
    free(stack->type_index_array);

    stack->ptr = NULL;
    stack->type_index_array = NULL;
    stack->size = 0;
}

_pl_local_var_t _pl_access_local_var(_pl_program_t program, _pl_local_stack_t stack, size_t index)
{
    size_t type_size_sum = 0;
    for(int i = 0; i < index; ++i) {
        // ts gettin to complex
        type_size_sum += program.types.handle[stack.type_index_array[i]].size;
    }

    _pl_local_var_t self;
    self.ptr = stack.ptr + type_size_sum;
    self.type_index = stack.type_index_array[index];

    return self;
}

// Load a script and initialize program data
_pl_program_t pl_load_script(const char *src)
{
    _pl_program_t self;
    self.types = _pl_types_init();

    return self;
}

_pl_instruction_t _pl_instruction_init(_pl_ins_intention_t intent, void *ptr, void *x)
{
    _pl_instruction_t self;

    self.intent = intent;

    if (!ptr && self.intent != _PL_INTENT_SET) {
        fprintf(stderr, "Failed to load pointers ptr and x in _pl_instruction_init\n");
        return self;  // If ptr is NULL, return uninitialized `self`, but that's not ideal in this case.
    }

    // Handle the intent case
    if (self.intent == _PL_INTENT_SET) {
        self.ptr = ptr;
        self.to  = x;
    } else {
        self.import = (_pl_imported_fn_t)ptr;
        self.ptr = NULL;  // Ensure ptr is NULL if it's not being used.
    }

    return self;  // Always return the initialized object.
}

double _pl_bytes_to_double(void *bytes)
{
    _PL_RAW_BYTES_TO_TP(__ret, bytes, double);
    return __ret;
}

void _pl_start_fn(_pl_program_t program, _pl_fn_t *fn) {
    // Allocate a buffer to build the format string dynamically
    /*char fmt[256] = {0}; // Make sure this is large enough for your needs
    char addon[16] = {0}; // Temporary buffer for each format specifier
    size_t offset = 0; // Offset to track where to append in the format string */
    fprintf(stderr, "afawfwaawfawfaw: Function %s, Line %d\n", __func__, __LINE__);

    if(fn->ret.type_index) {
        fprintf(stderr, "Debug: Function %s, Line %d\n", __func__, __LINE__);
        fn->ret = _pl_init_retv(program, fn->ret.type_index);
        fprintf(stderr, "Debug: Function %s, Line %d\n", __func__, __LINE__);
        if (!fn->ret.ptr) {
            fprintf(stderr, "Failed to initialize return value.\n");
            return;
        }
    }

    // Generate the format string dynamically
    /*for (size_t i = 0; i < fn->args.size; ++i) {
        if (fn->args.handle[i].state == _PL_ARG_VARIABLE) {
            snprintf(addon, sizeof(addon), "%s", _pl_get_va_list_format(fn->args.handle[i].type_index));
        } else {
            snprintf(addon, sizeof(addon), "%%p");
        }

        // Append the current specifier to the format string
        strncat(fmt, addon, sizeof(fmt) - strlen(fmt) - 1); 
    }*/

    // array of ptrs
    /*_PL_DYNAMIC_ARR(void*) args;

    for(int i = 0; i < fn->args.size; ++i) {
        __PL_APPEND(args, fn->args.handle[i].ptr, void*);
    }*/
   fprintf(stderr, "Debug: Function %s, Line %d\n", __func__, __LINE__);

    _pl_usable_args_t usable_args = _pl_get_usable_args(&fn->args, &program.types);
    
    if(usable_args.empty) {
        printf("No Arguments required.\n");
    }

    fprintf(stderr, "Debug: Function %s, Line %d\n", __func__, __LINE__);

    // Start each instruction
    for (size_t i = 0; i < fn->instructions.size; ++i) {
        if(fn->instructions.handle[i].intent == _PL_INTENT_SET) {
            fn->instructions.handle[i].ptr = fn->instructions.handle[i].to;
        } else {
        fprintf(stderr, "Debug: Function %s, Line %d\n", __func__, __LINE__);
            fn->instructions.handle[i].import(&fn->ret, usable_args);
        }
    }
}

void pl_end_fn(_pl_program_t *program, _pl_fn_t fn)
{
    program->ret.type_index = fn.ret.type_index;

    if(fn.ret.type_index == 0) return;

    program->ret.ptr = malloc(program->types.handle[program->ret.type_index].size);
    
    if(program->ret.ptr == NULL) {
        fprintf(stderr, "PLUTONIUM APPEND: Memory allocation failed\n"); \
        return;
    }

    _pl_local_stack_clear(&fn.local_stack);
    
    program->ret.ptr = fn.ret.ptr;
}

// Get the size of a variable type
size_t pl_sizeof(_pl_program_t program, _pl_var_t var)
{
    return program.types.handle[var.type_index].size;
}

_pl_usable_args_t _pl_get_usable_args(_pl_args_t *args, _pl_types_array_t *types)
{
    _pl_usable_args_t usable_args;
    // Initialize sizes to 0
    usable_args.int_args.size = 0;
    usable_args.str_args.size = 0;
    usable_args.float_args.size = 0;
    usable_args.ptrs.size = 0;
    usable_args.arenas.size = 0;

    if(!args->handle) {
        usable_args.empty = true;
        return usable_args;
    }

    fprintf(stderr, "  Debug: Function %s, Line %d\n", __func__, __LINE__);

    // Count arguments by type to allocate memory
    for (size_t i = 0; i < args->size; ++i) {
        size_t type_index = args->handle[i].type_index;
        const char *type_name = types->handle[type_index].name;

        
    fprintf(stderr, "  Debug: Function %s, Line %d\n", __func__, __LINE__);

        if (strcmp(type_name, "int") == 0) {
            usable_args.int_args.size++;
        } else if (strcmp(type_name, "str") == 0) {
            usable_args.str_args.size++;
        } else if (strcmp(type_name, "f64") == 0 || strcmp(type_name, "f32") == 0) {
            usable_args.float_args.size++;
        } else if (strcmp(type_name, "ptr") == 0) {
            usable_args.ptrs.size++;
        } else if (strcmp(type_name, "arena") == 0) {
            usable_args.arenas.size++;
        }
    }

    fprintf(stderr, "  Debug: Function %s, Line %d\n", __func__, __LINE__);

    // Allocate memory for dynamic arrays
    usable_args.int_args.handle = (int *)malloc(usable_args.int_args.size * sizeof(int));
    usable_args.str_args.handle = (const char **)malloc(usable_args.str_args.size * sizeof(const char *));
    usable_args.float_args.handle = (double *)malloc(usable_args.float_args.size * sizeof(double));
    usable_args.ptrs.handle = (_pl_prog_ptr_t *)malloc(usable_args.ptrs.size * sizeof(_pl_prog_ptr_t));
    usable_args.arenas.handle = (_pl_prog_arena_t *)malloc(usable_args.arenas.size * sizeof(_pl_prog_arena_t));

    // Reset counters
    size_t int_index = 0, str_index = 0, float_index = 0, ptr_index = 0, arena_index = 0;

    // Store arguments into appropriate arrays
    for (size_t i = 0; i < args->size; ++i) {
        size_t type_index = args->handle[i].type_index;
        const char *type_name = types->handle[type_index].name;

        if (strcmp(type_name, "int") == 0) {
            _PL_RAW_BYTES_TO_TP(value, args->handle[i].ptr, int);
            usable_args.int_args.handle[int_index++] = value;
        } else if (strcmp(type_name, "str") == 0) {
            usable_args.str_args.handle[str_index++] = (const char *)args->handle[i].ptr;
        } else if (strcmp(type_name, "f64") == 0 || strcmp(type_name, "f32") == 0) {
            _PL_RAW_BYTES_TO_TP(value, args->handle[i].ptr, double);
            usable_args.float_args.handle[float_index++] = value;
        } else if (strcmp(type_name, "ptr") == 0) {
            usable_args.ptrs.handle[ptr_index++] = *(_pl_prog_ptr_t *)args->handle[i].ptr;
        } else if (strcmp(type_name, "arena") == 0) {
            usable_args.arenas.handle[arena_index++] = *(_pl_prog_arena_t *)args->handle[i].ptr;
        }
    }

    return usable_args;
}

// Free memory allocated in _usable_args_t
void _pl_free_usable_args(_pl_usable_args_t *usable_args) {
    free(usable_args->int_args.handle);
    free(usable_args->str_args.handle);
    free(usable_args->float_args.handle);
    free(usable_args->ptrs.handle);
    free(usable_args->arenas.handle);
}

void _pl_append_ins(_pl_fn_t* fn, _pl_instruction_t ins) {
    __PL_APPEND(fn->instructions, ins, _pl_instruction_t);
}

// Append a function to the function array in the program
void _pl_append_fn(_pl_program_t *program, _pl_fn_t fn) {
    __PL_APPEND(program->fn_array, fn, _pl_fn_t);
}

// Append a type to the types array in the program
void _pl_append_type(_pl_program_t *program, _pl_type_t type) {
    __PL_APPEND(program->types, type, _pl_type_t);
}

// Append a module to the modules array in the program
void _pl_append_module(_pl_program_t *program, _pl_module_t module) {
    __PL_APPEND(program->modules, module, _pl_module_t);
}

// Append a variable to the stack in the program
void _pl_append_stack(_pl_program_t *program, _pl_var_t var) {
    __PL_APPEND(program->stack, var, _pl_var_t);
}

// Append an arena to the heap in the program
void _pl_append_heap(_pl_program_t *program, _pl_arena_t arena) {
    __PL_APPEND(program->heap, arena, _pl_arena_t);
}