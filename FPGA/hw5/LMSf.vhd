library ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
--use IEEE.STD_LOGIC_ARITH.ALL;
--use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.STD_LOGIC_SIGNED.ALL;
use ieee.float_pkg.all;

entity LMSf is
generic(
			constant size :  integer := 16;
			constant upl  :  integer := 8;--依據size正數最大值
			constant downl:  integer := -7--依據size負數最小值
);
port(
		clk,rst		: in std_logic;
		datain		: in float(upl downto downl);
		step		: in float(upl downto downl);
		dataout		: out float(upl downto downl);
		dataout1	: out float(upl downto downl);
		finish_flag : out std_logic
);
end LMSf;

architecture ar of LMSf is
	signal	wreg,wreg1, wreg2 		:	float(upl downto downl);--15 downto 0 16
	signal	xreg,xreg1, xreg2		:	float(upl downto downl);--15 sownto 0 16
 	
	signal	y					:	float(upl downto downl);-- 32 downto 0 33 
	signal	e					:	float(upl downto downl);-- 33 downto 0 34
	
	signal	temp1				:	float(upl downto downl);
	signal	temp2				:	float(upl downto downl);
	
	signal	x0,x1,x2,selecter,finish	:	std_logic;
	
	
	signal  vrtemp1 :	float(upl downto downl);
	signal  vrtemp2 :	float(upl downto downl);
	signal  vrtemp3 :	float(upl downto downl);
	signal  testf   :   float(upl downto downl);
begin
	
	
	
	
	process(clk,rst)
	begin
			if (rst = '1')then x0 <= '1';x1 <= '0';x2 <= '0';
			elsif (rising_edge(clk))then 
				if (x0 = '1') then x0 <= '0';x1 <= '1';
				elsif (x1 = '1') then x1 <= '0'; x2 <= '1';
				elsif (x2 = '1' and finish = '1') then x2 <= '0'; x1 <= '1';
				end if;
			end if;
	end process;
	
	process(clk,x0,x1,x2)
	--variable vrtemp1,vrtemp2,vitemp1,vitemp2 : std_logic_vector(15 downto 0);
	variable	y1, y2				:	float(upl downto downl);--31 downto 0 32
	variable	y3					:	float(upl downto downl);-- 32 downto 0 33 
	variable	e1					:	float(upl downto downl);-- 33 downto 0 34
	
	variable	vtemp1				:	float(upl downto downl);
	variable	vtemp2				:	float(upl downto downl);
	variable	vtemp3				:	float(upl downto downl);
	variable	vtemp4				:	float(upl downto downl);
	begin
		if(falling_edge(clk)) then
			if (x0 = '1') then 
				selecter <= '0'; wreg1 <= (others => '0'); wreg2 <= (others => '0'); xreg1 <= (others => '0'); xreg2 <= (others => '0');  y <= (others => '0'); 
				e <= (others => '0'); temp1 <= (others => '0'); temp2 <= (others => '0'); y1 := (others => '0'); y2 := (others => '0'); y3 := (others => '0');
				xreg  <= (others => '0'); wreg <= (others => '0'); finish <= '0';
				vtemp1 := (others => '0'); vtemp2 := (others => '0'); vtemp3 := (others => '0');
				
			elsif (x1 = '1') then
			
					y1 := xreg1 * wreg1; y2 := xreg2 * wreg2; 
					y3 := y1 + y2;
					y <= y3;
					e  <= datain - y3;
					--e <= e1;
					xreg <= xreg1;
					wreg <= wreg1;
					finish <= '0';
					
			elsif (x2 = '1') then
					vtemp1 := step * xreg;
					vtemp2 := vtemp1 * e;
					vtemp3 := vtemp2 + wreg;
					vrtemp1 <= vtemp1;
					vrtemp2 <= vtemp2;
					vrtemp3 <= vtemp3;
					selecter <= '1';
					
					
					xreg <= xreg2;
					wreg <= wreg2;
					
					if (selecter = '0') then
						finish <= '0';
						selecter <= '1';
						wreg1 <= vtemp3;
						
						dataout <= vtemp3;
					else
						selecter <= '0';
						finish <= '1';
						
						wreg2 <= vtemp3;
						xreg2 <= xreg1;
						xreg1 <= datain;
						dataout1 <= vtemp3;
					end if;
				
			end if;
		end if;
	end process;
	
finish_flag <= finish;
end ar;
