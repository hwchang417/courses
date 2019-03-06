library IEEE;
use IEEE.std_logic_1164.all;

entity tb_sequence_detector is
end tb_sequence_detector;

architecture arch_tb_sequence_detector of tb_sequence_detector is
	signal clk, input, output : std_logic := '0';
	signal ROM_IN	: std_logic_vector(32 downto 0) := "001010011100100111001001110000101";
	signal ROM_OUT	: std_logic_vector(32 downto 0) := "000000000000000000000000000000000";
	signal period	: time := 10 ns;
	signal i,j		: integer range 0 to 32;
	
	component sequence_detector is
		port(clk	: in std_logic;
			 input	: in std_logic;
			 output	: out std_logic);
	end component;
begin
	
	u1: sequence_detector port map(clk => clk, input => input, output => output);
	clk <= not clk after period;
	
	--rising edge
	process
	begin
		for i in 32 downto 0 loop
			input <= ROM_IN(i);
			wait for period * 2;
		end loop;
		wait;
	end process;
	
	--rising edge with 1 clock delay
	process
	begin
		wait for period * 2;
		for j in 32 downto 0 loop
			wait for period * 2;
			ROM_OUT(j) <= output;
		end loop;
		wait;
	end process;
end arch_tb_sequence_detector;
