library ieee;
use ieee.std_logic_1164.all;
USE ieee.numeric_std.ALL;


use IEEE.STD_LOGIC_SIGNED.ALL;
use std.textio.all;
use ieee.float_pkg.all;


entity tb_LMSf is
	
end tb_LMSf;

architecture ar of tb_LMSf is

component LMSf is 
generic(
			constant size :  integer := 16;
			constant upl  :  integer := 4;--依據size正數最大值
			constant downl:  integer := -11--依據size負數最小值
);
port(
		clk,rst		: in std_logic;
		datain		: in float(upl downto downl);
		step		: in float(upl downto downl);
		dataout		: out float(upl downto downl);
		dataout1	: out float(upl downto downl);
		finish_flag : out std_logic
);
end component;

constant t_upl  :  integer := 4;--依據size正數最大值
constant t_downl:  integer := -11;--依據size負數最小值

signal tclk: std_logic := '0';
signal trst : std_logic := '1';
signal period : time := 20 ns;


signal t_datain : float(t_upl downto t_downl) := (others => '0');
signal t_step : float(t_upl downto t_downl);
signal t_dataout,t_dataout1 : float(t_upl downto t_downl);
signal t_finish_flag : std_logic;

signal s_window,s_window2,input : real;
signal endof : std_logic := '0';
signal input_flag : std_logic := '0';
signal f_input : float(t_upl downto t_downl);
signal test : real := 1.1;

begin

d0  : LMSf port map (tclk, trst, t_datain, t_step, t_dataout, t_dataout1, t_finish_flag);
  
tclk <= not tclk after period/2;
f_input <= to_float(test,f_input);
t_step  <= to_float(0.02,t_step);
process
begin
		trst <= '0' after period*3.5;
		wait;
end process;

  
-- process(tclk,trst)
-- begin
	-- if(trst = '0' and rising_edge(tclk)) then t_D <= t_D + '1'; end if;
-- end process;
 
 process(tclk)
 file data_in_file : text;
 file data_out_file: text;
 file data_out_file2: text;
 variable input_value, output_value, output_value2 : line;
 
 variable theta_variable  : real;
 variable window,window2 : real := 0.0;
 variable r_input : real;
 begin
 -- wait for period*3.5;
  if(endof = '0')then
  file_open(data_in_file, "datain.txt", read_mode);
  file_open(data_out_file, "dataout.txt", write_mode);
  file_open(data_out_file2, "dataout2.txt", write_mode);
  if(rising_edge(tclk) and trst = '0') then
	  if (not endfile(data_in_file) and endof = '0' and t_finish_flag = '1') then
	  
	   readline(data_in_file, input_value);
	   read(input_value, theta_variable);
	   
	   input <= theta_variable;
	   
	   r_input := theta_variable;
	   
	   t_datain <= to_float(r_input,t_datain);
	   
	   input_flag <= '1';
	   --window := theta_variable;
	   window := to_real(t_dataout);
	   window2 := to_real(t_dataout1);
	   
	   s_window <= window;
	   s_window2 <= window2;
	   
	   write(output_value, window);
	   writeline(data_out_file, output_value);
	   
	   write(output_value2, window2);
	   writeline(data_out_file2, output_value2);
	   
	   elsif( endfile(data_in_file) and endof = '0') then
	   file_close(data_in_file);
	   file_close(data_out_file);
	   file_close(data_out_file2);
	   endof <= '1';
	  end if;
  end if;
  end if;
 -- wait;
  
 end process ;
 
  
 
end ar;