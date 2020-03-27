/*
 *
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

// Include the ARM constants
`include "arm_defines.vh"
`include "internal_defines.vh"

////
//// arm_decode: Decode ARM instructions
////
//// inst       (input)  - Instruction inst
//// alu_sel    (output) - Selects the ALU function
//// reg_we     (output) - Write to the register file
//// cpsr_mask  (output) - Write to which NZCV flag
//// swi        (output) - Indicates swi encountered
////
module arm_decode
(
  // Outputs
  reg_we, cpsr_mask, alu_sel, swi,
  // Inputs
  inst
);

  input       [31:0] inst;
  output reg  reg_we;
  output reg  [3:0] cpsr_mask;
  output reg  swi;
  output reg  [3:0] alu_sel;

  // A rather boring decoder right now
  always_comb begin
    alu_sel = 4'h0;
    swi = 0;
    reg_we = 0;
    cpsr_mask = 4'b0000;

    if (inst[27:24] == 4'b1111)
      swi = 1;
  end

endmodule
