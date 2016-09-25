module DM(clk, reset, enable_fetch, enable_writeback, enable_mem, DMin, DMout, DM_in_address, DM_out_address);

parameter data_size=32;
parameter mem_size=4096;

input clk,reset;
input enable_fetch,enable_writeback,enable_mem;
input [data_size-1:0] DMin;
input [11:0] DM_in_address;
input [11:0] DM_out_address;
output reg [data_size-1:0] DMout;
reg [data_size-1:0] mem_data[mem_size-1:0];
integer i;

always@(posedge clk or posedge reset)
begin
	if(reset)
	begin
		for(i=0;i<mem_size;i=i+1)
			mem_data[i] = 0;
		DMout = 0;
	end
	else if(enable_mem==1)
	begin
		if(enable_writeback==1)
			mem_data[DM_in_address] = DMin;
		if(enable_fetch==1)
			DMout = mem_data[DM_out_address];
	end
end
endmodule

