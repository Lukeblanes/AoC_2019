// Sum 
void OP_01(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
    input[c] = input[a] + input[b];
    *PC += PC_JMP; 
}

// Mul
void OP_02(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
    input[c] = input[a] * input[b];
    *PC += PC_JMP;
}

// Input
void OP_03(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
    input[a] = g_program_input;
    *PC += PC_JMP;
}

// Output
void OP_04(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
#if GENERATE_INTCODE_OUTPUT
    printf("Output: %ld\n", input[a]);
#endif

    *PC += PC_JMP;
}

// jmp if true
void OP_05(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
    if(input[a] != 0)
        *PC = input[b];
    else
        *PC += PC_JMP;
}

// jmp if false
void OP_06(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
    if(input[a] == 0)
        *PC = input[b];
    else
        *PC += PC_JMP;
}

// set if less than
void OP_07(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
    if(input[a] < input[b])
        input[c] = 1;
    else
        input[c] = 0;
    
    *PC += PC_JMP;
}

// set if equal
void OP_08(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
    if(input[a] == input[b])
        input[c] = 1;
    else
        input[c] = 0;
    
    *PC += PC_JMP;
}

void OP_09(int64_t* input, int64_t *PC, int64_t a, int64_t b, int64_t c, uint64_t PC_JMP)
{
    g_relative_base += input[a];
    *PC += PC_JMP;
}