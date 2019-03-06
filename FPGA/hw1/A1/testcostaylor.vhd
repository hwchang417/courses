library ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use ieee.float_pkg.all;

entity testcostaylor is
generic(
		constant taylor : integer := 4
);
port(
		clk		: in 	std_logic;
		rst		: in 	std_logic;
		itheta	: in 	float(3 downto -2);
		oenable : out   std_logic;
		ovalue	: out float(3 downto -6)
);end testcostaylor;

architecture ar of testcostaylor is

signal   temp1, lasttheta, ans, theta2, temp2,change : float(6 downto -6);
signal i	: integer range 0 to taylor+1;
signal j	: integer range 0 to (taylor+1)*2; 
signal laststep, step : integer range 0 to 2147483647;
signal s0,s01,s1,s2,s3,s4,s5,s6,s7 : std_logic;


begin

	process(rst,clk)
	begin
		if(rst = '0') then
			s0 <= '1';s01 <= '0';s1 <= '0';s2 <= '0';s3 <= '0';s4 <= '0';s5 <= '0';oenable <='0';
		elsif(rising_edge(clk)) then
			if(s0 = '1') 	 then	s0 <= '0';s01 <= '1';oenable <='0';
			elsif(s01 = '1')then s01<= '0';s1 <= '1';
			elsif(s1 = '1') then	s1 <= '0';s2 <= '1';
			elsif(s2 = '1') then s2 <= '0';s3 <= '1';
			elsif(s3 = '1') then s3 <= '0';s4 <= '1';
			elsif(s4 = '1') then s4 <= '0';s5 <= '1';
			elsif(s5 = '1') then
				if(i > taylor) then 
					s5 <= '0';s0 <= '1';
					oenable <= '1';
				else
					s5 <= '0';s1 <= '1';
				end if;
			end if;
		end if;
	end process;
	
	process(clk)
	begin
		if(falling_edge(clk)) then
			if(s0 = '1')    then ans <= to_float(1.0,ans); change <= to_float(0.0,change); lasttheta <= to_float(1.0,lasttheta); temp1 <= to_float(0.0,temp1); i <= 1; j <= 2; laststep <= 1; step <= 0;theta2 <= resize(itheta,theta2);temp2 <= to_float(0.0,temp2);				
			elsif(s01 = '1') then theta2 <=  theta2 * theta2;
			elsif(s1 = '1') then	
				temp1	<= lasttheta * theta2;
				step 	<= laststep * (j-1) * j;
			elsif(s2 = '1') then
				lasttheta <= temp1;
				laststep	 <= step;
			elsif(s3 = '1') then 
				if((i rem 2) = 1) then
					temp2 <=  (-1)*temp1  / to_float(to_signed(step,32),temp2);
				else
					temp2 <=  temp1 / to_float(to_signed(step,32),temp2);
				end if;
			elsif(s4 = '1') then
				ans <= ans + temp2;
				i	 <=  i + 1;
				j	 <=  j + 2;
			end if;
		end if;
	end process;
	ovalue <= resize(ans,3,6);
	--ovalue <= ans;
end ar;
