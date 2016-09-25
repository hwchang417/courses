module IM(clk,reset,IM_address,IM_enable_fetch,IM_enable_write,IM_enable_mem,IMin,IMout);

parameter data_size=32;
parameter men_size=1024;

input clk,reset,IM_enable_fetch,IM_enable_write,IM_enable_mem;
input [9:0]IM_address;
input [data_size-1:0]IMin;

output reg [data_size-1:0]IMout;
reg [data_size-1:0]mem_data[men_size-1:0];

integer i;

always@(posedge clk)
begin
	if(reset)
	begin
		for(i=0;i<men_size;i=i+1)
			mem_data[i] <= 0;
		IMout <= 32'd0;
	end
	else if(IM_enable_mem)
	begin
		if(IM_enable_fetch)
			IMout <= mem_data[IM_address];
		else if(IM_enable_write)
		begin
			mem_data[IM_address] <= IMin;
		end
	end
end
endmodule
