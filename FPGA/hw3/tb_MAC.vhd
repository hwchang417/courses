library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

library std;
use std.textio.all;

entity tb_MAC is
end tb_MAC;

architecture arch_tb_MAC of tb_MAC is
	signal clk, rst : std_logic := '0';
	signal clr : std_logic := '0';
	signal x_real, x_imag, y_real, y_imag : std_logic_vector(7 downto 0);
	signal real_result, imag_result : std_logic_vector(15 downto 0);
	signal ovf_result : std_logic;
	signal period : time := 10 ns;
	component MAC is
		port(clk : in std_logic;
			 rst : in std_logic;
			 clr : in std_logic;
			 x_real : in std_logic_vector(7 downto 0);
			 x_imag : in std_logic_vector(7 downto 0);
			 y_real : in std_logic_vector(7 downto 0);
			 y_imag : in std_logic_vector(7 downto 0);
			 s_real : out std_logic_vector(15 downto 0);
			 s_imag : out std_logic_vector(15 downto 0);
			 ovf : out std_logic);
	end component;
begin
	clk <= not clk after period;
	M1: MAC port map(clk => clk, rst => rst, clr => clr, x_real => x_real, 
					 x_imag => x_imag, y_real => y_real, y_imag => y_imag, 
					 s_real => real_result, s_imag => imag_result, 
					 ovf => ovf_result);
	process
	file integer_file : text;
	file result_file: text;
	variable f_status: FILE_OPEN_STATUS;
	variable integer_value, result_value	: line;
	variable xr, xi, yr, yi, input, output	: integer;
	begin
		rst <= '1';
		wait for period * 2;
		rst <= '0';
		
		file_open(integer_file, "input.txt", read_mode);
		file_open(result_file, "output.txt", write_mode);
		while not endfile(integer_file) loop
			--read input
			readline(integer_file, integer_value);
			read(integer_value, xr);
			readline(integer_file, integer_value);
			read(integer_value, xi);
			readline(integer_file, integer_value);
			read(integer_value, yr);
			readline(integer_file, integer_value);
			read(integer_value, yi);
			
			--get input
			x_real <= std_logic_vector(to_signed(integer(xr), 8));
			x_imag <= std_logic_vector(to_signed(integer(xi), 8));
			y_real <= std_logic_vector(to_signed(integer(yr), 8));
			y_imag <= std_logic_vector(to_signed(integer(yi), 8));
			wait for period;
			--get output
			--output <= real(to_integer(signed(dataout)))/100.0;
			
			--check input and output
			--report "input " & integer'image(input) & ", output = " & integer'image(output);
			report "input: " & integer'image(xr) & " " & integer'image(xi) & " " & integer'image(yr) & " " & integer'image(yi);
			
			--read output
			write(result_value, output);
			writeline(result_file, result_value);
			wait for period;
		end loop;
		file_close(integer_file);
		file_close(result_file);
		wait;
	end process;
	
end arch_tb_MAC;