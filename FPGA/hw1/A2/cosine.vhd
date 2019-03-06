
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.ALL;
USE IEEE.NUMERIC_STD.ALL;

entity cosine is
port (radium 	: in std_logic_vector(5 downto 0);
	  cos_value	: out std_logic_vector(9 downto 0)
	);
end cosine;

architecture arch_cos of cosine is

	signal r2	: std_logic_vector(11 downto 0); --6bit * 6bit => maximum needs 12 bits
	signal r4	: std_logic_vector(23 downto 0);
	signal r6	: std_logic_vector(35 downto 0);
	signal r8	: std_logic_vector(47 downto 0);
	
	signal a1	: std_logic_vector(11 downto 0);
	signal a2	: std_logic_vector(23 downto 0);
	signal a3	: std_logic_vector(35 downto 0);
	signal a4	: std_logic_vector(47 downto 0);
	
	signal a5	: std_logic_vector(47 downto 0);
	signal a6	: std_logic_vector(47 downto 0);
	signal a7	: std_logic_vector(47 downto 0);
	signal a8	: std_logic_vector(47 downto 0);
	signal o	: std_logic_vector(47 downto 0);
begin
	r2 <= std_logic_vector(to_signed(to_integer(signed(radium) * signed(radium)),12));
	r4 <= r2 * r2;
	r6 <= r4 * r2;
	r8 <= r4 * r4;
	
	-- 
	a1 <= std_logic_vector(to_signed(to_integer(signed(r2) / 2), 12));
	a2 <= std_logic_vector(to_signed(to_integer(signed(r4) / 24), 24));
	a3 <= std_logic_vector(to_signed(to_integer(signed(r6) / 720), 36));
	a4 <= std_logic_vector(to_signed(to_integer(signed(r8) / 40320), 48));
	-- 保留1000倍
	-- a5 <= std_logic_vector(to_signed(to_integer(signed(a1) * 10), 48));
	-- a6 <= std_logic_vector(to_signed(to_integer(signed(a2) / 10), 48));
	-- a7 <= std_logic_vector(to_signed(to_integer(signed(a3) / 1000), 48));
	-- a8 <= std_logic_vector(to_signed(to_integer(signed(a4) / 100000), 48));
	-- 保留100倍
	a5 <= std_logic_vector(to_signed(to_integer(signed(a1)), 48));
	a6 <= std_logic_vector(to_signed(to_integer(signed(a2) / 100), 48));
	a7 <= std_logic_vector(to_signed(to_integer(signed(a3) / 10000), 48));
	a8 <= std_logic_vector(to_signed(to_integer(signed(a4) / 1000000), 48));
	
	-- 若捨棄1個低位元，要補齊1位元，讓負數的數值*2
	-- 若捨棄2個低位元，要補齊2位元，讓負數的數值*4
	-- o <= "1111101000" - a5 + a6 - a7 + a8;
	o <= "1100100" - a5 + a6 - a7 + a8;
	
	cos_value <= o(47) & o(8 downto 0);
end arch_cos;

