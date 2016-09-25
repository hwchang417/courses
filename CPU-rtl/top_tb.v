`timescale 1ns/10ps
`include "IM.v"
`include "DM.v"
`define CYCLE 20
`define prog1

module top_tb;

  reg clk;
  reg rst;
  reg done;
  reg [31:0]regGolden[99:0];
  reg [31:0]memGolden[99:0];
  integer i;
  wire alu_overflow;
  wire [31:0]instruction;

  //IM
  wire IM_read, IM_write, IM_enable;
  wire [9:0] IM_address;
  
  //DM
  wire DM_read ,DM_enable;
  wire DM_write;
  wire [31:0] DM_in;
  wire [11:0] DM_in_address;
  wire [11:0] DM_out_address;
  wire [31:0] DM_out;
 
	top TOP(.IM_enable_fetch(IM_read),
		.IM_enable_write(IM_write),
		.IM_enable_mem(IM_enable),
		.IM_address(IM_address),
		.DM_in(DM_in),
		.DM_out(DM_out),
		.DM_enable_fetch(DM_read),
		.DM_enable_write(DM_write),
		.DM_enable_mem(DM_enable),
		.DM_in_address(DM_in_address),
		.DM_out_address(DM_out_address),
		.clk(clk),
        .rst(rst),
		.instruction(instruction),
		.alu_overflow(alu_overflow));
 
	DM DM1(.clk(clk),
		   .reset(rst),
		   .enable_fetch(DM_read),
		   .enable_writeback(DM_write),
		   .enable_mem(DM_enable),
		   .DM_in_address(DM_in_address),
		   .DM_out_address(DM_out_address),
		   .DMin(DM_in),
		   .DMout(DM_out));

    IM IM1(.IMout(instruction),
           .clk(clk),
		   .reset(rst),
		   .IM_enable_fetch(IM_read),
		   .IM_enable_write(IM_write),
		   .IM_enable_mem(IM_enable),
		   .IM_address(IM_address),
		   .IMin(32'd0));
  
  
  
	//clock gen.
	always #(`CYCLE/2) clk=~clk;
  
	initial
	begin
		clk=0;
		rst=1'b1;
		#(`CYCLE * 2)
		rst=1'b0;
		`ifdef prog1
			$readmemb("test01.prog",IM1.mem_data);
			$readmemh("regGolden01.dat", regGolden);
		`elsif prog2
			$readmemb("test02.prog",IM1.mem_data);
			$readmemh("regGolden02.dat", regGolden);
		`elsif prog3
			$readmemb("test03.prog",IM1.mem_data);
			$readmemh("regGolden03.dat", regGolden);
		`endif
		
		#(50000 * `CYCLE)
		
		$display( "done" );
		for( i=0;i<150;i=i+1 ) $display( "IM[%d]=%h",i,IM1.mem_data[i]); 
		for( i=0;i<32;i=i+1 )
		begin
			$display( "register[%d]=%h(golden=%h)",i,TOP.regfile1.rw_reg[i],regGolden[i]);
			if(TOP.regfile1.rw_reg[i] != regGolden[i]) $display("ERROR");
		end
		for( i=0;i<32*1024;i=i+1 ) if(DM1.mem_data[i] != 0) $display( "DM[%d]=%h",i,DM1.mem_data[i] );
		$finish;
	end

	//initial begin
	//$fsdbDumpfile("top.fsdb");
	//$fsdbDumpvars(0, top_tb);
	//end

endmodule
