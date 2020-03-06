#include <stdio.h>
#include "shell.h"
#include "sim.h"


#define WORD_WIDTH 32
#define WORD_TYPE uint32_t
#define BYTE_TYPE uint8_t
#define INSTR_TYPE WORD_TYPE
#define EXTRACT(word, index, width) ((((WORD_TYPE)word) >> index) & (((WORD_TYPE)~0) >> (WORD_WIDTH - width)))
/*#define GET_1(word, index) ((word >> index) & 0x1
#define GET_2(word, index) ((word >> index) & 0x3)
#define GET_4(word, index) ((word >> index) & 0xf)
#define GET_8(word, index) ((word >> index) & 0xff)
#define GET_16(word, index) ((word >> index) & 0xffff)*/
#define GET_CPSR_N(cs)  EXTRACT(cs.CPSR, 31, 1)
#define GET_CPSR_Z(cs) EXTRACT(cs.CPSR, 30, 1)
#define GET_CPSR_C(cs) EXTRACT(cs.CPSR, 29, 1)
#define GET_CPSR_V(cs) EXTRACT(cs.CPSR, 28, 1)

#define GET_CONDITION_FIELD(instr) GET_4(instr, 28)



enum
{
    DATA_PROCESSING,     
};

int check_condition(uint32_t condition_code)
{
    BYTE_TYPE c = GET_CPSR_C(CURRENT_STATE);
    BYTE_TYPE z = GET_CPSR_Z(CURRENT_STATE);
    BYTE_TYPE n = GET_CPSR_N(CURRENT_STATE);
    BYTE_TYPE v = GET_CPSR_V(CURRENT_STATE);
    switch (condition_code)
    {
        case (0):
        {
            // equal, Z set
            return (z == 1);
            break;
        }
        case (1):
        {
            // not equal, Z clear
            return (z == 0);
            break;
        }
        case (2):
        {
            // unsigned higher or same, C set
            return (c == 1);
            break;
        }
        case (3):
        {
            // unsigned lower, C clear
            return (c == 0);
            break;
        }
        case (4):
        {
            // negative, N set
            return (n == 1);
            break;
        }
        case (5):
        {
            // positive or zero, N clear
            return (n == 0);
            break;
        }
        case (6):
        {
            // overflow, V set
            return (v == 1);
            break;
        }
        case (7):
        {
            // no overflow, V clear
            return (v == 0);
            break;
        }
        case (8):
        {
            // unsigned higher, C set and Z clear
            return ((c == 1) && (z == 0));
            break;
        }
        case (9):
        {
            // unsigned lower or same, C clear or Z set
            return ((c == 0) || (z == 1));
            break;
        }
        case (10):
        {
            // greater or equal, N equals V
            return (n == v);
            break;
        }
        case (11):
        {
            // less than, N not equal to V
            return (n != v);
            break;
        }
        case (12):
        {
            // greater than, Z clear and (N equals V)
            return ((z == 0) && (n == v));
            break;
        }
        case (13):
        {
            // less than or equal, Z set or (N not equal to V)
            return ((z == 1) || (n != v));
            break;
        }
        case (14):
        {
            // ignored, always
            return 1;
            break;
        }
        default:
        {
            // the 1111 is reserved and should not be used.
        }
    }
}

WORD_TYPE fetch_next_instruciton()
{
    return mem_read_32(CURRENT_STATE.PC);
}

uint32_t get_instruction_group(INSTR_TYPE instr)
{
    if (EXTRACT(instr, 26, 1) == 0 && EXTRACT(instr, 27, 1) == 0 && (EXTRACT(instr, 4, 1) == 0 || EXTRACT(instr, 7, 1)))
    {
        return DATA_PROCESSING;
    }
}


void execute_data_processing(INSTR_TYPE instr)
{
    struct data_processing_instr_t *dp_instr_p = (struct data_processing_instr_t *) &instr;
    // get the second operand
    WORD_TYPE operand2_val;
    if (dp_instr_p->i == 0x0)
    {
        // the second operand is in register, but a shift/rotate operation is needed.
        uint8_t rm = EXTRACT(dp_instr_p->operand2, 0, 4);
        uint8_t shift_type = EXTRACT(dp_instr_p->operand2, 5, 2);
        uint8_t shift_amount;

        #define ISSA (EXTRACT(dp_instr_p->operand2, 4, 1) == 0x0) // Instruction Specified Shift Amount
        #define RSSA (!ISSA) // Register Specified Shift Amount
        if (ISSA)
        {
            // shift amount is an immediate value
            shift_amount = EXTRACT(dp_instr_p->operand2, 7, 5);
        }
        else    
        {
            // shift amount is the bottom byte of a register.
            WORD_TYPE rs = CURRENT_STATE.REGS[EXTRACT(dp_instr_p->operand2, 8, 4)];
            shift_amount = EXTRACT(rs, 0, 8);
        }

        
        BYTE_TYPE carry_out_bit;   // C flag in CPSR
        switch (shift_type)
        {
            case (0x0): // logical left
            {
                
                if (shift_amount != 0x0)    // if shift amount is zero, don't touch the flags
                {
                    carry_out_bit = ((CURRENT_STATE.REGS[rm] << (shift_amount - 1)) >> 31) & 0x1;
                    /*
                    if (ISSA || (shift_amount < 32))    // ISSA or RSSA with shift amount less than 32(the maximium vale ISSA can represent)
                    {
                        // carry out is the least significant bit of the discarded bits.
                        carry_out_bit = EXTRACT(CURRENT_STATE.REGS[rm], (WORD_WIDTH - shift_amount), 1);
                    }
                    else if (shift_amount == 32)    // RSSA with 32
                    {
                        // carry out bit is set to 0
                        carry_out_bit = 0x0;
                    }
                    else*/
                }
                    
                // do the left shift
                operand2_val = (CURRENT_STATE.REGS[rm] << shift_amount);
                break;
            }
            case (0x1): // logical right
            {
                break;
            }
            case (0x2): // arithmetic right
            {
                break;
            }
            case (0x3): // rotate right
            {
                break;
            }
        }
    }
    else if (dp_instr_p->i == 0x1)
    {
        // immediate value
    }
}


void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */

    INSTR_TYPE instr = fetch_next_instruciton();
    if (check_condition(GET_CONDITION_FIELD(instr)))
    {
        // execute
        uint32_t group = get_instruction_group(instr);
        switch (group)
        {
            case (DATA_PROCESSING):
            {
                execute_data_processing(instr);
                break;
            }
            default:
        }
    }
    else
    {
        // ignore, pass.
    }


    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
