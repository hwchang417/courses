`timescale 1ns/100ps
module alu(alu_result,alu_overflow,src1,src2,opcode,sub_opcode,enable_execute,reset);
parameter NOP_OR_SRLI=5'b01001,ADD=5'b00000,SUB=5'b00001,AND=5'b00010,OR=5'b00100,XOR=5'b00011;
parameter SLLI=5'b01000,ROTRI=5'b01011;
parameter LW = 8'b00000010, SW = 8'b00001010;

output reg [31:0]alu_result;
output reg alu_overflow;

input [31:0]src1,src2;
input [5:0]opcode;
input [7:0]sub_opcode;
input reset;
input enable_execute;

reg[63:0]rotate;
reg a,b;

//reference https://sites.google.com/site/nonsugarmilktea/ji-suan-ji-gai-lun/di-wu-zhang-2

always@(*)
begin
	if(reset)
	begin
		alu_result = 32'b0;
		alu_overflow = 1'b0;
		rotate = 64'd0;
	end
	else if(enable_execute)
	begin
		case(opcode)
		6'b100000:
		begin
			case(sub_opcode)
			NOP_OR_SRLI:
			begin
				if(src1 == 32'd0 && src2 == 32'd0) //?
					alu_result = 32'b0000_0000_0000_0000_0000_0000_0000_0000_0000_0000;
				else
					alu_result = src1 >> src2[4:0]; //SRLI
				alu_overflow=1'b0;
			end
			ADD:
			begin
				alu_result = src1 + src2;
				if(src1[31] == 1'b1 && src2[31] == 1'b1)
					alu_overflow = ~alu_result[31];
				else if(src1[31] == 1'b0 && src2[31] == 1'b0)
					alu_overflow = alu_result[31];
				else
					alu_overflow = 1'b0;
			end
			SUB:
			begin
				alu_result = src1 - src2;
				
				if(src1[31] == 1'b1 && src2[31] == 1'b0)
					alu_overflow = ~alu_result[31];
				else if(src1[31] == 1'b0 && src2[31] == 1'b1)
					alu_overflow = alu_result[31];
				else
					alu_overflow = 1'b0;
			end
			AND:
			begin
				alu_result = src1 & src2;
				alu_overflow = 1'b0;
			end
			OR:
			begin
				alu_result = src1 | src2;
				alu_overflow = 1'b0;
			end
			XOR:
			begin
				alu_result = src1 ^ src2;
				alu_overflow = 1'b0;
			end
			SLLI:
			begin
				alu_result = src1 << src2[4:0];
				alu_overflow = 1'b0;
			end
			ROTRI:
			begin
				rotate[63:32] = src1;
				rotate[31:0]  = src1;
				rotate = rotate >> src2[4:0];
				alu_result = rotate[31:0];
				alu_overflow = 1'b0;
			end
			default:
			begin 
			  alu_result = 32'd0;
			  alu_overflow = 1'b0; 
			end
			endcase
		end
		6'b101000://ADDI
		begin
			//4 bits can be ranged to -8~7, overflow of ADDI operation only happen to x < -8 or x > 7 
							
			alu_result = src1[31:0] + ((src2[14]) ? {17'h1ffff, src2[14:0]} : {17'h00000, src2[14:0]});
			
			if(src1[31] == 1'b1 && src2[14] == 1'b1)
				alu_overflow = ~alu_result[31];
			else if(src1[31] == 1'b0 && src2[14] == 1'b0)
				alu_overflow = alu_result[31];
			else
				alu_overflow = 1'b0;
		end
		6'b101100://ORI
		begin
			alu_result = src1 | {17'd0, src2[14:0]};
			alu_overflow = 1'b0;
		end
		6'b101011://XORI
		begin
			alu_result = src1 ^ {17'd0, src2[14:0]};
			alu_overflow = 1'b0;
		end
		6'b000010://LWI
		begin
			alu_result = src1;
			alu_overflow = 1'b0;
		end
		6'b001010://SWI
		begin
			//no operation
			alu_result = 32'd0;
			alu_overflow = 1'b0;
		end
		6'b100010://MOVI
		begin
			alu_result = ((src2[19]) ? {12'hfff, src2[19:0]} : {12'h000, src2[19:0]});
			alu_overflow = 1'b0;
		end
		6'b011100://LW, SW
		begin
		
			case(sub_opcode)
			LW:
			begin
				alu_result = src1;
			end
			SW:
			begin
				//no operation
				alu_result = 32'd0;
			end
			default: begin alu_result = 32'd0; end
			endcase
			alu_overflow = 1'b0;
		end
		default:
		begin 
		  alu_result = 32'd0;
		  alu_overflow = 1'b0; 
		end
		endcase		
	end
	else
	begin
		  alu_result = 32'd0;
		  alu_overflow = 1'b0;
	end
end
endmodule

