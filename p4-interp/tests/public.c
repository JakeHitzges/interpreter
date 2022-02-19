#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "../p4-interp.h"

/* check for null-pointer handling */
START_TEST (C_error_decode_NULL_cond_valA)
{
    y86_t cpu;
    y86_inst_t inst;
    y86_reg_t valE = 0;

    memset (&inst, 0, sizeof (inst));
    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;

    inst.icode = NOP;
    valE = decode_execute (&cpu, NULL, inst, NULL);
    ck_assert (valE == 0 || valE != 0);
    ck_assert (cpu.stat == INS);
}
END_TEST

/* check decoding and execution of the HALT instruction */
START_TEST (C_func_decode_halt)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_reg_t valA = 0;
    y86_reg_t valE = 0;

    memset (&inst, 0, sizeof (inst));
    inst.icode = HALT;

    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;

    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == 0 || valE != 0);
    ck_assert (cpu.stat == HLT);
}
END_TEST

/* check decoding and execution of the NOP instruction */
START_TEST (C_func_decode_nop)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_reg_t valA = 0;
    y86_reg_t valE = 0;

    memset (&inst, 0, sizeof (inst));
    inst.icode = NOP;

    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;

    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == 0 || valE != 0);
    ck_assert (cpu.stat == AOK);
}
END_TEST

/* helper function: extract the given register value from a CPU state */
static y86_reg_t _check_get_reg (y86_t *cpu, y86_regnum_t num)
{
    return cpu->reg[num];
}

/* check decoding and execution of the IRMOVQ instruction */
START_TEST (C_func_decode_irmovq)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_reg_t valA = 0;
    y86_reg_t valE = 0;

    /* assemble test instruction and initial CPU state */
    inst.icode = IRMOVQ;
    inst.size = 10;
    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;
    inst.v = 0 + rand();
    inst.opcode = IRMOVQ << 4;
    inst.rb = (y86_regnum_t)(rand() % NOREG);		/* random register */

    /* test: load a positive value */
    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == (y86_reg_t)inst.v);
    ck_assert (cpu.sf == 0);
    ck_assert (cpu.zf == 0);
    ck_assert (_check_get_reg (&cpu, inst.rb) == 0);

    /* test: load a negative value */
    inst.v = -inst.v;
    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == (y86_reg_t)inst.v);
    ck_assert (cpu.sf == 0);
    ck_assert (cpu.zf == 0);
    ck_assert (_check_get_reg (&cpu, inst.rb) == 0);

    /* test: load a zero value */
    inst.v = 0;
    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == (y86_reg_t)inst.v);
    ck_assert (cpu.sf == 0);
    ck_assert (cpu.zf == 0);
    ck_assert (_check_get_reg (&cpu, inst.rb) == 0);
}
END_TEST

/* helper function: set the given register value in a CPU state */
static void _check_set_reg (y86_t *cpu, y86_regnum_t num, y86_reg_t val)
{
    cpu->reg[num] = val;
}

/* check decoding and execution of the OPq instructions */
START_TEST (C_func_decode_opq)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_reg_t valA = 0;
    y86_reg_t valE = 0;
    y86_reg_t expected_valE = 0;
    y86_reg_t regs[NOREG];
    uint8_t i = 0;

    memset (&inst, 0, sizeof (inst));
    inst.icode = OPQ;
    inst.size = 2;

    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;
    for (i = RAX; i < NOREG; i++)
    {
        /* set registers to random values */
        regs[i] = (((int64_t)rand()) << 32) | rand();
        _check_set_reg (&cpu, i, regs[i]);
    }

    /* addition */
    inst.ra = (y86_regnum_t)(rand() % NOREG);
    inst.rb = (y86_regnum_t)(rand() % NOREG);
    inst.op = ADD;
    expected_valE = _check_get_reg (&cpu, inst.ra) + _check_get_reg (&cpu, inst.rb);
    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == expected_valE);
    ck_assert (regs[inst.ra] == _check_get_reg (&cpu, inst.ra));
    ck_assert (regs[inst.rb] == _check_get_reg (&cpu, inst.rb));

    /* subtraction */
    inst.ra = (y86_regnum_t)(rand() % NOREG);
    inst.rb = (y86_regnum_t)(rand() % NOREG);
    inst.op = SUB;
    expected_valE = _check_get_reg (&cpu, inst.rb) - _check_get_reg (&cpu, inst.ra);
    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == expected_valE);
    ck_assert (regs[inst.ra] == _check_get_reg (&cpu, inst.ra));
    ck_assert (regs[inst.rb] == _check_get_reg (&cpu, inst.rb));

    /* bitwise AND */
    inst.ra = (y86_regnum_t)(rand() % NOREG);
    inst.rb = (y86_regnum_t)(rand() % NOREG);
    inst.op = AND;
    expected_valE = _check_get_reg (&cpu, inst.ra) & _check_get_reg (&cpu, inst.rb);
    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == expected_valE);
    ck_assert (regs[inst.ra] == _check_get_reg (&cpu, inst.ra));
    ck_assert (regs[inst.rb] == _check_get_reg (&cpu, inst.rb));

    /* bitwise XOR */
    inst.ra = (y86_regnum_t)(rand() % NOREG);
    inst.rb = (y86_regnum_t)(rand() % NOREG);
    inst.op = XOR;
    expected_valE = _check_get_reg (&cpu, inst.ra) ^ _check_get_reg (&cpu, inst.rb);
    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == expected_valE);
    ck_assert (regs[inst.ra] == _check_get_reg (&cpu, inst.ra));
    ck_assert (regs[inst.rb] == _check_get_reg (&cpu, inst.rb));
}
END_TEST

/* check handling of invalid OPq instruction */
START_TEST (C_error_decode_invalid_opq)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_reg_t valA = 0;
    y86_reg_t valE = 0;

    memset (&inst, 0, sizeof (inst));
    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;

    inst.icode = OPQ;
    inst.op = BADOP;
    valE = decode_execute (&cpu, &cond, inst, &valA);
    ck_assert (valE == 0 || valE != 0);
    ck_assert (cpu.stat == INS);
}
END_TEST

void public_tests (Suite *s)
{
    TCase *tc_public = tcase_create ("Public");
    tcase_add_test (tc_public, C_error_decode_NULL_cond_valA);
    tcase_add_test (tc_public, C_func_decode_halt);
    tcase_add_test (tc_public, C_func_decode_nop);
    tcase_add_test (tc_public, C_func_decode_irmovq);
    tcase_add_test (tc_public, C_func_decode_opq);
    tcase_add_test (tc_public, C_error_decode_invalid_opq);
    suite_add_tcase (s, tc_public);
}

