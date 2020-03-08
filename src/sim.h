#ifndef SIM_H
#define SIM_H



#define WORD_WIDTH 32
#define BYTE_WIDTH 8
#define WORD_TYPE uint32_t
#define BYTE_TYPE uint8_t
#define INSTR_TYPE WORD_TYPE
#define GET_BITS(word, index, width) ((((WORD_TYPE)word) >> index) & (((WORD_TYPE)~0) >> (WORD_WIDTH - width)))
#define SET_BITS(word, index, width, val)   \
{\
    word = (((~((((WORD_TYPE) ~0) >> (WORD_WIDTH - width)) << index)) & word) | (val << index));\
}

#define GET_CPSR_N(cs)  GET_BITS(cs.CPSR, 31, 1)
#define GET_CPSR_Z(cs) GET_BITS(cs.CPSR, 30, 1)
#define GET_CPSR_C(cs) GET_BITS(cs.CPSR, 29, 1)
#define GET_CPSR_V(cs) GET_BITS(cs.CPSR, 28, 1)

#define SET_CPSR_N(cs, flag) SET_BITS(cs.CPSR, 31, 1, flag)
#define SET_CPSR_Z(cs, flag) SET_BITS(cs.CPSR, 30, 1, flag)
#define SET_CPSR_C(cs, flag) SET_BITS(cs.CPSR, 29, 1, flag)
#define SET_CPSR_V(cs, flag) SET_BITS(cs.CPSR, 28, 1, flag)

#define GET_CONDITION_FIELD(instr) GET_BITS(instr, 28, 4)
int check_condition();

uint32_t fetch_next_instruciton();


uint32_t get_instruction_group(uint32_t instr);


void execute_data_processing(INSTR_TYPE instr);
void execute_branch_with_link(INSTR_TYPE instr);
void execute_software_interrupt(INSTR_TYPE instr);
void execute_multiply(INSTR_TYPE instr);
enum
{
    INVALID_GROUP,
    DATA_PROCESSING,
    BRANCH_WITH_LINK,   // branch and branch with link
    SOFTWARE_INTERRUPT,
    MULTIPLY,           // multiply and multiply-accumulate
};


struct data_processing_instr_t
{
    unsigned operand2: 12;
    unsigned rd: 4;
    unsigned rn: 4;
    unsigned s: 1;
    unsigned opcode: 4;
    unsigned i: 1;
    unsigned zeros:2;
    unsigned cond: 4;
};

struct branch_with_link_instr_t
{
    unsigned offset: 24;
    unsigned l: 1;
    unsigned fixed: 3;
    signed cond: 4;
};

struct software_interrupt_instr_t
{
    unsigned comment: 24;   // ignored by processor
    unsigned fixed: 4;      // all ones
    unsigned cond: 4;
};

struct multiply_instr_t
{
    unsigned rm: 4;     // operand register
    unsigned fixed_1: 4;// 1001
    unsigned rs: 4;     // operand register  
    unsigned rn: 4;     // operand register
    unsigned rd: 4;     // destination register
    unsigned s: 1;      // set condition code
    unsigned a: 1;      // accumulate code
    unsigned fixed_2: 6;// all zeros
    unsigned cond: 4; 
};
#endif