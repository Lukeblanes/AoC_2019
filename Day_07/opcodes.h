// Sum 
void OP_01(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
    input[c] = input[a] + input[b];
    *PC += PC_JMP; 
}

// Mul
void OP_02(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
    input[c] = input[a] * input[b];
    *PC += PC_JMP;
}

// Input
void OP_03(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
    if(g_first_iteration == 0 && g_input_flag == 0) // phase input first time and then always g_input_signal
    {
        input[a] = g_phase_num;
        g_input_flag = 1;
    }
    else // Second input
    {
        input[a] = g_input_signal;
        g_input_flag = 0;
    }
    
    *PC += PC_JMP;
}

// Output
void OP_04(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
#if GENERATE_INTCODE_OUTPUT
    printf("Output: %d\n", input[a]);
#endif

    g_input_signal = input[a];
    g_end_wait = 1;
    *PC += PC_JMP;
}

// jmp if true
void OP_05(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
    if(input[a] != 0)
        *PC = input[b];
    else
        *PC += PC_JMP;
}

// jmp if false
void OP_06(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
    if(input[a] == 0)
        *PC = input[b];
    else
        *PC += PC_JMP;
}

// set if less than
void OP_07(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
    if(input[a] < input[b])
        input[c] = 1;
    else
        input[c] = 0;
    
    *PC += PC_JMP;
}

// set if equal
void OP_08(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
    if(input[a] == input[b])
        input[c] = 1;
    else
        input[c] = 0;
    
    *PC += PC_JMP;
}
