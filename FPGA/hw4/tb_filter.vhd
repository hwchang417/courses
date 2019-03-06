library IEEE;

use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library std;
use std.textio.all;

entity tb_filter is
	generic(data_width : integer := 8;
			filter_width : integer := 8);
end tb_filter;

architecture arch_tb_filter of tb_filter is
	signal clk, rst : std_logic := '0';
	signal xin: std_logic_vector(data_width - 1 downto 0);
	signal yout: std_logic_vector(data_width - 1 downto 0);
	signal xin_csd: std_logic_vector(data_width - 1 downto 0);
	signal yout_csd: std_logic_vector(data_width - 1 downto 0);
	signal period 	: time := 10 ns;
	signal output_count : integer := 0;
	component filter_two_com is
		port(clk : in std_logic;
			 rst : in std_logic;
			 xin : in std_logic_vector(data_width - 1 downto 0);
			 yout: out std_logic_vector(data_width - 1 downto 0));
	end component;
	component filter_CSD is
		port(clk : in std_logic;
			 rst : in std_logic;
			 xin : in std_logic_vector(data_width - 1 downto 0);
			 yout: out std_logic_vector(data_width - 1 downto 0));
	end component;
begin
	f1: filter_two_com port map(clk => clk, rst => rst, xin => xin, yout => yout);
	f2: filter_CSD port map(clk => clk, rst => rst, xin => xin_csd, yout => yout_csd);
	
	clk <= not clk after period;
	
	
	process
	file integer_file : text;
	file result_file: text;
	file result_csd_file: text;
	variable integer_value, result_value, result_csd_value	: line;
	variable input, output, output_csd	: real;
	begin
		rst <= '1';
		wait for period * 2;
		rst <= '0';
		wait for period;
		
		file_open(integer_file, "input.txt", read_mode);
		file_open(result_file, "output.txt", write_mode);
		file_open(result_csd_file, "output_csd.txt", write_mode);
		while not endfile(integer_file) loop
			--read input
			readline(integer_file, integer_value);
			read(integer_value, input);
			
			--get input
			xin <= std_logic_vector(to_signed(integer(input), data_width));
			xin_csd <= std_logic_vector(to_signed(integer(input), data_width));
			
			wait for period*2;
			
			--get output
			output := real(to_integer(signed(yout)));
			output_csd := real(to_integer(signed(yout_csd)));
			
			--check input and output
			report "input: " & real'image(input) & " " & real'image(output) & real'image(output_csd);
			
			--read output
			write(result_value, output);
			writeline(result_file, result_value);
			
			write(result_csd_value, output_csd);
			writeline(result_csd_file, result_csd_value);
			output_count <= output_count + 1;
		end loop;
		
		wait for period*2;
		--read output
		output := real(to_integer(signed(yout)));
		write(result_value, output);
		writeline(result_file, result_value);
		
		output_csd := real(to_integer(signed(yout_csd)));
		write(result_csd_value, output_csd);
		writeline(result_csd_file, result_csd_value);
		output_count <= output_count + 1;
	
		wait for period*2;
		output := real(to_integer(signed(yout)));
		write(result_value, output);
		writeline(result_file, result_value);
		output_csd := real(to_integer(signed(yout_csd)));
		write(result_csd_value, output_csd);
		writeline(result_csd_file, result_csd_value);
		output_count <= output_count + 1;
		
		
		file_close(integer_file);
		file_close(result_file);
		file_close(result_csd_file);
		wait;
	end process;
	

end arch_tb_filter;
