
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.ALL;
use IEEE.fixed_float_types.all; -- ieee_proposed for VHDL-93 version
use IEEE.fixed_pkg.all; -- ieee_proposed for compatibility version
use IEEE.float_pkg.all; -- ieee_proposed for compatibility version
library std;
use std.textio.all;

entity tb_cosine is end;--empty entity

architecture arch_tb_cosine of tb_cosine is
	signal datain		: std_logic_vector(5 downto 0) := (others => '0'); 
	signal dataout		: std_logic_vector(9 downto 0) := (others => '0');
	-- signal datain_std	: std_logic_vector(5 downto 0) := (others => '0');
	-- signal dataout_std	: std_logic_vector(9 downto 0) := (others => '0');
	
	signal clk			: std_logic := '0';
	--delay
	signal result		: real := 0.0;
	signal period		: time := 10 ns;
	
	component cosine
		port (radium 	: in std_logic_vector(5 downto 0);
			  cos_value	: out std_logic_vector(9 downto 0)
			);
	end component;
begin
	-- datain_std <= to_std_logic_vector(datain);
	-- dataout <= to_sfixed(dataout_std, dataout'high, dataout'low);
	c1: cosine port map(radium => datain, cos_value => dataout);
	
	clk <= not clk after period;
	
	test:
	process
	file phase_file : text;
	file cos_file: text;
	variable f_status: FILE_OPEN_STATUS;
	variable phase_value, cos_value	: line;
	variable theta_variable		: real;
	begin
		file_open(phase_file, "phasedata.txt", read_mode);
		file_open(cos_file, "cosdata.txt", write_mode);
		while not endfile(phase_file) loop
			readline(phase_file, phase_value);
			read(phase_value, theta_variable);
			--c <= to_ufixed(5.125, 2, -3);
			datain <= std_logic_vector(to_signed(integer((theta_variable - 3.14159) * 10.0), 6));
			wait for period;
			--input ok
			result <= real(to_integer(signed(dataout)))/100.0;
			
			--output value ok
			report "-----------------------------";
			report "input " & real'image(theta_variable) & ", output = " & real'image(result);
			report "-----------------------------";
			write(cos_value, result);
			writeline(cos_file, cos_value);
			wait for period;
		end loop;
		file_close(phase_file);
		file_close(cos_file);
		wait;
	end process test;
	

end arch_tb_cosine;

