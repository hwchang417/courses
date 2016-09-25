
`timescale 1ns/10ps
module PC(clk, rst, address, enable_fetch, enable_mem, ir, Rt, Ra, isBranch);

parameter memSize = 10;
parameter men_size=1024;

input clk;
input rst;
input enable_fetch;
input enable_mem;
input[31:0] ir;
input[31:0] Rt, Ra;
output reg [memSize-1:0] address;
output isBranch;

wire [5:0] opcode = ir[30:25];
wire sub_opcode_branch = ir[14];
wire sub_opcode_jmp = ir[24];
wire [31:0] SE_imm_branch = ((ir[13]) ? {17'h1ffff, ({1'b1, ir[13:0]} << 1)} : {17'h00000, ({1'b0, ir[13:0]} << 1)});
wire [31:0] SE_imm_jmp = ((ir[23]) ? {7'h7f, ({1'b1, ir[23:0]} << 1)} : {7'h00, ({1'b0, ir[23:0]} << 1)});


reg [1:0] count2times;
wire isBranch = (opcode == 6'b100110) && (count2times >= 2'b1);
wire isBranch_onlyop = (opcode == 6'b100110);
//PC	  
always@(negedge clk)
begin
  if(rst)
  begin
	address = 10'h3ff; //delay one clock
	count2times = 2'd0;
  end
  else
  begin
	
	if(enable_mem && enable_fetch && address != men_size - 2) 
	begin
		if(isBranch_onlyop && count2times == 2'd2)
		begin
			count2times = 2'd0;
			if(((sub_opcode_branch == 1'b0 && Rt == Ra) || (sub_opcode_branch == 1'b1 && Rt != Ra)))
				address = address + SE_imm_branch;	
			else address = address + 1'd1;
		end
		//else if(isBranch_onlyop && count2times == 2'd0) begin count2times = count2times + 1'd1; address = address - 1; end
		else if(isBranch_onlyop) begin count2times = count2times + 1'd1;end
		else
		begin
			if(opcode == 6'b100100 && sub_opcode_jmp == 1'b0) address = address + SE_imm_jmp;	
			else	address = address + 1'd1;
		end
	end
  end
end
	  

endmodule
