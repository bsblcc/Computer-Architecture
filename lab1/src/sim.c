#include <stdio.h>
#include "shell.h"
#include "sim.h"



/*  instr to do:
 *  adc-----------------
 *  add-----------------
 *  and-----------------
 *  b-------------------
 *  bic-----------------
 *  bl------------------
 *  cmn-----------------
 *  cmp-----------------
 *  eor-----------------
 *  ldr-----------------
 * ldrb-----------------
 *  mla-----------------
 *  mov-----------------
 *  mul-----------------
 *  mvn-----------------
 *  orr-----------------
 *  rsb-----------------
 *  rsc-----------------
 *  sbc-----------------
 *  str-----------------
 * strb-----------------
 *  sub-----------------
 *  teq-----------------
 *  tst-----------------
 *  swi-----------------
 */







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
    // should be a DFA
    if (GET_BITS(instr, 26, 2) == 0 && ((GET_BITS(instr, 4, 1) == 0) || (GET_BITS(instr, 7, 1) == 0) || (GET_BITS(instr, 25, 1) == 1)))
    {
        return DATA_PROCESSING;
    }
    else if (GET_BITS(instr, 25, 3) == 0x5)
    {
        return BRANCH_WITH_LINK;
    }
    else if (GET_BITS(instr, 24, 4) == 0xf)
    {
        return SOFTWARE_INTERRUPT;
    }
    else if (GET_BITS(instr, 4, 4) == 0x9 && GET_BITS(instr, 22, 6) == 0x0)
    {
        return MULTIPLY;
    }
    else if (GET_BITS(instr, 26, 2) == 0x1)
    {
        return DATA_TRANSFER;
    }
    return INVALID_GROUP;
}

WORD_TYPE shift_barrel(WORD_TYPE shift_field, BYTE_TYPE s)
{
    #define ISSA (GET_BITS(shift_field, 4, 1) == 0x0) // Instruction Specified Shift Amount
    #define RSSA (!ISSA) // Register Specified Shift Amount
   
    uint8_t rm = GET_BITS(shift_field, 0, 4);
    uint8_t shift_type = GET_BITS(shift_field, 5, 2);
    uint8_t shift_amount;
    BYTE_TYPE carry_out_bit = GET_CPSR_C(CURRENT_STATE);   // C flag in CPSR
    
    WORD_TYPE result;

    if (ISSA)
    {
        // shift amount is an immediate value
        shift_amount = GET_BITS(shift_field, 7, 5);
    }
    else    
    {
        // shift amount is the bottom byte of a register.
        WORD_TYPE rs = CURRENT_STATE.REGS[GET_BITS(shift_field, 8, 4)];
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
            result = (CURRENT_STATE.REGS[rm] << shift_amount);
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
            result = (CURRENT_STATE.REGS[rm] >> shift_amount);
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
            result = (((int32_t) CURRENT_STATE.REGS[rm]) >> shift_amount);
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
                    result = CURRENT_STATE.REGS[rm];
                    carry_out_bit = result & 0x1;
                    result = (result >> 1) | temp;
                    
                }
                else
                {
                    // perform normal rotate right
                    if (shift_amount > 32)
                    {
                        shift_amount %= 32;
                    }
                    
                    result = CURRENT_STATE.REGS[rm];
                    carry_out_bit = (result >> (shift_amount - 1)) & 0x1;
                    
                    //operand2_val = (operand2_val << (WORD_WIDTH - shift_amount)) | (operand2_val >> shift_amount);
                    result = ROTATE_RIGHT(result, shift_amount);
                }
            }
            break;
        }
    }

    // if S bit is set, C flag will be set to what barrier shifter gave.
    // QUESTION: condition that r15 is rd should not be concerned here?
    if (s == 1)
    {
        SET_CPSR_C(NEXT_STATE, carry_out_bit);
    }

    return result;

    #undef ISSA
    #undef RSSA
    
}


void execute_data_processing(INSTR_TYPE instr)
{
    
    struct data_processing_instr_t *dp_instr_p = (struct data_processing_instr_t *) &instr;
    // get the second operand
    WORD_TYPE operand1_val, operand2_val, result;
    BYTE_TYPE carry_out_bit = GET_CPSR_C(CURRENT_STATE);   // C flag in CPSR
    



    if (dp_instr_p->i == 0x0)
    {
        // the second operand is in register, but a shift/rotate operation is needed.
        operand2_val = shift_barrel(dp_instr_p->operand2, dp_instr_p->s);
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
        if ((GET_BITS(dp_instr_p->operand2, 4, 1) == 0x0))
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
            dp_instr_p->s = 0x1;
            not_written = 1;
            break;
        case (0x9): // teq
            result = operand1_val ^ operand2_val;
            dp_instr_p->s = 0x1;
            not_written = 1;
            break;
        case (0xa): // cmp
            result = operand1_val - operand2_val;
            dp_instr_p->s = 0x1;        // S bit in comparison instructions is set by default?
            not_written = 1;


            break;
        case (0xb): // cmn
            result = operand1_val + operand2_val;
            dp_instr_p->s = 0x1;
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
            break;
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
}

void execute_branch_with_link(INSTR_TYPE instr)
{
   
    struct branch_with_link_instr_t *bl_instr_p = (struct branch_with_link_instr_t *) &instr;

    // offset field will be left-shifted 2 bits
    int32_t offset = ((int32_t) bl_instr_p->offset) << 2;
    
    // PC = PC(old PC + 0x4) + 8 +offset, because of the pi peline, but how comes that?
    // why isn't that
    NEXT_STATE.PC = CURRENT_STATE.PC + 0x8 + offset;
    
    if (bl_instr_p->l == 0x1)   // branch with link
    {
        NEXT_STATE.REGS[14] = CURRENT_STATE.PC;     // store the PC of the current instr
    }
}

void execute_software_interrupt(INSTR_TYPE instr)
{
    // hack for this lab: if the bottom byte is 0x0a, halt
    if (GET_BITS(instr, 0, BYTE_WIDTH) == 0xa)
    {
        RUN_BIT = 0;
    }
}

void execute_multiply(INSTR_TYPE instr)
{
    #define RM_VAL CURRENT_STATE.REGS[mul_instr_p->rm]
    #define RS_VAL CURRENT_STATE.REGS[mul_instr_p->rs]
    #define RN_VAL CURRENT_STATE.REGS[mul_instr_p->rn]
    
    struct multiply_instr_t *mul_instr_p = (struct multiply_instr_t *) &instr;


    // r15 cannot be used, rd cannot be the same as rm
    if ((mul_instr_p->rd == mul_instr_p->rm)
        || (mul_instr_p->rd == 15 && 
            mul_instr_p->rm == 15 &&
            mul_instr_p->rs == 15 &&
            mul_instr_p->rn == 15))
    {
        // invalid instruction!
        fprintf(stderr, "invalid multiply instruction\n");
    }
    
    WORD_TYPE result = (RM_VAL * RS_VAL);
    //fprintf(stderr, "%x * %x = %x\n", RM_VAL, RS_VAL, result);
    result += (mul_instr_p->a == 0) ? (0) : (RN_VAL);

    if (mul_instr_p->s == 1)
    {
        // set CPSR flags
        SET_CPSR_N(NEXT_STATE, GET_BITS(result, 31, 1));
        SET_CPSR_Z(NEXT_STATE, (result == 0));

        // what is "the C flag is set to a meaningless value" ???
    }
    NEXT_STATE.REGS[mul_instr_p->rd] = result;
    #undef RM_VAL
    #undef RS_VAL
    #undef RN_VAL
}

void execute_data_transfer(INSTR_TYPE instr)
{


    struct data_transfer_instr_t *dt_instr_p = (struct data_transfer_instr_t *) &instr;

    int32_t offset;
    WORD_TYPE base = CURRENT_STATE.REGS[dt_instr_p->rn];
    fprintf(stderr, "base: %x\n", base);
    if (dt_instr_p->i == 0) // immediate offset
    {
        offset = (int32_t) dt_instr_p->offset;
    }
    else            // shifted 
    {   
        // QUESTION: do shift barrel here changes CPSR?
        offset = (int32_t) shift_barrel(dt_instr_p->offset, 1);
    }

    if (dt_instr_p->u == 0) // subtract the offset
    {
        offset = -offset;
    }

    fprintf(stderr, " offset: %x\n", offset);
    if (dt_instr_p->p == 1) // pre, offset modification before transfer
    {
        base += offset;
    }

    fprintf(stderr, "base: %x\n", base);
    // little-endian by default
    if (dt_instr_p->b == 1) // transfer by byte
    {
        if (dt_instr_p->l == 0) // store
        {
            // this is my "memory system" haha
            WORD_TYPE temp = mem_read_32(base);
            // only modify the lowest byte
            temp = ((temp & (~0xff)) | GET_BITS(CURRENT_STATE.REGS[dt_instr_p->rd], 0, 8));
            mem_write_32(base, temp);
        }
        else    // load
        {
            NEXT_STATE.REGS[dt_instr_p->rd] = GET_BITS(mem_read_32(base), 0, 8);
        }
    }
    else        // transfer by word
    {
        if (dt_instr_p->l == 1)         // load
        {
            uint8_t mod = base & 0x3;
            // not word-aligned, a rotatation is needed.
            WORD_TYPE aligned_base = base & (~0x3);
            WORD_TYPE temp = mem_read_32(aligned_base);
            
            temp = ROTATE_RIGHT(temp, mod * BYTE_WIDTH);
            NEXT_STATE.REGS[dt_instr_p->rd] = temp;
        }
        else        // store
        {
            // nothing to worry
            
            mem_write_32(base, CURRENT_STATE.REGS[dt_instr_p->rd]);
        }
    }




    if (dt_instr_p->p == 0) // post, offset modification after transfer 
    {
        base += offset;
        // if it's post, the modified base must be written back to rn
        // otherwise we modified that base, and never use it?
        NEXT_STATE.REGS[dt_instr_p->rn] = base;
    }
    // ignore the user mode
    if (dt_instr_p->w == 1)
    {
        NEXT_STATE.REGS[dt_instr_p->rn] = base;
    }
    
}


void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    NEXT_STATE = CURRENT_STATE;

    NEXT_STATE.PC = CURRENT_STATE.PC + sizeof(INSTR_TYPE);

    // fetch
    INSTR_TYPE instr = fetch_next_instruciton();
    fprintf(stderr, "pc: %x\tinstr: %x\n",CURRENT_STATE.PC, instr);
    // execute
    if (check_condition(GET_CONDITION_FIELD(instr)))
    {
        // execute
        uint32_t group = get_instruction_group(instr);
        fprintf(stderr, "group: %ud\n", group);
        switch (group)
        {
            case (DATA_PROCESSING):
            {
                execute_data_processing(instr);
                break;
            }
            case (BRANCH_WITH_LINK):
            {
                execute_branch_with_link(instr);
                break;
            }
            case (SOFTWARE_INTERRUPT):
            {
                execute_software_interrupt(instr);
                break;
            }
            case (MULTIPLY):
            {
                execute_multiply(instr);
                break;
            }
            case (DATA_TRANSFER):
            {
                execute_data_transfer(instr);
                break;
            }
            default:
                // not yet implemented
                printf("ERROR: instruction not implemented\n");
                //exit(0);
                break;
        }
    }
    else
    {
        // ignore, pass.
    }

    
}