#include "plutonium.h"

void print_hello(_pl_ret_t* ret, _pl_usable_args_t args) {
    ret->type_index = 2;
    ret->ptr = args.int_args.handle[0] * args.int_args.handle[1];
}

int main() {
    _pl_program_t program = pl_load_script(NULL);
    _pl_fn_t fn;
    
    fn.name = "hello";
    fn.ret.type_index = 0;

    _pl_instruction_t ins = _pl_instruction_init(_PL_INTENT_RUN_FUNC, print_hello, NULL);

    fn.instructions.handle = NULL;
    fn.instructions.size = 0;

    _pl_append_ins(&fn, ins);

    int x = 0;

    _pl_ret_t _rt;
    _rt.ptr = &x;
    _rt.type_index = 2;

    fn.ret = _rt;



    fn.args.handle = malloc(2);
    fn.args.handle[0].state = _PL_ARG_VARIABLE;
    fn.args.handle[0].ptr = 3;
    fn.args.handle[0].type_index = 2;
    fn.args.handle[1].state = _PL_ARG_VARIABLE;
    fn.args.handle[1].ptr = 3;
    fn.args.handle[1].type_index = 2;
    fn.args.size = 2;
    fn.args.handle->type_index;

    // Start the first instruction of the function
    _pl_start_fn(program, &fn);

    printf("%d\n", fn12);
}
