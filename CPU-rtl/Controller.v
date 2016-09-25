`define SRLI 5'b01001
`define SLLI 5'b01000
`define ROTRI 5'b01011
`timescale 1ns/10ps
module Controller(enable_execute, enable_fetch, enable_writeback, 
opcode, sub_opcode, 
mux4to1_select, mux2to1_select, imm_reg_select, 
IM_enable_fetch, IM_enable_write, IM_enable_mem,
DM_enable_fetch, DM_enable_write, DM_enable_mem,
clock, reset, PC, flag_IF, flag_ID, flag_EX, flag_MEM, flag_WB,
ir);

input clock,reset;
input [9:0] PC;
input [31:0] ir;
output reg enable_execute,enable_fetch,enable_writeback;
output [5:0] opcode;
output [7:0] sub_opcode;
output reg [1:0] mux4to1_select;
output reg mux2to1_select,imm_reg_select;
output reg IM_enable_fetch,IM_enable_write,IM_enable_mem;
output reg DM_enable_fetch, DM_enable_write, DM_enable_mem;

//reg [31:0] present_instruction;
reg [31:0] IF_ir;
reg [5:0] IF_opcode;
reg [7:0] IF_sub_opcode; // condition ? ADD, AND... : LW, SW
reg [31:0] ID_ir;
reg [5:0] ID_opcode;
reg [7:0] ID_sub_opcode; // condition ? ADD, AND... : LW, SW
reg [31:0] EX_ir;
reg [5:0] EX_opcode;
reg [7:0] EX_sub_opcode; // condition ? ADD, AND... : LW, SW
reg [31:0] MEM_ir;
reg [5:0] MEM_opcode;
reg [7:0] MEM_sub_opcode; // condition ? ADD, AND... : LW, SW
reg [31:0] WB_ir;
reg [5:0] WB_opcode;
reg [7:0] WB_sub_opcode; // condition ? ADD, AND... : LW, SW

parameter S0 = 2'b00, S1 = 2'b01, S2 = 2'b10, S3 = 2'b11;


output reg flag_IF, flag_ID, flag_EX, flag_MEM, flag_WB;



always@(negedge clock)
begin
	if(reset)
	begin
		IF_ir <= 32'd0;
		IF_opcode <= 6'd0;
		IF_sub_opcode <= 8'd0;
		ID_ir <= 32'd0;
		ID_opcode <= 6'd0;
		ID_sub_opcode <= 8'd0; 
		EX_ir <= 32'd0;
		EX_opcode <= 6'd0;
		EX_sub_opcode <= 8'd0; 
		MEM_ir <= 32'd0;
		MEM_opcode <= 6'd0;
		MEM_sub_opcode <= 8'd0;
		WB_ir <= 32'd0;
		WB_opcode <= 6'd0;
		WB_sub_opcode <= 8'd0;
	end
	else
	begin
		if(flag_IF)
		begin
			IF_ir <= ir;
			IF_opcode <= ir[30:25];
			IF_sub_opcode <= (ir[30:25] == 6'b011100) ? ir[7:0] : {3'd0, ir[4:0]};
		end
		if(flag_ID)
		begin
			ID_ir <= IF_ir;
			ID_opcode <= IF_opcode;
			ID_sub_opcode <= IF_sub_opcode;
		end
		if(flag_EX)
		begin
			EX_ir <= ID_ir;
			EX_opcode <= ID_opcode;
			EX_sub_opcode <= ID_sub_opcode;
		end
		if(flag_MEM)
		begin
			MEM_ir <= EX_ir;
			MEM_opcode <= EX_opcode;
			MEM_sub_opcode <= EX_sub_opcode;
		end
		if(flag_WB)
		begin
			WB_ir <= MEM_ir;
			WB_opcode <= MEM_opcode;
			WB_sub_opcode <= MEM_sub_opcode;
		end
	end
end

always@(negedge clock)
begin
	if(reset)
	begin
		flag_IF = 1'b0;
		flag_ID = 1'b0;
		flag_EX = 1'b0;
		flag_MEM = 1'b0;
		flag_WB = 1'b0;
	end
	else
	begin
		if(flag_EX && !flag_WB) flag_WB = 1'b1;
		if(flag_EX && !flag_MEM) flag_MEM = 1'b1;
		if(flag_ID && !flag_EX) flag_EX = 1'b1;
		if(flag_IF && !flag_ID) flag_ID = 1'b1;
		if(!flag_IF) flag_IF = 1'b1;
	end
end
always@(*)
begin
	if(reset)
	begin
	
		enable_fetch = 1'b0;
		enable_execute = 1'b0;
		enable_writeback = 1'b0;
		mux4to1_select = 2'b00;
		mux2to1_select = 1'b0;
		imm_reg_select = 1'b0;
		
//IM control
		IM_enable_fetch = 1'b1;
		IM_enable_write = 1'b0;
		IM_enable_mem = 1'b1;
//DM control
		DM_enable_fetch = 1'b0;
		DM_enable_write = 1'b0;
		DM_enable_mem = 1'b1;
	end
	else
	begin
		if(flag_IF)
		begin
		
			//IM control
			IM_enable_fetch = 1'b1;
			IM_enable_write = 1'b0;
			IM_enable_mem = 1'b1;
		end
		if(flag_ID)
		begin
			
			enable_fetch = 1'b1;
			//00 for ZE(imm_5bit), 01 for SE(imm_15bit), 10 for ZE(imm_15bit), 11 for SE(imm_20bit)
			mux4to1_select = (IF_opcode == 6'b100000) ? 2'b00:
								  (IF_opcode == 6'b101000) ? 2'b01:
								  (IF_opcode == 6'b101100 || IF_opcode == 6'b101011 || 
								  //LWI, SWI
								  IF_opcode == 6'b000010 || IF_opcode == 6'b001010) ? 2'b10:
								  (IF_opcode == 6'b100010) ? 2'b11: 2'bzz;
			
			//write_back select, 1 for imm_data or 0 for alu_result
			mux2to1_select = (IF_opcode == 6'b100010) ? 1'b1 //MOVI
																: 1'b0; 
			
			//use imm for 1 or readdata2 for 0
			imm_reg_select = (IF_opcode == 6'b100000 && ( IF_sub_opcode == 8'b00000000 || 
																	IF_sub_opcode == 8'b00000001 || 
																	IF_sub_opcode == 8'b00000010 || 
																	IF_sub_opcode == 8'b00000100 ||
																	IF_sub_opcode == 8'b00000011)) || 
							//LW, SW
							 (IF_opcode == 6'b011100 && (IF_sub_opcode == 8'b00000010 || IF_sub_opcode == 8'b00001010)) ? 1'b0 : 1'b1;

		end
		if(flag_EX)
		begin
		
			enable_execute = 1'b1;
			
			//LWI, LW
			DM_enable_fetch = (ID_opcode == 6'b000010 || (ID_opcode == 6'b011100 && ID_sub_opcode == 8'b00000010));
		
		end
		if(flag_MEM)
		begin
			DM_enable_mem = 1'b1;
			DM_enable_write = (EX_opcode == 6'b001010 || (EX_opcode == 6'b011100 && EX_sub_opcode == 8'b00001010)) ? 1'b1 : 1'b0;

		
		end
		if(flag_WB)
		begin
		
			if((EX_opcode == 6'b100000 && EX_ir[24:5] == 20'd0 && EX_sub_opcode == 5'b01001) || 
			//SWI, SW
			   EX_opcode == 6'b001010 || (EX_opcode == 6'b011100 && EX_sub_opcode == 8'b00001010) ||
			   EX_opcode == 6'b100110 || EX_opcode == 6'b100100)	enable_writeback = 1'b0;
			else enable_writeback = 1'b1;
			
			//enable_fetch = ~enable_writeback;
			////////若有需要寫入register file, 則要延遲一個clock，讓reg_file不要讀取
		end
	end
end
/*
always@(posedge clock)
begin
	if(reset)
		cs <= S0;
	else
		cs <= ns;
end

always@(cs)
begin
	case(cs)
	S0 :
	begin  
		ns = S1;
		enable_fetch = 1'b0;
		enable_execute = 1'b0;
		enable_writeback = 1'b0;
		mux4to1_select = 2'b00;
		mux2to1_select = 1'b0;
		imm_reg_select = 1'b0;
		
//IM control
		IM_enable_fetch = 1'b1;
		IM_enable_write = 1'b0;
		IM_enable_mem = 1'b1;
//DM control
		DM_enable_fetch = 1'b0;
		DM_enable_write = 1'b0;
		DM_enable_mem = 1'b0;
	end
	S1 :
	begin  
		ns = S2;
		enable_fetch = 1'b1;
		enable_execute = 1'b0;
		enable_writeback = 1'b0;
		mux4to1_select = 2'b00;
		mux2to1_select = 1'b0;
		imm_reg_select = 1'b0;
//LWI, SWI, LW, SW
		DM_enable_mem = (opcode == 6'b000010 || (opcode == 6'b011100 && sub_opcode == 8'b00000010)) ? 1'b1 : 1'b0;
		DM_enable_fetch = (opcode == 6'b000010 || (opcode == 6'b011100 && sub_opcode == 8'b00000010)) ? 1'b1 : 1'b0;
		
		
	end
	S2 :
	begin  
		ns = S3;
		enable_fetch = 1'b0;
		enable_execute = 1'b1;
		enable_writeback = 1'b0;
		
		
		//00 for ZE(imm_5bit), 01 for SE(imm_15bit), 10 for ZE(imm_15bit), 11 for SE(imm_20bit)
		mux4to1_select = (opcode == 6'b100000) ? 2'b00:
							  (opcode == 6'b101000) ? 2'b01:
							  (opcode == 6'b101100 || opcode == 6'b101011 || 
							  //LWI, SWI
							  opcode == 6'b000010 || opcode == 6'b001010) ? 2'b10:
							  (opcode == 6'b100010) ? 2'b11: 2'bzz;
		
		//write_back select, 1 for imm_data or 0 for alu_result
		mux2to1_select = (opcode == 6'b100010) ? 1'b1 //MOVI
															: 1'b0; 
		
		//use imm for 1 or readdata2 for 0
		imm_reg_select = (opcode == 6'b100000 && ( sub_opcode == 5'b00000 || 
																sub_opcode == 5'b00001 || 
																sub_opcode == 5'b00010 || 
																sub_opcode == 5'b00100 ||
																sub_opcode == 5'b00011)) || 
						//LW, SW
						 (opcode == 6'b011100 && (sub_opcode == 8'b00000010 || sub_opcode == 8'b00001010)) ? 1'b0 : 1'b1;
	end
	S3 :
	begin
		ns = S0;
		enable_fetch = 1'b0;
		enable_execute = 1'b0;
		if((opcode == 6'b100000 && ir[24:5] == 20'd0 && sub_opcode == 5'b01001) || 
		//SWI, SW
		   opcode == 6'b001010 || (opcode == 6'b011100 && sub_opcode == 8'b00001010) ||
		   opcode == 6'b100110 || opcode == 6'b100100)	enable_writeback = 1'b0;
		else enable_writeback = 1'b1;
		mux4to1_select = mux4to1_select;
		mux2to1_select = mux2to1_select; 
		imm_reg_select = imm_reg_select;
		
		DM_enable_mem = 1'b1;
		DM_enable_fetch = 1'b0;
		DM_enable_write = (opcode == 6'b001010 || (opcode == 6'b011100 && sub_opcode == 8'b00001010)) ? 1'b1 : 1'b0;
	end
	endcase 
end
*/
/*
always@(posedge clock or posedge reset)
begin
	if(PC == 0)
		present_instruction = 0;
	else
		present_instruction = ir;
end*/
endmodule
