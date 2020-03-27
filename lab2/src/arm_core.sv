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

//////
////// ARM 447: A single-cycle ARM ISA simulator
//////

// Include the ARM constants
`include "arm_defines.vh"
`include "internal_defines.vh"

////
//// The ARM standalone processor module
////
////   clk          (input)  - The clock
////   inst_addr    (output) - Address of instruction to load
////   inst         (input)  - Instruction from memory
////   mem_addr     (output) - Address of data to load
////   mem_data_in  (output) - Data for memory store
////   mem_data_out (input)  - Data from memory load
////   mem_write_en (output) - Memory write mask
////   halted       (output) - Processor halted
////   rst_b        (input)  - Reset the processor
////

module arm_core (
  // Outputs
  inst_addr, mem_addr, mem_data_in, mem_write_en, halted,
  // Inputs
  clk, rst_b, inst, mem_data_out
);

  // Core Interface
  input         clk, rst_b;
  output [29:0] inst_addr;
  output [29:0] mem_addr;
  input  [31:0] inst, mem_data_out;
  output [31:0] mem_data_in;
  output [3:0]  mem_write_en;
  output        halted;

  // Forced interface signals -- required for synthesis to work OK.
  // This is probably not what you want!
  assign        mem_addr = 0;
  assign        mem_data_in = mem_data_out;
  assign        mem_write_en = 4'b0;

  wire [3:0]    dcd_rn, dcd_rd, dcd_rm;
  wire [3:0]    dcd_mul_rn, dcd_mul_rd, dcd_mul_rs;

  wire [7:0]    dcd_dp_imm;
  wire [3:0]    dcd_dp_rotate;

  wire          dcd_shift_fmt;
  wire [1:0]    dcd_shift_type;
  wire [4:0]    dcd_shift_amt;
  wire [3:0]    dcd_shift_rs;
  wire [11:0]   dcd_sdt_offset;
  wire [23:0]   dcd_br_offset;
  wire [23:0]   dcd_swi_offset;

  assign        dcd_rn = inst[19:16];
  assign        dcd_rd = inst[15:12];
  assign        dcd_rm = inst[3:0];

  // Multiply reverses rd/rn
  assign        dcd_mul_rd = inst[19:16];
  assign        dcd_mul_rn = inst[15:12];
  assign        dcd_mul_rs = inst[11:8];

  assign        dcd_dp_imm = inst[7:0];
  assign        dcd_dp_rotate = inst[11:8];

  assign        dcd_shift_fmt = inst[4];
  assign        dcd_shift_type = inst[6:5];
  assign        dcd_shift_amt = inst[11:7];
  assign        dcd_shift_rs = inst[11:8];
  assign        dcd_sdt_offset = inst[11:0];
  assign        dcd_br_offset = inst[23:0];
  assign        dcd_swi_offset = inst[23:0];

  wire [31:0] pc;

  // You probably want to connect this to the register file
  assign pc = 32'h00400000;
  assign inst_addr = pc;

  // synthesis translate_off
  //synopsys translate_off
  always_ff @(posedge clk) begin
    // useful for debugging, you will want to comment this out for long programs
    if (rst_b) begin
      $display ( "=== Simulation Cycle %d ===", $time );
      $display ( "[pc=%x, inst=%x] [reset=%d, halted=%d]",
                   pc, inst, ~rst_b, halted);
    end
  end
  //synopsys translate_on
  // synthesis translate_on

  wire [3:0]		alu_sel;		// From Decoder of arm_decode.v
  wire          reg_we;
  wire [3:0]    cpsr_mask;
  wire          dcd_swi;


  // Generate control signals
  arm_decode Decoder
  (
	  // Outputs
    .reg_we(reg_we),
    .cpsr_mask(cpsr_mask),
		.alu_sel(alu_sel),
    .swi(dcd_swi),
		// Inputs
    .inst(inst)
  );

  // Register File
  // Instantiate the register file from reg_file.v here.
  // Don't forget to hookup the "halted" signal to trigger the register dump

  // synthesis translate_off
  //synopsys translate_off
  initial begin
    // Delete this block when you are ready to try for real
    $display("");
    $display("");
    $display("");
    $display("");
    $display(">>>>> This works much better after you have hooked up the reg file. <<<<<");
    $display("");
    $display("");
    $display("");
    $display("");
    $finish;
  end
  //synopsys translate_on
  // synthesis translate_on

  wire [31:0] alu_out;
  wire        alu_cout;

  // Execute
  arm_alu alu
  (
    .alu_out(alu_out),
    .alu_cout(alu_cout),
    .alu_op1(32'h1),
    .alu_op2(32'h0),
    .alu_cin(1'h0),
    .alu_sel(alu_sel)
  );

  wire [31:0] mac_out;

  // Multiply Accumulator
  arm_mac mac
  (
    .mac_out(mac_out),
    .mac_op1(32'hcafe),
    .mac_op2(32'h1ace),
    .mac_acc(32'h1acc),
    .mac_sel(1'h0)
  );


  wire          syscall_halt, internal_halt;

  assign        syscall_halt = dcd_swi && (dcd_swi_offset == `SWI_EXIT);

  assign        internal_halt = syscall_halt;

  register #(1, 0) Halt(halted, internal_halt, clk, 1'b1, rst_b);

endmodule // arm_core


////
//// arm_alu: Performs all arithmetic and logical operations
////
//// alu_out  (output) - Final result
//// alu_cout (output) - Carry out
//// alu_op1  (input)  - Operand modified by the operation
//// alu_op2  (input)  - Operand used (in arithmetic ops) to modify op1
//// alu_sel  (input)  - Selects which operation is to be performed
//// alu_cin  (input)  - Carry in
////
module arm_alu(alu_out, alu_cout, alu_op1, alu_op2, alu_sel, alu_cin);

  output reg  [31:0]  alu_out;
  output reg          alu_cout;
  input       [31:0]  alu_op1, alu_op2;
  input       [3:0]   alu_sel;
  input               alu_cin;
  

  reg [31:0] adder_a , adder_b, out;
  always_comb begin
    //alu_out = alu_op1 + alu_op2;
    case (alu_sel) 
      4'd0: begin
        clu_out = alu_op1 + alu_op2;
      end
      4'd1: begin
        alu_out = alu_op1 ^ alu_op2;
      end
      4'd2: begin
        alu_out = alu_op1 - alu_op2;
      end
      4'd3: begin
        alu_out = alu_op2 - alu_op1;
      end
      4'd4: begin
        alu_out = alu_op1 + alu_op2;
      end
      4'd5: begin
        alu_out = alu_op1 + alu_op2 + alu
    endcase
  end

  arm_alu_adder (.operand_a(adder_a), .operand_b(adder_b),
   .carry_in(alu_cin), .out(adder_out));

endmodule

////
//// arm_mac: Performs multiply accumulate operations
////
//// mac_out  (output) - Final result
//// mac_op1  (input)  - Operand modified by the operation
//// mac_op2  (input)  - Operand used to multiply op1
//// mac_acc  (input)  - Accumulator
//// mac_sel  (input)  - Selects whether to accumulate
////
module arm_mac(mac_out, mac_op1, mac_op2, mac_acc, mac_sel);

  output reg  [31:0]  mac_out;
  input       [31:0]  mac_op1, mac_op2, mac_acc;
  input               mac_sel;

  always_comb begin
    // I suspect there is something missing here
    mac_out = mac_op1;
  end

endmodule


//// register: A register which may be reset to an arbirary value
////
//// q      (output) - Current value of register
//// d      (input)  - Next value of register
//// clk    (input)  - Clock (positive edge-sensitive)
//// enable (input)  - Load new value?
//// reset  (input)  - System reset
////
module register(q, d, clk, enable, rst_b);

   parameter
            width = 32,
            reset_value = 0;

   output [(width-1):0] q;
   reg [(width-1):0]    q;
   input [(width-1):0]  d;
   input                 clk, enable, rst_b;

   always_ff @(posedge clk or negedge rst_b)
     if (~rst_b)
       q <= reset_value;
     else if (enable)
       q <= d;

endmodule // register
