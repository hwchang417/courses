`timescale 1ns/10ps
module reg_f(read_data0, read_data1, read_data2, read_address0, read_address1, read_address2,
				write_address, write_data, clk, reset, read, write);

				
parameter DataSize = 32;
parameter AddrSize = 5;

output reg [DataSize - 1:0] read_data0;
output reg [DataSize - 1:0] read_data1;
output reg [DataSize - 1:0] read_data2;

input [AddrSize - 1:0] read_address0;
input [AddrSize - 1:0] read_address1;
input [AddrSize - 1:0] read_address2;
input [AddrSize - 1:0] write_address;
input [DataSize - 1:0] write_data;
input clk, reset, read, write;

reg [DataSize - 1:0] rw_reg[31:0];
integer i;

always @(posedge clk or posedge reset)begin
	if(reset) begin
		read_data0 = 32'd0;
		read_data1 = 32'd0;
		read_data2 = 32'd0;
		for(i = 0;i < 32;i = i + 1)
			rw_reg[i] = 32'd0;
	end
	else begin
		if(write) begin
			rw_reg[write_address] = write_data;
		end
		if(read) begin
			read_data0 = rw_reg[read_address0];
			read_data1 = rw_reg[read_address1];
			read_data2 = rw_reg[read_address2];		
		end
		else begin
			read_data0 = 32'd0;
			read_data1 = 32'd0;
			read_data2 = 32'd0;
		end
	end
end
	
endmodule

