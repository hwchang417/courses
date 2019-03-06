library IEEE;

use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity MAC is
	--generic(bits : integer := 8);
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
end MAC;

architecture arch_MAC of MAC is
	signal X0, X1, X2, X3, X4, X5, X6 : std_logic;
	signal x_real_reg, x_imag_reg, y_real_reg, y_imag_reg : std_logic_vector(7 downto 0);
	signal rr, ri, ir, ii : std_logic_vector(15 downto 0);
	--signal ovf_rr, ovf_ri, ovf_ir, ovf_ii : std_logic;
	signal real_part, imag_part : std_logic_vector(15 downto 0);
	signal ovf_part : std_logic;
	signal real_acc, imag_acc : std_logic_vector(15 downto 0);
	--signal ovf_acc : std_logic;
	signal real_result, imag_result : std_logic_vector(15 downto 0);
	signal ovf_result : std_logic;
begin
	s_real <= real_result;
	s_imag <= imag_result;
	datapath:process(clk)
	variable real_part_tmp, imag_part_tmp : std_logic_vector(15 downto 0);
	variable real_acc_tmp, imag_acc_tmp : std_logic_vector(15 downto 0);
	variable ovf_acc : std_logic;
	begin
		if(clk'event and clk = '1') then
			if(X0 = '1') then
				x_real_reg <= (others => '0'); x_imag_reg <= (others => '0');
				y_real_reg <= (others => '0'); y_imag_reg <= (others => '0');
				rr <= (others => '0'); ri <= (others => '0');
				ir <= (others => '0'); ii <= (others => '0');
				--ovf_rr <= '0'; ovf_ri <= '0'; ovf_ir <= '0'; ovf_ii <= '0';
				real_part <= (others => '0'); imag_part <= (others => '0');
				ovf_part <= '0';
				real_acc <= (others => '0'); imag_acc <= (others => '0');
				ovf_acc := '0';
				real_result <= (others => '0'); imag_result <= (others => '0');
				ovf_result <= '0';
			else
				if(X1 = '1') then
					x_real_reg <= x_real; x_imag_reg <= x_imag;
					y_real_reg <= y_real; y_imag_reg <= y_imag;
				end if;
				if(X2 = '1') then
					rr <= signed(x_real_reg) * signed(y_real_reg);
					ri <= signed(x_real_reg) * signed(y_imag_reg);
					ir <= signed(x_imag_reg) * signed(y_real_reg);
					ii <= signed(x_imag_reg) * signed(y_imag_reg);
					--ovf_rr <= '0'; ovf_ri <= '0'; ovf_ir <= '0'; ovf_ii <= '0';
				end if;
				if(X3 = '1') then
					real_part_tmp := signed(rr) - signed(ii);
					imag_part_tmp := signed(ri) + signed(ir);
					real_part <= real_part_tmp;
					imag_part <= imag_part_tmp;
					
					ovf_part <= (not real_part_tmp(15) and rr(15) and not ii(15)) or 
								(real_part_tmp(15) and not rr(15) and ii(15)) or
								(imag_part_tmp(15) and not ri(15) and not ir(15)) or 
								(not imag_part_tmp(15) and ri(15) and ir(15));
					
				end if;
				if(X4 = '1') then
					real_acc_tmp := signed(real_acc) + signed(real_part);
					imag_acc_tmp := signed(imag_acc) + signed(imag_part);
				
					ovf_acc :=	(real_acc_tmp(15) and not real_acc(15) and not real_part(15)) or
								(not real_acc_tmp(15) and real_acc(15) and real_part(15)) or
								(imag_acc_tmp(15) and not imag_acc(15) and not imag_part(15)) or
								(not imag_acc_tmp(15) and imag_acc(15) and imag_part(15)) or
								ovf_part;
					if(ovf_acc = '0') then			
						real_acc <= real_acc_tmp;
						imag_acc <= imag_acc_tmp;
					else
						real_acc <= (others => '0');
						imag_acc <= (others => '0');
					end if;
				end if;
				if(X5 = '1' and (clr = '1' or ovf_acc = '1')) then
					real_result <= (others => '0');
					imag_result <= (others => '0');
					ovf_result <= ovf_acc;
				end if;
				if(X6 = '1' and (clr = '0' and ovf_acc = '0')) then
					real_result <= real_acc_tmp;
					imag_result <= imag_acc_tmp;
					ovf_result <= ovf_acc;
				end if;
			end if;
		end if;
	end process datapath;
	
	grafcet:process(clk, rst)
	begin
		if(rst = '1') then
			X0 <= '1'; X1 <= '0'; X2 <= '0'; X3 <= '0'; X4 <= '0'; X5 <= '0';
			X6 <= '0';
		elsif(clk'event and clk = '1') then
			if(X0 = '1') then
				X0 <= '0'; X1 <= '1'; X2 <= '1'; X3 <= '1'; X4 <= '1'; X5 <= '1'; X6 <= '1'; 
			end if;
		end if;
	end process grafcet;
	
end arch_MAC;