/*
 * Redistributions of any form whatsoever must retain and/or include the
 * following acknowledgment, notices and disclaimer:
 *
 * This product includes software developed by Carnegie Mellon University. 
 *
 * Copyright (c) 2004 by Babak Falsafi and James Hoe,
 * Computer Architecture Lab at Carnegie Mellon (CALCM), 
 * Carnegie Mellon University.
 *
 * This source file was modified by Xiao Bo Zhao for the class 18-447 in
 * order to meet the requirements of an ARM processor.
 *
 * You may not use the name "Carnegie Mellon University" or derivations 
 * thereof to endorse or promote products derived from this software.
 *
 * If you modify the software you must place a notice on or within any 
 * modified version provided or made available to any third party stating 
 * that you have modified the software.  The notice shall include at least 
 * your name, address, phone number, email address and the date and purpose 
 * of the modification.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER 
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO ANYWARRANTY 
 * THAT THE SOFTWARE WILL CONFORM TO SPECIFICATIONS OR BE ERROR-FREE AND ANY 
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
 * TITLE, OR NON-INFRINGEMENT.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY 
 * BE LIABLE FOR ANY DAMAGES, INCLUDING BUT NOT LIMITED TO DIRECT, INDIRECT, 
 * SPECIAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN 
 * ANY WAY CONNECTED WITH THIS SOFTWARE (WHETHER OR NOT BASED UPON WARRANTY, 
 * CONTRACT, TORT OR OTHERWISE).
 *
 */

//////
////// ARM Defines: Numerical parameters of the ARM processor
//////

////
//// USEFUL CONSTANTS
////

// Mask to force word-alignment of addresses
`define ADDR_ALIGN_MASK 32'hfffffffc

// Mask particular bytes
`define BYTE_0_MASK     32'h000000ff
`define BYTE_1_MASK     32'h0000ff00
`define BYTE_2_MASK     32'h00ff0000
`define BYTE_3_MASK     32'hff000000
`define BYTE_0_1_MASK   32'h0000ffff
`define BYTE_0_2_MASK   32'h00ff00ff
`define BYTE_0_3_MASK   32'hff0000ff
`define BYTE_1_2_MASK   32'h00ffff00
`define BYTE_1_3_MASK   32'hff00ff00
`define BYTE_2_3_MASK   32'hffff0000
`define BYTE_0_1_2_MASK 32'h00ffffff
`define BYTE_0_1_3_MASK 32'hff00ffff
`define BYTE_0_2_3_MASK 32'hffff00ff
`define BYTE_1_2_3_MASK 32'hffffff00


////
//// OPCODES
////

// Data processing opcodes

`define OPD_AND     4'h0
`define OPD_EOR     4'h1
`define OPD_SUB     4'h2
`define OPD_RSB     4'h3
`define OPD_ADD     4'h4
`define OPD_ADC     4'h5
`define OPD_SBC     4'h6
`define OPD_RSC     4'h7
`define OPD_TST     4'h8
`define OPD_TEQ     4'h9
`define OPD_CMP     4'ha
`define OPD_CMN     4'hb
`define OPD_ORR     4'hc
`define OPD_MOV     4'hd
`define OPD_BIC     4'he
`define OPD_MVN     4'hf

// Shift opcodes

`define OPS_SLL     2'h0
`define OPS_SLR     2'h1
`define OPS_SAR     2'h2
`define OPS_ROR     2'h3

// Condition opcodes

`define COND_EQ     4'h0
`define COND_NE     4'h1
`define COND_CS     4'h2
`define COND_CC     4'h3
`define COND_MI     4'h4
`define COND_PL     4'h5
`define COND_VS     4'h6
`define COND_VC     4'h7
`define COND_HI     4'h8
`define COND_LS     4'h9
`define COND_GE     4'ha
`define COND_LT     4'hb
`define COND_GT     4'hc
`define COND_LE     4'hd
`define COND_AL     4'he
`define COND_UNDEF  4'hf

////
//// Register Mnemonics
////

`define R_A1        4'h0
`define R_A2        4'h1
`define R_A3        4'h2
`define R_A4        4'h3
`define R_V1        4'h4
`define R_V2        4'h5
`define R_V3        4'h6
`define R_V4        4'h7
`define R_V5        4'h8
`define R_V6        4'h9
`define R_V7        4'ha
`define R_V8        4'hb
`define R_SB        4'h9
`define R_SL        4'ha
`define R_FP        4'hb
`define R_IP        4'hc
`define R_SP        4'hd
`define R_LR        4'he
`define R_PC        4'hf

////
//// ARM447 constants
////

// System calls
`define SWI_EXIT         24'ha
