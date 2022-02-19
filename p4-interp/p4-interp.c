/*
 * CS 261 PA4: Mini-ELF interpreter
 *
 * Name: Jacob Hitzges
 */

#include "p4-interp.h"


//prints the register as a string from within p4-interp file.
char *registerStringP4(int reg)
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

//prints the status of the computer as a string
char *statStringP4(int stat)
{
    switch (stat) {
        case AOK:
            return "AOK";
        case HLT:

            return "HLT";
        case INS:
            return "INS";
        case ADR:
            return "ADR";



        default:
            return "";


    }
}




//does the decode and execute parts of the semantics
y86_reg_t decode_execute(y86_t *cpu, bool *cond, y86_inst_t inst,


                         y86_reg_t *valA)
{
    cpu->stat = AOK;

    //declare and instantiate variables
    y86_reg_t valE = 0;
    y86_reg_t valB = 0;

    y86_reg_t valC = 0;
    if (cond == NULL)

    {
        cpu->stat = INS;
        // cpu->pc = -1;
        return valE; // 0

    }


    //checks the icode for the instruction
    switch (inst.icode) {

        //each case sets variables according to the semantics for decode and execute.
        case HALT:
            cpu->stat = HLT;
            cpu->pc = 0;

            cpu->zf = 0;

            cpu->sf = 0;
            cpu->of = 0;
            break;
        case NOP:


            break;
        case CMOV:
            *valA = cpu->reg[inst.ra];




            valE = cpu->reg[inst.ra];

            //sets the condition for each of the types of cmov
            switch (inst.cmov) {


                case RRMOVQ:
                    *cond = true;


                    break;
                case CMOVLE:


                    *cond = ((cpu->sf ^ cpu->of) | cpu->zf);
                    break;
                case CMOVL:

                    *cond = (cpu->sf ^ cpu->of);

                    break;
                case CMOVE:
                    *cond = (cpu->zf);

                    break;
                case CMOVNE:
                    *cond = (!cpu->zf);

                    break;

                case CMOVGE:

                    *cond = (!(cpu->sf ^ cpu->of));
                    break;
                case CMOVG:
                    *cond = (!(cpu->sf ^ cpu->of) & !cpu->zf);

                    break;

                case BADCMOV:
                    *cond = false;
                    cpu->stat = INS;



                    break;
                default:
                    *cond = false;
                    cpu->stat = INS;

                    break;
            }

            break;

        case IRMOVQ:
            valE = inst.v;
            break;


        case RMMOVQ:
            *valA = cpu->reg[inst.ra];

            valB = cpu->reg[inst.rb];

            valE = valB + inst.d;
            break;
        case MRMOVQ:

            valB = cpu->reg[inst.rb];
            valC = inst.d;
            valE = valB + valC;
            if (valE < 0 || valE >= MEMSIZE)

            {


                cpu->stat = ADR;

            }

            break;

        case OPQ:

            valB = cpu->reg[inst.rb];

            *valA = cpu->reg[inst.ra];

            switch (inst.op)

            {

                //sets valE for the different types of operations
                case ADD:
                    valE = valB + *valA;



                    break;
                case SUB:

                    valE = valB - *valA;
                    break;

                case AND:

                    valE = *valA & valB;
                    break;

                case XOR:
                    valE = *valA ^ valB;


                    break;
                case BADOP:
                    cpu->stat = INS;


                    break;


                default:
                    cpu->stat = INS;



                    break;
            }


            //sets the flags dependant on the situation and makes sure no conflicting flags are set simultaneously.
            if (valE == 0) {

                cpu->sf = false;

                cpu->zf = true;

            }

            else

                if (valE & 0x8000000000000000) {

                    cpu->sf = true;
                    cpu->zf = false;

                } else {
                    cpu->sf = false;
                }

            if (valE != ((int)valE)) {



                cpu->of = true;

            }

            else {
                cpu->of = false;
            }


            break;


        case JUMP:
            switch (inst.jump) {

                case JMP:
                    *cond = true;
                    break;
                case JLE:


                    *cond = ((cpu->sf ^ cpu->of) | cpu->zf);


                    break;
                case JL:
                    *cond = ((cpu->of ^ cpu->sf));
                    break;

                case JE:

                    *cond = cpu->zf;
                    break;

                case JNE:

                    *cond = !cpu->zf;

                    break;

                case JGE:

                    *cond = !(cpu->sf ^ cpu->of);

                    break;

                case JG:
                    *cond = (!(cpu->of ^ cpu->sf) & !cpu->zf);
                    break;

                case BADJUMP:

                    cpu->stat = INS;

                    break;

                default:
                    cpu->stat = INS;

                    break;
            }



            break;

        case CALL:

            if ((inst.dest < 0) || (inst.dest >= MEMSIZE)) {

                cpu->stat = ADR;


            }

            else {


                valE = cpu->reg[RSP] - 8;

            }
            break;

        case RET:

            valB = cpu->reg[RSP];
            *valA = cpu->reg[RSP];





            valE = cpu->reg[RSP] + 8;
            break;

        case PUSHQ:
            *valA = cpu->reg[inst.ra];





            valE = cpu->reg[RSP] - 8;
            if (valE >= MEMSIZE || valE < 0)

            {

                cpu->stat = ADR;

            }





            break;

        case POPQ:

            *valA = cpu->reg[RSP];




            valE = cpu->reg[RSP] + 8;

            if (valE < 0 || valE >= MEMSIZE) {
                cpu->stat = ADR;




            }

            break;

        case IOTRAP:
            cpu->stat = AOK;

            break;

        case INVALID:

            cpu->stat = INS;


            break;
        default:

            cpu->stat = INS;


            break;
    }


    return valE;

}

//completes the memory, write back, and program counter portions of the semantics.
void memory_wb_pc(y86_t *cpu, byte_t *memory, bool cond, y86_inst_t inst,

                  y86_reg_t valE, y86_reg_t valA)
{



    //declare and initialize variables.
    static char buffer[256];
    char tempChar;
    int toScan;
    bool flagBool = false;
    int64_t tempInt;





    //switches through the icode to get the instruction.
    switch(inst.icode) {

        //sets the memory, write back and program counter within each case.
        case HALT:
            cpu->pc = 0;


            break;
        case NOP:
            cpu->pc+= 1;
            break;


        case CMOV:

            if (cond == true) {


                cpu->reg[inst.rb] = valE;

            }
            cpu->pc+= inst.size;
            break;


        case IRMOVQ:

            cpu->reg[inst.rb] = valE;


            cpu->pc+= inst.size;


            break;
        case RMMOVQ:

            *(y86_reg_t*)&memory[valE] = valA;

            cpu->pc+= inst.size;

            break;

        case MRMOVQ:
            if (valE + 8>= MEMSIZE || valE < 0)

            {




                cpu->stat = ADR;

            } else {
                cpu->reg[inst.ra] = *(y86_reg_t*)&memory[valE];


            }

            cpu->pc+= inst.size;





            break;

        case OPQ:



            cpu->reg[inst.rb] = valE;
            cpu->pc+= 2;
            break;

        case JUMP:

            if (cond == false) {


                cpu->pc+= inst.size;
            } else {


                cpu->pc = inst.dest;


            }

            break;
        case CALL:

            if (cpu->reg[RSP] == 0) {

                cpu->stat = ADR;

                flagBool = true;


            } else {
                *(y86_reg_t*)&memory[valE] = cpu->pc + 9;
                cpu->reg[RSP] = valE;
            }
            cpu->pc= *(y86_reg_t*)&memory[cpu->pc +1];
            break;

        case RET:
            if (cpu->reg[RSP] >= MEMSIZE || cpu->reg[RSP] <= 0) {

                cpu->stat = ADR;


                flagBool = true;
            } else {
                cpu->reg[RSP] = valE;
            }





            cpu->pc= *(y86_reg_t*)&memory[valA];
            break;

        case PUSHQ:

            if (cpu->reg[RSP] >= MEMSIZE || cpu->reg[RSP] <= 0) {

                flagBool = true;

                cpu->stat = ADR;


            } else {
                *(y86_reg_t*)&memory[valE] = valA;
                cpu->reg[RSP] = valE;

            }
            cpu->pc+= 2;
            break;

        case POPQ:
            if (cpu->reg[RSP] >= MEMSIZE || cpu->reg[RSP] < 0) {
                flagBool = true;
                cpu->stat = ADR;



            } else {
                cpu->reg[inst.ra] = *(y86_reg_t*)&memory[valA];


                cpu->reg[RSP] = valE;
            }
            cpu->pc+= 2;

            break;


        case IOTRAP:


            switch (inst.id) {

                //writes from memory to buffer
                case CHAROUT:



                    tempChar = (char)memory[cpu->reg[RSI]];

                    strncat(buffer, &tempChar, 1);

                    break;

                //reads into memory
                case CHARIN:
                    tempChar = fgetc(stdin);

                    memcpy(&memory[cpu->reg[RDI]], &tempChar, 1);


                    break;

                //writes an int from memory to output buffer
                case DECOUT:
                    memcpy( &tempInt, &memory[cpu->reg[RSI]], 8);


                    snprintf(&buffer[strlen(buffer)], 100 - strlen(buffer), "%ld", tempInt);



                    break;

                //prints I/O error if a trap fails
                case DECIN:


                    toScan = scanf("%" PRId64, &tempInt);



                    if (toScan != 1) {

                        printf("I/O Error\n");


                        cpu->pc+= inst.size;

                        cpu->stat = HLT;


                        return;

                    }
                    memcpy(&memory[cpu->reg[RDI]], &tempInt, 1);


                //writes a null terminated character into the output buffer
                case STROUT:


                    snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer), "%s", &memory[cpu->reg[RSI]]);


                    break;

                //copies the output buffer and clears it.
                case FLUSH:



                    printf("%s", buffer);
                    memset(buffer, 0, sizeof(buffer));


                    break;

                case BADTRAP:
                    break;

                default:
                    break;

            }
            cpu->pc+= 1;

            break;
        case INVALID:


            break;


    }

    //resets program counter.
    if (flagBool && cpu->stat == ADR) {

        cpu->pc = 0;
    }
}




//prints the usage for help.
void usage_p4(char **argv)
{
    printf("Usage: %s <option(s)> mini-elf-file\n", argv[0]);
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
    printf("  -a      Show all with brief memory\n");
    printf("  -f      Show all with full memory\n");
    printf("  -s      Show the program headers\n");
    printf("  -m      Show the memory contents (brief)\n");
    printf("  -M      Show the memory contents (full)\n");
    printf("  -d      Disassemble code contents\n");
    printf("  -D      Disassemble data contents\n");
    printf("  -e      Execute program\n");
    printf("  -E      Execute program (trace mode)\n");
}

//parses through to see what needs to be done.
bool parse_command_line_p4(int argc, char **argv,
                           bool *header, bool *segments, bool *membrief, bool *memfull,
                           bool *disas_code, bool *disas_data,
                           bool *exec_normal, bool *exec_trace, char **filename)
{
    *membrief = false;


    *memfull = false;


    int parseInt;

    //calls usage if anything below is null.
    if (!filename || header == NULL || argv == NULL) {


        usage_p4(argv);


        exit(EXIT_SUCCESS);

        return false;

    }


    //the switch statment with the cases to look out for
    while ((parseInt = getopt(argc, argv, "+hHafsmMdDeE")) != -1) {


        switch (parseInt) {

            //calls usage for help
            case 'h':


                *header = false;
                *filename = NULL;


                return false;

            //sets header true
            case 'H':



                *header = true;
                break;

            //combination of header membrief and segments

            case 'a':


                *header = true;

                *membrief = true;
                *segments = true;

                break;

            //combination of header, memfull and segments
            case 'f':

                *header = true;


                *memfull = true;
                *segments = true;

                break;


            //sets segments true

            case 's':


                *segments = true;

                break;

            //sets membrief true
            case 'm':



                *membrief = true;

                break;

            //sets memfull true
            case 'M':


                *memfull = true;
                break;




            //sets disas code true
            case 'd':

                *disas_code = true;



                break;

            //sets disas data true
            case 'D':

                *disas_data = true;


                break;

            //sets exec normal to true
            case 'e':


                *exec_normal = true;

                break;


            //sets exec trace to true
            case 'E':

                *exec_trace = true;

                break;

            //sets everything to false/NULL if none of the cases above happen.
            default:

                *filename = NULL;
                *header = false;
                *memfull = false;

                *membrief = false;


                usage_p4(argv); // may not need this
                return false;
        }
    }


    //calls usage on error
    if ((optind != argc - 1 ) || (*exec_normal && *exec_trace) || (*membrief && *memfull)) {

        return false;

    } else {
        *filename = argv[optind];
    }






    return true;
}

//prints out the state of the cpu.
void dump_cpu_state(y86_t cpu)
{



    printf("Y86 CPU state:\n");

    printf("  %%rip: %016lx   flags: Z%i S%i O%i     %s\n", cpu.pc, cpu.zf, cpu.sf, cpu.of,
           statStringP4(cpu.stat));




    for (int i = RAX; i < NOREG; i++)


    {
        printf("  %4s: %016lx%s", registerStringP4(i), cpu.reg[i], (i == 14
                || i % 2 != 0) ? "\n" : "  ");  // switch these around, or make boolean value
    }
}