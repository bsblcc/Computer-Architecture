/*
 * Redistributions of any form whatsoever must retain and/or include the
 * following acknowledgment, notices and disclaimer:
 *
 * This product includes software developed by Carnegie Mellon University.
 *
 * Copyright (c) 2008 James C. Hoe,
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
 */

module regfile
(
  // Outputs
  rn_data, rm_data, rs_data, pc_out, cpsr_out,
  // Inputs
  rn_num, rm_num, rs_num, rd_num, rd_data, rd_we,
  pc_in, pc_we, cpsr_in, cpsr_we,
  clk, rst_b, halted
);

  parameter text_start  = 32'h00400000; /* Initial value of PC */

  input       [3:0]  rn_num, rm_num, rs_num, rd_num;
  input       [31:0] rd_data, pc_in, cpsr_in;
  input              rd_we, pc_we, cpsr_we, clk, rst_b, halted;
  output wire [31:0] rn_data, rm_data, rs_data, pc_out, cpsr_out;

  reg         [31:0] mem[0:15];
  reg         [31:0] cpsr;
  integer            i;

  always @(posedge clk or negedge rst_b) begin
    if (!rst_b) begin
      for (i = 0; i < 15; i = i+1)
        mem[i] <= 32'b0;
      mem[15] <= text_start;
      cpsr <= 32'b0;
    end else begin
      if (rd_we && (rd_num != 4'd15))
        mem[rd_num] <= rd_data;
      if (pc_we)
        mem[4'd15] <= pc_in;
      if (cpsr_we)
        cpsr <= cpsr_in;
    end
  end

  assign rn_data = mem[rn_num];
  assign rm_data = mem[rm_num];
  assign rs_data = mem[rs_num];
  assign pc_out = mem[4'd15];
  assign cpsr_out = cpsr;

  // synthesis translate_off
  //synopsys translate_off
  integer fd;
  always @(halted) begin
    if (rst_b && halted) begin
      fd = $fopen("regdump.txt");

      $display("--- 18-447 Register file dump ---");
      $display("=== Simulation Cycle %d ===", $time);

      $fdisplay(fd, "--- 18-447 Register file dump ---");
      $fdisplay(fd, "=== Simulation Cycle %d ===", $time);

      for(i = 0; i < 16; i = i+1) begin
        $display("R%d\t= 0x%8x\t( %0d )", i, mem[i], mem[i]);
        $fdisplay(fd, "R%d\t= 0x%8h\t( %0d )", i, mem[i], mem[i]);
      end
      $display("CPSR\t= 0x%8x\t( %0d )", cpsr, cpsr);
      $fdisplay(fd, "CPSR\t= 0x%8h\t( %0d )", cpsr, cpsr);

      $display("--- End register file dump ---");
      $fdisplay(fd, "--- End register file dump ---");

      $fclose(fd);
    end
  end
  //synopsys translate_on
  // synthesis translate_on

endmodule

