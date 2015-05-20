#include <stdio.h>

#include "lily_vm.h"
#include "lily_value.h"
#include "inttypes.h"
#include "lily_seed.h"

int lily_double_eq(lily_vm_state *vm, int *depth, lily_value *left,
        lily_value *right)
{
    return (left->value.doubleval == right->value.doubleval);
}

void lily_double_to_i(lily_vm_state *vm, uint16_t argc, uint16_t *code)
{
    lily_value **vm_regs = vm->vm_regs;
    double double_val = vm_regs[code[0]]->value.doubleval;
    lily_value *result_reg = vm_regs[code[1]];

    result_reg->flags = 0;
    result_reg->value.integer = (int64_t)double_val;
}

static const lily_func_seed to_i =
    {NULL, "to_i", dyna_function, "function to_i(double => integer)", lily_double_to_i};

int lily_double_setup(lily_symtab *symtab, lily_class *cls)
{
    cls->dynaload_table = &to_i;
    return 1;
}
