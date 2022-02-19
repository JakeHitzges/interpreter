/*
 * CS 261: Main driver
 *
 * Name: Jacob Hitzges
 */

#include "p1-check.h"
#include "p2-load.h"
#include "p3-disas.h"
#include "p4-interp.h"


//helper function to print out the register
char *registerString(int reg)
{
    switch (reg) {
        case RAX:
            return "%rax";
        case RCX:

            return "%rcx";
        case RDX:
            return "%rdx";
        case RBX:
            return "%rbx";
        case RSP:

            return "%rsp";
        case RBP:


            return "%rbp";
        case RSI:
            return "%rsi";
        case RDI:
            return "%rdi";


        case R8:
            return "%r8";
        case R9:
            return "%r9";
        case R10:

            return "%r10";
        case R11:
            return "%r11";
        case R12:
            return "%r12";
        case R13:
            return "%r13";
        case R14:
            return "%r14";


        case NOREG:
            return "";
        default:
            return "";


    }
}


//helper method to print out the instructions
void instructionString (y86_inst_t inst)
{


    switch((inst.opcode & 0xF0 ) >> 4) {

        case HALT:


            printf("halt\n%s", "");
            break;
        case NOP:
            printf("nop\n%s", "");
            break;

        case CALL:

            printf("call 0x%lx\n", inst.dest);


            break;
        case RET:
            printf("ret\n%s", "");
            break;

        case CMOV:
            printf("%s %s, %s\n", (inst.cmov == RRMOVQ ? "rrmovq" : inst.cmov == CMOVLE ? "cmovle" : inst.cmov
                                   == CMOVL ? "cmovl" : inst.cmov == CMOVE ? "cmove" : inst.cmov == CMOVNE ? "cmovne" : inst.cmov ==
                                   CMOVGE ? "cmovge" : inst.cmov == CMOVG ? "cmovg" : inst.cmov == BADCMOV ? "badcmov" : ""),
                   registerString(inst.ra), registerString(inst.rb));

            break;
        case RMMOVQ:

            printf("rmmovq %s, 0x%lx", registerString(inst.ra), inst.d);
            if (inst.rb < NOREG) {
                printf("(%s)", registerString(inst.rb));
            }
            printf("\n");

            break;
        case MRMOVQ:

            printf("mrmovq 0x%lx", inst.d);
            if (inst.rb < NOREG) {
                printf("(");
                printf("%s", registerString(inst.rb));
                printf(")");
            }

            printf(", %s\n", registerString(inst.ra));
            break;
        case IRMOVQ:

            printf("irmovq 0x%lx, %s\n", inst.v, registerString(inst.rb));

            break;

        case OPQ:
            printf("%s %s, %s\n", (inst.op == ADD ? "addq" : inst.op == SUB ? "subq" : inst.op == AND ? "andq" :
                                   inst.op == XOR ? "xorq" : inst.op == BADOP ? "" : ""), registerString(inst.ra),
                   registerString(inst.rb));



            break;

        case JUMP:
            printf("%s 0x%lx\n", (inst.jump == JMP ? "jmp" : inst.jump == JLE ? "jle" : inst.jump == JL ? "jl" :
                                  inst.jump == JE ? "je" : inst.jump == JNE ? "jne" : inst.jump == JGE ? "jge" : inst.jump == JG ?
                                  "jg" : inst.jump == BADJUMP ? "badjump" : ""), inst.dest);
            break;

        case PUSHQ:
            printf("pushq %s\n", registerString(inst.ra));


            break;

        case POPQ:
            printf("popq %s\n", registerString(inst.ra));

            break;


        case IOTRAP:
            printf("iotrap %d\n", inst.id);
            break;
        case INVALID:

            printf("Invalid opcode: 0x%x\n", inst.opcode);

            break;
        default:
            printf("Invalid opcode: 0x%x\n", inst.opcode);

            break;

    }


}

int main(int argc, char **argv)
{

    //declerations and initializations.
    bool header = false;
    bool segments = false;
    bool membrief = false;
    bool memfull = false;
    bool disas_code = false;
    bool disas_data = false;
    bool exec_normal = false;
    bool exec_trace = false;
    char **filename = (char **)malloc(sizeof(char));
    bool cond = false;
    y86_inst_t *inst = (y86_inst_t *)malloc(sizeof(y86_inst_t));
    y86_reg_t *valA = (y86_reg_t *)malloc(sizeof(y86_reg_t));
    y86_reg_t *valE = (y86_reg_t *)malloc(sizeof(y86_reg_t));


    //parses flags to tell software what to do
    bool parseBool = parse_command_line_p4(argc, argv, &header, &segments, &membrief, &memfull,
                                           &disas_code, &disas_data, &exec_normal, &exec_trace, filename);

    //checks if parsing worked
    if (!parseBool) {
        free(filename);

        usage_p4(argv);

        exit(EXIT_FAILURE);

    }

    FILE *mainFile = fopen(*filename, "r");

    //checks if files are null
    if (mainFile == NULL || filename == NULL)

    {
        if (parseBool)

        {
            printf("Failed to read file\n");


            free(filename);
        }

        free(filename);

        exit(EXIT_FAILURE);
    }

    if ((filename == NULL || argv == NULL) || !parseBool)

    {
        free(filename);

        exit(EXIT_FAILURE);
    }

    else if (filename == NULL && !header)

    {

        free(filename);

        exit(EXIT_SUCCESS);
    }

    //creates memory for cpu and header files
    y86_t *cpu = (y86_t *)malloc(sizeof(y86_t));
    elf_hdr_t *hdr = (elf_hdr_t *)malloc(sizeof(elf_hdr_t));
    bool readHead = read_header(mainFile, hdr);

    if (!readHead) {
        free(filename);

        free(hdr);

        printf("Failed to read file\n");

        exit(EXIT_FAILURE);
    }

    //dumps header
    if (header)

    {

        dump_header(*hdr);

    }

    elf_phdr_t *phdr = (elf_phdr_t *)calloc(hdr->e_num_phdr, sizeof(elf_phdr_t));


    if (readHead)

    {
        for (int i = 0; i < hdr->e_num_phdr; i++)

        {

            bool readProgramHead = read_phdr(mainFile, hdr->e_phdr_start + (sizeof(elf_phdr_t) * i),
                                             &(phdr[i]));

            if (!readProgramHead) {
                printf("Failed to read file\n");
                fclose(mainFile);
                free(filename);
                free(phdr);
                free(hdr);
                exit(EXIT_FAILURE);

            }

        }

    }

    if (segments)

    {
        dump_phdrs(hdr->e_num_phdr, phdr);

    }

    byte_t *memory = (byte_t *)malloc(MEMSIZE);

    if (membrief || memfull)

    {
        for (int i = 0; i < hdr->e_num_phdr; i++)

        {
            load_segment(mainFile, memory, phdr[i]);

        }
    }

    if (membrief)

    {
        for (int i = 0; i < hdr->e_num_phdr; i++)

        {
            dump_memory(memory, phdr[i].p_vaddr, phdr[i].p_vaddr + phdr[i].p_filesz);

        }


    }

    if (memfull)

    {
        dump_memory(memory, 0, MEMSIZE);

    }

    if (disas_code || disas_data)

    {

        for (int i = 0; i < hdr->e_num_phdr; i++) {

            load_segment(mainFile, memory, phdr[i]);
        }

    }


    if (disas_code) {

        disassemble_code(memory, phdr, hdr);
    }

    if (disas_data) {

        for (int i = 0; i < hdr->e_num_phdr; i++)

            if (phdr[i].p_type == DATA && phdr[i].p_flag == RO)

            {

                disassemble_rodata(memory, &phdr[i]);


            } else if (phdr[i].p_type == DATA && phdr[i].p_flag == RW) {

                disassemble_data(memory, &phdr[i]);
            }
    }


    if (exec_normal) {

        //loads memory
        for (int i = 0; i < hdr->e_num_phdr; i++) {


            load_segment(mainFile, memory, phdr[i]);
        }

        int count = 0;

        cpu->pc = hdr->e_entry;

        printf("Beginning execution at 0x%04x\n", hdr->e_entry);

        cpu->stat = AOK;

        //checks that the cpu status is AOK and stops otherwise
        while (cpu->stat == AOK)

        {
            //gets the instruction
            *inst = fetch(cpu, memory);

            //completes the decode, execute, memory, write back, and program counter update stages.

            if (cpu->stat == AOK)

            {
                *valE = decode_execute(cpu, &cond, *inst, valA);



                memory_wb_pc(cpu, memory, cond, *inst, *valE, *valA);


                count++;
            }

        }



        dump_cpu_state(*cpu);
        if (cpu->stat == INS || cpu->stat == ADR)


        {
            cpu->pc = -1;
        }

        printf("Total execution count: %i\n", count);

    }

    //similiar to exec_normal, but it also prints the memory state.
    if (exec_trace) {

        for (int i = 0; i < hdr->e_num_phdr; i++) {

            load_segment(mainFile, memory, phdr[i]);

        }
        int count = 0;

        cpu->pc = hdr->e_entry;

        printf("Beginning execution at 0x%04x\n", hdr->e_entry);

        cpu->stat = AOK;

        dump_cpu_state(*cpu);



        for (int i = 0; i < hdr->e_num_phdr; i++) {

            while (cpu->stat == AOK) {


                *inst = fetch(cpu, memory);


                if (cpu->stat == AOK) {



                    printf("\nExecuting: %s", "");

                    instructionString(*inst);

                    *valE = decode_execute(cpu, &cond, *inst, valA);

                    memory_wb_pc(cpu, memory, cond, *inst, *valE, *valA);

                    count++;

                } else {

                    printf("\nInvalid instruction at 0x%04lx\n", cpu->pc);
                }

                dump_cpu_state(*cpu);

            }
        }

        printf("Total execution count: %i\n\n", count);

        dump_memory(memory, 0, MEMSIZE);

    }

    //free's everything
    fclose(mainFile);
    free(memory);
    free(hdr);
    free(filename);
    free(phdr);
    free(cpu);
    free(valA);
    free(valE);
    free(inst);

    return 0;
}