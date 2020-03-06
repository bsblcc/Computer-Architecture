#include <stdio.h>
#include "shell.h"
#include "sim.h"


#define WORD_WIDTH 32
#define WORD_TYPE uint32_t
#define BYTE_TYPE uint8_t
#define INSTR_TYPE WORD_TYPE
#define GET_BITS(word, index, width) ((((WORD_TYPE)word) >> index) & (((WORD_TYPE)~0) >> (WORD_WIDTH - width)))
#define SET_BITS(word, index, width, val)   \
{\
    word = (((~((((WORD_TYPE) ~0) >> (WORD_WIDTH - width)) << index)) & word) | (val << index));\
}
/*#define GET_1(word, index) ((word >> index) & 0x1
#define GET_2(word, index) ((word >> index) & 0x3)
#define GET_4(word, index) ((word >> index) & 0xf)
#define GET_8(word, index) ((word >> index) & 0xff)
#define GET_16(word, index) ((word >> index) & 0xffff)*/
#define GET_CPSR_N(cs)  GET_BITS(cs.CPSR, 31, 1)
#define GET_CPSR_Z(cs) GET_BITS(cs.CPSR, 30, 1)
#define GET_CPSR_C(cs) GET_BITS(cs.CPSR, 29, 1)
#define GET_CPSR_V(cs) GET_BITS(cs.CPSR, 28, 1)

#define SET_CPSR_N(cs, flag) SET_BITS(cs.CPSR, 31, 1, flag)
#define SET_CPSR_Z(cs, flag) SET_BITS(cs.CPSR, 30, 1, flag)
#define SET_CPSR_C(cs, flag) SET_BITS(cs.CPSR, 29, 1, flag)
#define SET_CPSR_V(cs, flag) SET_BITS(cs.CPSR, 28, 1, flag)
/*
#define SET_CPSR_N(cs, flag) \
{   \
    cs.CSPR = ((  (flag) & cs.CSPR) | (flag << 31)); \
}

#define SET_CPSR_Z(cs, flag) \
{   \
    cs.CSPR = (((((WORD_TYPE) ~0) >> 2) & cs.CSPR) | (flag << 30)); \
}
#define SET_CPSR_C(cs, flag) \
{   \
    cs.CSPR = (((((WORD_TYPE) ~0) >> 2) & cs.CSPR) | (flag << 30)); \
}

#define GET_CONDITION_FIELD(instr) GET_4(instr, 28)
*/


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
    if (GET_BITS(instr, 26, 1) == 0 && GET_BITS(instr, 27, 1) == 0 && (GET_BITS(instr, 4, 1) == 0 || GET_BITS(instr, 7, 1)))
    {
        return DATA_PROCESSING;
    }
}


void execute_data_processing(INSTR_TYPE instr)
{
    #define ISSA (GET_BITS(dp_instr_p->operand2, 4, 1) == 0x0) // Instruction Specified Shift Amount
    #define RSSA (!ISSA) // Register Specified Shift Amount
    #define ROTATE_RIGHT(word, amount) ((word << (WORD_WIDTH - amount)) | (word >> amount))


    struct data_processing_instr_t *dp_instr_p = (struct data_processing_instr_t *) &instr;
    // get the second operand
    WORD_TYPE operand1_val, operand2_val, result;
    BYTE_TYPE carry_out_bit = GET_CPSR_C(CURRENT_STATE);   // C flag in CPSR
    
    if (dp_instr_p->i == 0x0)
    {
        // the second operand is in register, but a shift/rotate operation is needed.
        uint8_t rm = GET_BITS(dp_instr_p->operand2, 0, 4);
        uint8_t shift_type = GET_BITS(dp_instr_p->operand2, 5, 2);
        uint8_t shift_amount;

        
        if (ISSA)
        {
            // shift amount is an immediate value
            shift_amount = GET_BITS(dp_instr_p->operand2, 7, 5);
        }
        else    
        {
            // shift amount is the bottom byte of a register.
            WORD_TYPE rs = CURRENT_STATE.REGS[GET_BITS(dp_instr_p->operand2, 8, 4)];
            shift_amount = GET_BITS(rs, 0, 8);
        }

        
        
        switch (shift_type)
        {
            case (0x0): // logical left
            {
                
                if (shift_amount != 0x0)    // if shift amount is zero, don't touch the flags
                {
                    /* carry out bit is set to: 
                     * 1) the least significant bit discarded during the shift, if ISSA or RSAA is less than 32
                     * 2) bit #0, if RSSA is 32
                     * 3) 0, if RSSA is more than 32
                     */
                    carry_out_bit = ((CURRENT_STATE.REGS[rm] << (shift_amount - 1)) >> 31) & 0x1;
                }
                // do the left shift
                operand2_val = (CURRENT_STATE.REGS[rm] << shift_amount);
                break;
            }
            case (0x1): // logical right
            {
                
                if (!(RSSA && (shift_amount == 0x0)))   // RSSA of 0 means don't touch the flags.
                {
                    if (ISSA && (shift_amount == 0x0))    // LSR ISSA of 0 is redundant with LSL $0, so that form is used to represent LSR $32
                    {
                        shift_amount = 32;
                    }
                    carry_out_bit = (CURRENT_STATE.REGS[rm] >> (shift_amount - 1)) & 0x1;
                }
                operand2_val = (CURRENT_STATE.REGS[rm] >> shift_amount);
                break;
            }
            case (0x2): // arithmetic right
            {
                if (!(RSSA && (shift_amount == 0x0)))   // RSSA of 0 means don't touch the flags.
                {
                    if (ISSA && (shift_amount == 0x0))    // ASR ISSA of 0 is redundant with LSL $0, so that form is used to represent ASR $32
                    {
                        shift_amount = 32;
                    }
                    carry_out_bit = (((int32_t) CURRENT_STATE.REGS[rm]) >> (shift_amount - 1)) & 0x1;
                }
                operand2_val = (((int32_t) CURRENT_STATE.REGS[rm]) >> shift_amount);
                break;
            }
            case (0x3): // rotate right
            {
                if (!(RSSA && (shift_amount == 0x0)))   // RSSA of 0 means don't touch the flags.
                {
                    if (ISSA && (shift_amount == 0x0))  // still ISSA of 0 has a specific behaviour.
                    {
                        // rotate right extended, use C flag to fill it up.
                        WORD_TYPE temp = carry_out_bit << 31;
                        operand2_val = CURRENT_STATE.REGS[rm];
                        carry_out_bit = operand2_val & 0x1;
                        operand2_val = (operand2_val >> 1) | temp;
                        
                    }
                    else
                    {
                        // perform normal rotate right
                        if (shift_amount > 32)
                        {
                            shift_amount %= 32;
                        }
                        
                        operand2_val = CURRENT_STATE.REGS[rm];
                        carry_out_bit = (operand2_val >> (shift_amount - 1)) & 0x1;
                        
                        //operand2_val = (operand2_val << (WORD_WIDTH - shift_amount)) | (operand2_val >> shift_amount);
                        operand2_val = ROTATE_RIGHT(operand2_val, shift_amount);
                    }
                }
                break;
            }
        }
    }
    else if (dp_instr_p->i == 0x1)
    {
        // immediate value
        WORD_TYPE immediate_val = GET_BITS(dp_instr_p->operand2, 0, 8);
        WORD_TYPE rotate_amount = GET_BITS(dp_instr_p->operand2, 8, 4);
        operand2_val = ROTATE_RIGHT(immediate_val, (rotate_amount << 1));
        // QUESTION: should I set the carry out flag here?
    }


    // get the first operand
    if (dp_instr_p->rn == 15)   // r15(pc) as a operand
    {// QUESTION: can r15 be rm?
        if (ISSA)
        {
            operand1_val = CURRENT_STATE.REGS[15] + 8;
        }
        else    // RSSA
        {
            operand1_val = CURRENT_STATE.REGS[15] + 12;
        }
    }
    else
    {
        operand1_val = CURRENT_STATE.REGS[dp_instr_p->rn];
    }

    // if S bit is set, C flag will be set to what barrier shifter gave.
    // QUESTION: condition that r15 is rd should not be concerned here?
    carry_out_bit = (dp_instr_p->s == 0x0) ? GET_CPSR_C(CURRENT_STATE) : carry_out_bit;

    // operate
    uint8_t not_written = 0;
    switch (dp_instr_p->opcode)
    {
        case (0x0): // and
            result = operand1_val & operand2_val;
            break;
        case (0x1): // eor
            result = operand1_val ^ operand2_val;
            break;
        case (0x2): // sub
            result = operand1_val - operand2_val;
            break;
        case (0x3): // rsb
            result = operand2_val - operand1_val;
            break;
        case (0x4): // add
            result = operand1_val + operand2_val;
            break;
        case (0x5): // adc
            result = operand1_val + operand2_val + carry_out_bit;
            break;
        case (0x6): // sbc
            result = operand1_val - operand2_val + carry_out_bit - 1;
            break;
        case (0x7): // rsc
            result = operand2_val - operand1_val + carry_out_bit - 1;
            break;
        case (0x8): // tst
            result = operand1_val & operand2_val;
            not_written = 1;
            break;
        case (0x9): // teq
            result = operand1_val ^ operand2_val;
            not_written = 1;
            break;
        case (0xa): // cmp
            result = operand1_val - operand2_val;
            not_written = 1;
            break;
        case (0xb): // cmn
            result = operand1_val + operand2_val;
            not_written = 1;
            break;
        case (0xc): // orr
            result = operand1_val | operand2_val;
            break;
        case (0xd): // mov
            result = operand2_val;
            break;
        case (0xe): // bic QUESTION and not == ~(a|b) ???
            result = ~(operand1_val | operand2_val);
            break;
        case (0xf): // mvn
            result = ~operand2_val;
            break;
        default:
            // shouldn't reach here

    }

    // set flags
    if (dp_instr_p->s == 0x1)
    {
        SET_CPSR_C(NEXT_STATE, carry_out_bit);
        SET_CPSR_Z(NEXT_STATE, (result == 0));
        SET_CPSR_N(NEXT_STATE, GET_BITS(result, 31, 1));
    }
    
    // write result
    if (!not_written)
    {
        NEXT_STATE.REGS[dp_instr_p->rd] = result;
    }

    #undef ISSA
    #undef RSSA
    #undef ROTATE_RIGHT
}


void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    NEXT_STATE = CURRENT_STATE;


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
