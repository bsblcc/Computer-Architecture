#ifndef SIM_H
#define SIM_H


int check_condition();

uint32_t fetch_next_instruciton();


uint32_t get_instruction_group(uint32_t instr);
void execute_data_processing(uint32_t instr);

struct data_processing_instr_t
{
    int operand2: 12;
    int rd: 4;
    int rn: 4;
    int s: 1;
    int opcode: 4;
    int i: 1;
    int zeros:2;
    int cond: 4;
};


#endif