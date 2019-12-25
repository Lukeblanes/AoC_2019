#define NUM_OPCODES 9

typedef struct{
    // Internal state with flags
    int64_t PC;
    int64_t A;
    int64_t B;
    int64_t C;
    int64_t relative_base;
    uint32_t pause_flag;
    uint32_t end_program;
    int32_t step_mode;

    // Input 
    int32_t input_elems_tracker;
    int32_t input_elems_size;
    int32_t* input_elems; 

    // Output
    int32_t output_elems_tracker;
    int32_t output_elems_size; 
    int32_t* output_elems;

    // used for day 25
    uint32_t output_pause_flag;

    // Program
    uint32_t input_size;
    int64_t* input;

} IntCodePC;

IntCodePC* icpc_create(int64_t* program, int32_t program_size, uint32_t input_size, uint32_t output_size, uint32_t step_mode)
{
    IntCodePC* icpc = malloc(sizeof *icpc);
    icpc->PC = 0;
    icpc->A = 0;
    icpc->B = 0;
    icpc->C = 0;
    icpc->relative_base = 0;
    icpc->pause_flag = 0;
    icpc->end_program = 0;
    icpc->step_mode = step_mode;

    icpc->input_elems_tracker = 0;
    icpc->input_elems_size = input_size;
    icpc->input_elems = malloc(sizeof(int32_t) * input_size);

    icpc->output_elems_tracker = 0;
    icpc->output_elems_size = output_size;
    icpc->output_elems = malloc(sizeof(int32_t) * output_size);

    // day 25 var
    icpc->output_pause_flag = 0;

    icpc->input_size = program_size;
    icpc->input = malloc(sizeof(int64_t) * program_size);
    memcpy(icpc->input, program, program_size * sizeof(int64_t));

    return icpc;
}

// Sum 
void OP_01(IntCodePC* icpc, uint64_t PC_JMP)
{
    icpc->input[icpc->C] = icpc->input[icpc->A] + icpc->input[icpc->B];
    icpc->PC += PC_JMP; 

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

// Mul
void OP_02(IntCodePC* icpc, uint64_t PC_JMP)
{
    icpc->input[icpc->C] = icpc->input[icpc->A] * icpc->input[icpc->B];
    icpc->PC += PC_JMP;

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

// Input
void OP_03(IntCodePC* icpc, uint64_t PC_JMP)
{
    icpc->input[icpc->A] = icpc->input_elems[icpc->input_elems_tracker++];
    icpc->PC += PC_JMP;

    if(icpc->input_elems_tracker == icpc->input_elems_size)
    {
        printf("Paused due to inserting all elements\n");
        icpc->input_elems_tracker = 0;
        icpc->pause_flag = 1;
    }

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

// Output
void OP_04(IntCodePC* icpc, uint64_t PC_JMP)
{
#if GENERATE_INTCODE_OUTPUT
    // char version
    printf("%c", (char)icpc->input[icpc->A]);
#endif

    icpc->output_elems[icpc->output_elems_tracker++] = icpc->input[icpc->A];

    if(icpc->output_elems_tracker >= 8 && icpc->output_elems[icpc->output_elems_tracker-1] == '\n') // if it outputs \n we check if command was written
    {
        uint32_t aux_tracker = icpc->output_elems_tracker-2; // where last letter of output_elems is 
        char COMMAND_CHECK[] = "Command?"; 
        for (uint32_t i = 7; i >= 0; i--)
        {
            if(icpc->output_elems[aux_tracker] != COMMAND_CHECK[i]) {
                break;
            } else if(icpc->output_elems[aux_tracker] == COMMAND_CHECK[i] && i == 0) {
                icpc->pause_flag = 1;
                icpc->output_pause_flag = 1;
                return;
            }
            aux_tracker--;
        }
    }

    if(icpc->output_elems_tracker == icpc->output_elems_size) // 
    {
        icpc->output_elems_tracker = 0; // reset tracker
        icpc->pause_flag = 1;
    }

    icpc->PC += PC_JMP;

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

// jmp if true
void OP_05(IntCodePC* icpc, uint64_t PC_JMP)
{
    if(icpc->input[icpc->A] != 0)
        icpc->PC = icpc->input[icpc->B];
    else
        icpc->PC += PC_JMP;

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

// jmp if false
void OP_06(IntCodePC* icpc, uint64_t PC_JMP)
{
    if(icpc->input[icpc->A] == 0)
        icpc->PC = icpc->input[icpc->B];
    else
        icpc->PC += PC_JMP;

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

// set if less than
void OP_07(IntCodePC* icpc, uint64_t PC_JMP)
{
    if(icpc->input[icpc->A] < icpc->input[icpc->B])
        icpc->input[icpc->C] = 1;
    else
        icpc->input[icpc->C] = 0;
    
    icpc->PC += PC_JMP;

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

// set if equal
void OP_08(IntCodePC* icpc, uint64_t PC_JMP)
{
    if(icpc->input[icpc->A] == icpc->input[icpc->B])
        icpc->input[icpc->C] = 1;
    else
        icpc->input[icpc->C] = 0;
    
    icpc->PC += PC_JMP;

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

void OP_09(IntCodePC* icpc, uint64_t PC_JMP)
{
    icpc->relative_base += icpc->input[icpc->A];
    icpc->PC += PC_JMP;

    if(icpc->step_mode == 1)
        icpc->pause_flag = 1;
}

void (*OP_functions[NUM_OPCODES])(IntCodePC*, uint64_t) =
                                     { OP_01, OP_02, OP_03, OP_04, OP_05, OP_06, OP_07, OP_08, OP_09};
uint64_t OP_jumpValue[NUM_OPCODES] = { 4,     4,     2,     2,     3,     3,     4,     4,     2};

int64_t get_mode_index(IntCodePC* icpc)
{
    int64_t OP = icpc->input[icpc->PC];
    if(OP >= 20000)      icpc->C = icpc->input[icpc->PC + 3] + icpc->relative_base; // Relative mode
    else if(OP >= 10000) icpc->C = icpc->PC+3;                                      // Immediate mode
    else                 icpc->C = icpc->input[icpc->PC+3];                         // Position mode

    OP %= 10000;
    if(OP >= 2000)       icpc->B = icpc->input[icpc->PC + 2] + icpc->relative_base;
    else if(OP >= 1000)  icpc->B = icpc->PC+2;
    else                 icpc->B = icpc->input[icpc->PC+2];

    OP %= 1000;
    if(OP >= 200)        icpc->A = icpc->input[icpc->PC + 1] + icpc->relative_base;
    else if(OP >= 100)   icpc->A = icpc->PC+1;
    else                 icpc->A = icpc->input[icpc->PC+1];

    return OP % 100;
}

void run_intcode_program(IntCodePC *icpc)
{
    while(icpc->input[icpc->PC] != 99 && icpc->pause_flag == 0)
    {
        int64_t OP = get_mode_index(icpc);

        if (OP < 1 || OP > NUM_OPCODES)
        {
            printf("Something went wrong. PC = %ld. input[PC] = %ld. OP = %ld. a,b,c = (%ld, %ld), (%ld, %ld), (%ld, %ld)\n",
                    icpc->PC, icpc->input[icpc->PC], OP, icpc->input[icpc->PC+1], icpc->input[icpc->A], icpc->input[icpc->PC+2], icpc->input[icpc->B], icpc->input[icpc->PC+3], icpc->input[icpc->C]);
            exit(0);
        }
        else
        {
            OP_functions[OP-1](icpc, OP_jumpValue[OP-1]);
        }
    }

    if(icpc->input[icpc->PC] == 99)
        icpc->end_program = 1;

    // Reset flags
    icpc->pause_flag = 0;

    if(icpc->end_program == 1)
        icpc->relative_base = 0;
}

