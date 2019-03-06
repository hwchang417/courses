library ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

use std.textio.all;
use ieee.float_pkg.all;

entity te_testcostaylor is
end te_testcostaylor;


architecture ar of te_testcostaylor is

component testcostaylor 
generic(
		constant taylor : integer := 4
);
port(
		clk		: in 	std_logic;
		rst		: in 	std_logic;
		itheta	: in 	float(3 downto -2);
		oenable : out   std_logic;
		ovalue	: out 	float(3 downto -6)
); end component;

signal t_clk : std_logic := '0';
signal period : time := 20 ns;
signal t_out,tt : std_logic_vector(3 downto 0) := "0000";
signal t_rst ,t_oenable: std_logic := '0';
signal t_ovalue : float(3 downto -6);
signal t_itheta : float(3 downto -2);
signal s_window : real := 0.0;
--signal window : real := 0.0;

-- signal look : real;
-- signal test : float (8 downto -20);
-- signal testreal : real := 0.3;
-- signal testreal2 : real := 0.3;
-- signal temp : float (8 downto -20);
-- signal test2 : float (8 downto -20);
-- signal temp2 : real := 0.0;
begin
-- t_itheta <= to_float(-3.14,t_itheta);
 u1 : testcostaylor port map (t_clk,t_rst,t_itheta,t_oenable,t_ovalue);
 t_clk <= not t_clk after period/2;
-- window <= to_real(t_ovalue);
process
begin
	t_rst <= '1' after 3*period;
	wait;
end process;


process
 file phase_file : text;
 file cos_file: text;
 

 variable phase_value, cos_value : line;
 
 variable theta_variable  : real;
 variable window : real := 0.0;
 
 begin
  wait for 2*period;
  file_open(phase_file, "phasedata.txt", read_mode);
  file_open(cos_file, "cosdata.txt", write_mode);
 -- if(rising_edge(t_clk)) then
  while not endfile(phase_file) loop
   readline(phase_file, phase_value);
   read(phase_value, theta_variable);
   t_itheta <= to_float(theta_variable-3.14,t_itheta);
   --test20 <= std_logic_vector(to_signed(integer((theta_variable-3.14)*10.0),6));
   
   wait for 22*period;
   window := to_real(t_ovalue);
   s_window <= window;
   write(cos_value, window);
   writeline(cos_file, cos_value);
   
  end loop;
  file_close(phase_file);
  file_close(cos_file);
-- end if;
wait;
 end process ;
-- test <= to_float(testreal,test);

-- test2 <= to_float(testreal2,test2);

-- temp <= test * test2; 

-- temp2 <= to_real(temp);
end ar;
