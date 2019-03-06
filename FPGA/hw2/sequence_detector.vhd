library IEEE;
use IEEE.std_logic_1164.all;

entity sequence_detector is
	port(clk	: in std_logic;
		 input	: in std_logic;
		 output	: out std_logic);
end sequence_detector;

architecture arch_sequence_detector of sequence_detector is
	Type STATE_TYPE is (s0, s1, s2, s3, s4, s5, s6, s7);
	signal state : STATE_TYPE;
	signal out_data	: std_logic;
begin
	
	process(clk)
	begin
		if(clk'event and clk = '1') then
			if state = s0 then
				if input = '1' then
					state <= s0;
				else
					state <= s1;
				end if;
			elsif state = s1 then
				if input = '1' then
					state <= s0;
				else
					state <= s2;
				end if;
			elsif state = s2 then
				if input = '1' then
					state <= s3;
				else
					state <= s2;
				end if;
			elsif state = s3 then
				if input = '1' then
					state <= s4;
				else
					state <= s3;
				end if;
			elsif state = s4 then
				if input = '1' then
					state <= s5;
				else
					state <= s3;
				end if;
			elsif state = s5 then
				if input = '1' then
					state <= s5;
				else
					state <= s6;
				end if;
			elsif state = s6 then
				if input = '1' then
					state <= s4;
				else
					state <= s7;
				end if;
			elsif state = s7 then
				if input = '1' then
					state <= s3;
				else
					state <= s2;
				end if;
			end if;
		end if;
	end process;
	
	process(clk)
	begin
		if(clk'event and clk = '1') then
			if(state = s7) then
				out_data <= '1';
			else
				out_data <= '0';
			end if;
		end if;
	end process;
	output <= out_data;
end arch_sequence_detector;
