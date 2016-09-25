`timescale 1ns/10ps
module top(clk, rst, instruction, IM_enable_fetch, IM_enable_write, IM_enable_mem, IM_address, alu_overflow,
			DM_out, DM_enable_fetch, DM_enable_write, DM_enable_mem, DM_in, DM_in_address, DM_out_address);

parameter DataSize=32;
parameter AddrSize=5;
parameter memSize=10;

input clk, rst;
input [31:0] DM_out;
input [31:0] instruction;
output wire alu_overflow;

wire [5:0]opcode;
wire [7:0]sub_opcode;
wire enable_execute,enable_fetch,enable_writeback;
wire [31:0]read_data0, read_data1, read_data2;//,write_data;


wire flag_IF, flag_ID, flag_EX, flag_MEM, flag_WB;

////////////IF
output wire IM_enable_fetch;
output wire IM_enable_write;
output wire IM_enable_mem;
output wire [memSize-1:0] IM_address;

reg [5:0]IF_opcode;
reg [7:0]IF_sub_opcode;
reg [31:0]IF_ir;
wire [31:0] ir = (instruction == 32'd0) ? 32'b01000000000000000000000000001001 : instruction;

reg [4:0] imm_5bit;
reg [14:0] imm_15bit;
reg [19:0] imm_20bit;

reg [AddrSize-1:0]read_address0;
reg [AddrSize-1:0]read_address1;
reg [AddrSize-1:0]read_address2;
reg [AddrSize-1:0]write_address;

////////////ID
wire [1:0]mux4to1_select;
wire mux2to1_select,imm_reg_select;
reg [1:0]ID_mux4to1_select;
reg ID_mux2to1_select,ID_imm_reg_select;

reg [DataSize - 1:0]imm_out;

reg [31:0] write_data_buf;
reg [AddrSize-1:0] ID_write_address_buf;
reg [31:0] read_data0_buf, read_data1_buf, read_data2_buf;

reg [5:0]ID_opcode;
reg [7:0]ID_sub_opcode;
reg [31:0]ID_ir;

reg [AddrSize-1:0]ID_read_address0;
reg [AddrSize-1:0]ID_read_address1;
reg [AddrSize-1:0]ID_read_address2;
reg [AddrSize-1:0]ID_write_address;
reg isDataHazard0, isDataHazard1, isDataHazard2;

////////////EX

reg [31:0] src1_data;
reg [AddrSize-1:0] EX_write_address_buf;
reg [5:0]EX_opcode;
reg [7:0]EX_sub_opcode;
reg [31:0]EX_ir;
wire [DataSize - 1:0]alu_result;
reg [DataSize - 1:0]imm_data;
reg [DataSize - 1:0] EX_imm_data;

reg [31:0] EX_read_data0_buf, EX_read_data1_buf, EX_read_data2_buf;

reg [AddrSize-1:0]EX_read_address0;
reg [AddrSize-1:0]EX_read_address1;
reg [AddrSize-1:0]EX_read_address2;
reg [AddrSize-1:0]EX_write_address;
////////////MEM
//DM
reg [5:0]MEM_opcode;
reg [7:0]MEM_sub_opcode;
reg [31:0]MEM_ir;
reg [31:0] MEM_read_data0_buf, MEM_read_data1_buf, MEM_read_data2_buf;

reg [31:0] MEM_write_data_buf;
reg [AddrSize-1:0] MEM_write_address_buf;
output wire DM_enable_fetch,DM_enable_write,DM_enable_mem;
output reg [31:0] DM_in;
output reg [11:0] DM_in_address;
output reg [11:0] DM_out_address;

wire isBranch;

////////////IF
always@(negedge clk)
begin
  if(rst)
  begin
	imm_5bit <= 5'd0;
	imm_15bit <= 15'd0;
	imm_20bit <= 20'd0;
	
	read_address0 <= 5'd0; //AddrSize <= 5
	read_address1 <= 5'd0;
	read_address2 <= 5'd0;
	write_address <= 5'd0;
	IF_ir <= ir;
	IF_opcode <= ir[30:25];
	IF_sub_opcode <= (ir[30:25] == 6'b011100) ? ir[7:0] : {3'd0, ir[4:0]};

	imm_5bit <= ir[14:10];
	imm_15bit <= ir[14:0];
	imm_20bit <= ir[19:0];
	
	read_address0 <= ir[24:20];	
	read_address1 <= ir[19:15];
	read_address2 <= ir[14:10];
	write_address <= ir[24:20];
  end
  else
  begin
	if(flag_IF && !isBranch)
	begin
		IF_ir <= ir;
		IF_opcode <= ir[30:25];
		IF_sub_opcode <= (ir[30:25] == 6'b011100) ? ir[7:0] : {3'd0, ir[4:0]};
	
		imm_5bit <= ir[14:10];
		imm_15bit <= ir[14:0];
		imm_20bit <= ir[19:0];
		
		read_address0 <= ir[24:20];	
		read_address1 <= ir[19:15];
		read_address2 <= ir[14:10];
		write_address <= ir[24:20];
	end
	else if(isBranch)
	begin
		IF_ir <= 32'b0_100000_00000_00000_00000_00000_01001;
		IF_opcode <= 6'b100000;
		IF_sub_opcode <= 8'b00001001;
	
		imm_5bit <= 5'b00000;
		imm_15bit <= 15'b00000;
		imm_20bit <= 20'b00000;
		
		read_address0 <= 5'b00000;	
		read_address1 <= 5'b00000;
		read_address2 <= 5'b00000;
		write_address <= 5'b00000;
		
	end
  end
end	  

PC p1(.clk(clk),
	  .rst(rst),
	  .address(IM_address),
	  .enable_fetch(IM_enable_fetch),
	  .enable_mem(IM_enable_mem),
	  .ir(ir),
	  .Rt(read_data0_buf),
	  .Ra(read_data1_buf),
	  .isBranch(isBranch));	  
	
////////////ID
	
always@(negedge clk)
begin
  if(rst)
  begin
	imm_out = 32'd0;
  end
  else
  begin
	if(flag_ID)
	begin
	//table1, row3
		imm_out = // 5bit ZE
				(mux4to1_select == 2'b00) ? {27'd0, imm_5bit} 
				// 15bit SE
				:(mux4to1_select == 2'b01) ? ((imm_15bit[14] == 1'b1) ? {17'h1ffff, imm_15bit} : {17'h00000, imm_15bit})
				// 15bit ZE
				:(mux4to1_select == 2'b10) ? {17'h00000, imm_15bit}
				// 20bit SE
				:(mux4to1_select == 2'b11) ? ((imm_20bit[19] == 1'b1) ? {12'hfff, imm_20bit} : {12'h000, imm_20bit})
				:32'd0;
		
	end
  end
end

		//two operand
parameter LOGIC_opcode = 6'b100000, LW_SW = 6'b011100;
		//one operand
parameter ADDI = 6'b101000, ORI = 6'b101100, XORI = 6'b101011, LWI = 6'b000010, SWI = 6'b001010;
parameter BEQ = 6'b100110, BNE = 6'b100110;

always@(negedge clk)
begin
	if(rst)
	begin
		isDataHazard0 = 1'b0;
		isDataHazard1 = 1'b0;
		isDataHazard2 = 1'b0;
	end
	else 
	begin
		if(flag_EX)
		begin
		//去判斷這一次和上一次運算動作有無使用到會相同的資料
			if(ID_opcode == 6'b011100 && ID_sub_opcode == 8'b00001010 || 
				(ID_opcode == LOGIC_opcode && (ID_sub_opcode == 8'b00001001 && ID_ir[24:10] == 15'd0)) ||
				ID_opcode == BEQ || ID_opcode == BNE || ID_opcode == SWI)
			begin
				isDataHazard0 = 1'b0;
				isDataHazard1 = 1'b0;
				isDataHazard2 = 1'b0;
			end
			//one operand
			else if(IF_opcode == ADDI || IF_opcode == ORI || IF_opcode == XORI || IF_opcode == LWI)
			begin
				isDataHazard0 = 1'b0;
				isDataHazard1 = (ID_write_address == read_address1);
				isDataHazard2 = 1'b0;
			end
			else if(IF_opcode == BEQ || IF_opcode == BNE || IF_opcode == SWI)
			begin
				isDataHazard0 = (ID_write_address == read_address0);
				isDataHazard1 = (ID_write_address == read_address1);
				isDataHazard2 = 1'b0;
			end
			//two operand
			else if(IF_opcode == LOGIC_opcode || IF_opcode == LW_SW)
			begin
				//NOP
				if((IF_sub_opcode == 8'b00001001 && IF_ir[24:10] == 15'd0))
				begin
					isDataHazard0 = 1'b0;
					isDataHazard1 = 1'b0;
					isDataHazard2 = 1'b0;
				end
				else
				begin
					isDataHazard0 = (ID_write_address == read_address0);
					isDataHazard1 = (ID_write_address == read_address1);
					isDataHazard2 = (ID_write_address == read_address2);
				end
			end
			//no operand
			else
			begin
				isDataHazard0 = 1'b0;
				isDataHazard1 = 1'b0;
				isDataHazard2 = 1'b0;
			end
		end
		else
		begin
			isDataHazard0 = 1'b0;
			isDataHazard1 = 1'b0;
			isDataHazard2 = 1'b0;
		end
	end
end

always@(negedge clk)
begin
  if(rst)
  begin
	read_data0_buf <= 32'd0;
	read_data1_buf <= 32'd0;
	read_data2_buf <= 32'd0;
	
	ID_opcode <= 6'd0;
	ID_sub_opcode <= 8'd0;
	ID_ir <= 32'd0;
	
	ID_mux4to1_select <= 2'd0;
	ID_mux2to1_select <= 1'd0;
	ID_imm_reg_select <= 1'd0;
	ID_write_address_buf <= 5'd0;
	imm_data <= 32'd0;
	src1_data <= 32'd0;
	
	ID_read_address0 <= 5'd0; //AddrSize <= 5
	ID_read_address1 <= 5'd0;
	ID_read_address2 <= 5'd0;
	ID_write_address <= 5'd0;
	
	DM_out_address <= 12'd0;
	DM_in_address <= 12'd0;
	DM_in <= 32'd0;
  end
  else
  begin
	if(flag_ID)
	begin
		
		ID_read_address0 <= read_address0;	
		ID_read_address1 <= read_address1;
		ID_read_address2 <= read_address2;
		ID_write_address <= write_address;
			
		read_data0_buf <= (isDataHazard0) ? alu_result : read_data0;
		read_data1_buf <= (isDataHazard1) ? alu_result : read_data1;
		read_data2_buf <= (isDataHazard2) ? alu_result : read_data2;
		
		ID_opcode <= IF_opcode;
		ID_sub_opcode <= IF_sub_opcode;
		ID_ir <= IF_ir;
	
		ID_mux4to1_select <= mux4to1_select;
		ID_mux2to1_select <= mux2to1_select;
		ID_imm_reg_select <= imm_reg_select;
		
		ID_write_address_buf <= write_address;
		/////////////////////////
		imm_data <= (imm_reg_select == 1'b1) ? imm_out : ((isDataHazard2) ? alu_result : (isDataHazard2) ? alu_result : read_data2); //table1, row2
		src1_data <= ((isDataHazard1) ? alu_result : (isDataHazard1) ? alu_result : read_data1);
		
		DM_out_address <= //LWI
				((IF_opcode == 6'b000010) ? (((isDataHazard1) ? alu_result : read_data1) + (imm_out << 2)) :
				//LW
				((IF_opcode == 6'b011100 && IF_sub_opcode == 8'b00000010) ? ((isDataHazard1) ? alu_result : read_data1) + (((isDataHazard2) ? alu_result : read_data2) << IF_ir[9:8]): 12'd0));		
			
	end
  end
end

reg_f regfile1( .read_data0(read_data0), 
				.read_data1(read_data1), 
				.read_data2(read_data2), 
				.read_address0(read_address0),
				.read_address1(read_address1),
				.read_address2(read_address2),
				.write_address(EX_write_address_buf),
				.write_data(((EX_opcode == 6'b011100 && EX_sub_opcode == 8'b00000010) || 
							   EX_opcode == 6'b000010) ? DM_out : write_data_buf),
				.clk(clk),
				.reset(rst),
				.read(enable_fetch),
				.write(enable_writeback));
				

////////////EX
always@(negedge clk)
begin
	if(rst)
	begin
		write_data_buf <= 32'd0;
		
		EX_opcode <= 6'd0;
		EX_ir <= 32'd0;
		EX_sub_opcode <= 8'd0;
		EX_read_data0_buf <= 32'd0;
		EX_read_data1_buf <= 32'd0;
		EX_read_data2_buf <= 32'd0;
		
		EX_write_address_buf <= 5'd0;
		EX_imm_data <= 32'd0;
		
		EX_read_address0 <= 5'd0;	
		EX_read_address1 <= 5'd0;
		EX_read_address2 <= 5'd0;
		EX_write_address <= 5'd0;
		
	end
	else
	begin
		if(flag_EX)
		begin
			EX_opcode <= ID_opcode;
			EX_ir <= ID_ir;
			EX_sub_opcode <= ID_sub_opcode;
			
			EX_read_data0_buf <= read_data0_buf;
			EX_read_data1_buf <= read_data1_buf;
			EX_read_data2_buf <= read_data2_buf;
			
			EX_write_address_buf <= ID_write_address_buf;
			
			//write_data <<= (ID_mux2to1_select <=<= 1'b1) ? imm_data : alu_result; //table1, row3
			write_data_buf <= ((ID_mux2to1_select == 1'b1) ? imm_data : alu_result);
			
			DM_in_address = //SWI
					((ID_opcode == 6'b001010) ? read_data1_buf + (imm_data << 2) :
					//SW
					((ID_opcode == 6'b011100 && ID_sub_opcode == 8'b00001010) ? read_data1_buf + (read_data2_buf << ID_ir[9:8]): 12'd0));
			DM_in = read_data0_buf;
			EX_imm_data <= imm_data;
			
			EX_read_address0 <= ID_read_address0;	
			EX_read_address1 <= ID_read_address1;
			EX_read_address2 <= ID_read_address2;
			EX_write_address <= ID_write_address;
			//////////////////////////////////////////////////
			//如果reg還沒存入，則直接拉來用。
			//DM_out_address <= //LWI
			//		((ID_opcode == 6'b000010) ? (((isDataHazard1) ? alu_result : read_data1) + ((imm_reg_select == 1'b1) ? imm_out : ((isDataHazard2) ? alu_result : read_data2) << 2)) :
			//		//LW
			//		((ID_opcode == 6'b011100 && ID_sub_opcode == 8'b00000010) ? ((isDataHazard1) ? alu_result : read_data1) + (((isDataHazard2) ? alu_result : read_data2) << ID_ir[9:8]): 12'd0));		
			//DM_out_address <= //LWI
			//		((ID_opcode == 6'b000010) ? (read_data1_buf + (imm_data << 2)) :
			//		//LW
			//		((ID_opcode == 6'b011100 && ID_sub_opcode == 8'b00000010) ? read_data1_buf + (read_data2_buf << ID_ir[9:8]): 12'd0));		
		
		end
	end
end


alu ALU(.alu_result(alu_result),
		.alu_overflow(alu_overflow),
		.src1(src1_data),
		.src2(imm_data),
		.opcode(ID_opcode),
		.sub_opcode(ID_sub_opcode),
		.enable_execute(enable_execute),
		.reset(rst));	
				
				
Controller controller1(.enable_execute(enable_execute),
					   .enable_fetch(enable_fetch),
					   .enable_writeback(enable_writeback),
					   .opcode(opcode),
					   .sub_opcode(sub_opcode),
					   .mux4to1_select(mux4to1_select),
					   .mux2to1_select(mux2to1_select),
					   .imm_reg_select(imm_reg_select),
					   
					   .IM_enable_fetch(IM_enable_fetch),
					   .IM_enable_write(IM_enable_write),
					   .IM_enable_mem(IM_enable_mem),
					   
					   .DM_enable_fetch(DM_enable_fetch),
					   .DM_enable_write(DM_enable_write),
					   .DM_enable_mem(DM_enable_mem),
					   
					   .flag_IF(flag_IF), 
					   .flag_ID(flag_ID),
					   .flag_EX(flag_EX),
					   .flag_MEM(flag_MEM),
					   .flag_WB(flag_WB),
					   
					   .clock(clk),
					   .reset(rst),
					   .PC(IM_address),
					   .ir(ir));
	  
	  
////////////MEM
always@(negedge clk)
begin
	if(rst)
	begin
		MEM_opcode <= 6'd0;
		MEM_sub_opcode <= 8'd0;
		MEM_ir <= 32'd0;
		MEM_read_data0_buf <= 32'd0;
		MEM_read_data1_buf <= 32'd0;
		MEM_read_data2_buf <= 32'd0;
		MEM_write_data_buf <= 32'd0;
		MEM_write_address_buf <= 5'd0;
		
	end
	else
	begin
		if(flag_MEM)
		begin
			
			MEM_opcode <= EX_opcode;
			MEM_sub_opcode <= EX_sub_opcode;
			MEM_ir <= EX_ir;
			
			MEM_read_data0_buf <= EX_read_data0_buf;
			MEM_read_data1_buf <= EX_read_data1_buf;
			MEM_read_data2_buf <= EX_read_data2_buf;
			
			MEM_write_address_buf <= EX_write_address_buf;
			MEM_write_data_buf <= write_data_buf;
		end
	end
end			
			

////////////WB
always@(negedge clk)
begin
	if(rst)
	begin
		
	end
	else
	begin
	
	
	end
end


endmodule
