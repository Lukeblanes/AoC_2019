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
    input[a] = PROGRAM_INPUT;
    *PC += PC_JMP;
}

// Output
void OP_04(int32_t* input, int32_t *PC, int32_t a, int32_t b, int32_t c, uint32_t PC_JMP)
{
    printf("%d\n", input[a]);
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
