library IEEE;

use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity filter_CSD is
	generic(data_width : integer := 8;
			filter_width : integer := 8);
	port(clk : in std_logic;
		 rst : in std_logic;
		 xin : in std_logic_vector(data_width - 1 downto 0);
		 yout: out std_logic_vector(data_width - 1 downto 0));
end filter_CSD;

architecture arch_filter_CSD of filter_CSD is
	signal h0 : std_logic_vector(filter_width downto 0) := "0" & "00000010";
	signal h1_1 : std_logic_vector(filter_width downto 0) := "0" & "00000001";
	signal h1_2 : std_logic_vector(filter_width downto 0) := "0" & "00000100";
	signal h2_1 : std_logic_vector(filter_width downto 0) := "0" & "00000100";
	signal h2_2 : std_logic_vector(filter_width downto 0) := "0" & "00000001";
	signal h3_1 : std_logic_vector(filter_width downto 0) := "0" & "00000100";
	signal h3_2 : std_logic_vector(filter_width downto 0) := "0" & "00000001";
	signal h4_1 : std_logic_vector(filter_width downto 0) := "0" & "00000010";
	signal h4_2 : std_logic_vector(filter_width downto 0) := "0" & "00010000";
	signal h5_1 : std_logic_vector(filter_width downto 0) := "0" & "01001000";
	signal h6 : std_logic_vector(filter_width downto 0) := "0" & "10010100";
	signal h7_1 : std_logic_vector(filter_width downto 0) := "0" & "01001000";
	signal h8_1 : std_logic_vector(filter_width downto 0) := "0" & "00000010";
	signal h8_2 : std_logic_vector(filter_width downto 0) := "0" & "00010000";
	signal h9_1 : std_logic_vector(filter_width downto 0) := "0" & "00000100";
	signal h9_2 : std_logic_vector(filter_width downto 0) := "0" & "00000001";
	signal h10_1 : std_logic_vector(filter_width downto 0) := "0" & "00000100";
	signal h10_2 : std_logic_vector(filter_width downto 0) := "0" & "00000001";
	signal h11_1 : std_logic_vector(filter_width downto 0) := "0" & "00000001";
	signal h11_2 : std_logic_vector(filter_width downto 0) := "0" & "00000100";
	signal h12 : std_logic_vector(filter_width downto 0) := "0" & "00000010";
	
	signal X0, X1, X2, X3 : std_logic;
	signal x_tmp0, x_tmp1, x_tmp2, x_tmp3, x_tmp4, x_tmp5, x_tmp6, x_tmp7
		 , x_tmp8, x_tmp9, x_tmp10, x_tmp11, x_tmp12: std_logic_vector(data_width - 1 downto 0);--7~0
	signal y_out_tmp : std_logic_vector(data_width + filter_width + 4 downto 0);--20~0
begin
	
	grafcet:process(clk, rst)
	begin
		if(rst = '1') then
			X0 <= '1'; X1 <= '0'; X2 <= '0'; X3 <= '0'; 
		elsif(clk'event and clk = '1') then
			if(X0 = '1') then
				X0 <= '0'; X1 <= '1'; X2 <= '1'; X3 <= '1';
			--elsif(X1 = '1' or X2 = '1' or X3 = '1') then	
			end if;
		end if;
	end process grafcet;
	
	datapath:process(clk)
	variable y_tmp0, y_tmp1, y_tmp2, y_tmp3, y_tmp4, y_tmp5, y_tmp6, y_tmp7
		 , y_tmp8, y_tmp9, y_tmp10, y_tmp11, y_tmp12: std_logic_vector(data_width + filter_width + 4 downto 0);--20~0
	begin
		if(clk'event and clk = '1') then
			if(X0 = '1') then
				x_tmp0 <= (others => '0');x_tmp1 <= (others => '0');x_tmp2 <= (others => '0');
				x_tmp3 <= (others => '0');x_tmp4 <= (others => '0');x_tmp5 <= (others => '0');
				x_tmp6 <= (others => '0');x_tmp7 <= (others => '0');x_tmp8 <= (others => '0');
				x_tmp9 <= (others => '0');x_tmp10 <= (others => '0');x_tmp11 <= (others => '0');
				x_tmp12 <= (others => '0');
				y_out_tmp <= (others => '0');
			else
				if(X1 = '1') then
				--8 bits
					x_tmp0 <= xin;
					x_tmp1 <= x_tmp0;
					x_tmp2 <= x_tmp1;
					x_tmp3 <= x_tmp2;
					x_tmp4 <= x_tmp3;
					x_tmp5 <= x_tmp4;
					x_tmp6 <= x_tmp5;
					x_tmp7 <= x_tmp6;
					x_tmp8 <= x_tmp7;
					x_tmp9 <= x_tmp8;
					x_tmp10 <= x_tmp9;
					x_tmp11 <= x_tmp10;
					x_tmp12 <= x_tmp11;
				end if;
				if(X2 = '1') then
				--X bits * 8 bits => 21 bits
					y_tmp0 := (signed(h0)) * (x_tmp0(7) & x_tmp0(7) & x_tmp0(7) & x_tmp0(7) & signed(x_tmp0));
					y_tmp1 := signed(h1_1) * (x_tmp1(7) & x_tmp1(7) & x_tmp1(7) & x_tmp1(7) & signed(x_tmp1))
							- signed(h1_2) * (x_tmp1(7) & x_tmp1(7) & x_tmp1(7) & x_tmp1(7) & signed(x_tmp1));
							
					y_tmp2 := signed(h2_1) * (x_tmp2(7) & x_tmp2(7) & x_tmp2(7) & x_tmp2(7) & signed(x_tmp2))
							- signed(h2_2) * (x_tmp2(7) & x_tmp2(7) & x_tmp2(7) & x_tmp2(7) & signed(x_tmp2));
					y_tmp3 := - signed(h3_1) * (x_tmp3(7) & x_tmp3(7) & x_tmp3(7) & x_tmp3(7) & signed(x_tmp3))
							- signed(h3_2) * (x_tmp3(7) & x_tmp3(7) & x_tmp3(7) & x_tmp3(7) & signed(x_tmp3));
					
					y_tmp4 := signed(h4_1) * (x_tmp4(7) & x_tmp4(7) & x_tmp4(7) & x_tmp4(7) & signed(x_tmp4))
							- signed(h4_2) * (x_tmp4(7) & x_tmp4(7) & x_tmp4(7) & x_tmp4(7) & signed(x_tmp4));
					y_tmp5 := signed(h5_1) * (x_tmp5(7) & x_tmp5(7) & x_tmp5(7) & x_tmp5(7) & signed(x_tmp5));
					y_tmp6 := signed(h6) * (x_tmp6(7) & x_tmp6(7) & x_tmp6(7) & x_tmp6(7) & signed(x_tmp6));
					y_tmp7 := signed(h7_1) * (x_tmp7(7) & x_tmp7(7) & x_tmp7(7) & x_tmp7(7) & signed(x_tmp7));
					y_tmp8 := signed(h8_1) * ((x_tmp8(7) & x_tmp8(7) & x_tmp8(7) & x_tmp8(7) & signed(x_tmp8)))
							- signed(h8_2) * (x_tmp8(7) & x_tmp8(7) & x_tmp8(7) & x_tmp8(7) & signed(x_tmp8));
					
					y_tmp9 := - signed(h9_1) * (x_tmp9(7) & x_tmp9(7) & x_tmp9(7) & x_tmp9(7) & signed(x_tmp9))
							- signed(h9_2) * (x_tmp9(7) & x_tmp9(7) & x_tmp9(7) & x_tmp9(7) & signed(x_tmp9));
					y_tmp10 := signed(h10_1) * (x_tmp10(7) & x_tmp10(7) & x_tmp10(7) & x_tmp10(7) & signed(x_tmp10))
							- signed(h10_2) * (x_tmp10(7) & x_tmp10(7) & x_tmp10(7) & x_tmp10(7) & signed(x_tmp10));
					
					y_tmp11 := signed(h11_1) * (x_tmp11(7) & x_tmp11(7) & x_tmp11(7) & x_tmp11(7) & signed(x_tmp11))
							- signed(h11_2) * ((x_tmp11(7) & x_tmp11(7) & x_tmp11(7) & x_tmp11(7) & signed(x_tmp11)));
					y_tmp12 := signed(h12) * (x_tmp12(7) & x_tmp12(7) & x_tmp12(7) & x_tmp12(7) & signed(x_tmp12));
				end if;
				if(X3 = '1') then
					y_out_tmp <= (((signed(y_tmp0) + signed(y_tmp1)) + (signed(y_tmp2) + signed(y_tmp3))) + ((signed(y_tmp4) + signed(y_tmp5)) +
								 (signed(y_tmp6) + signed(y_tmp7)))) + (((signed(y_tmp8) + signed(y_tmp9)) + (signed(y_tmp10) + signed(y_tmp11))) +
								 (signed(y_tmp12)));
				end if;
			end if;
		end if;
	end process datapath; 
	-- 8 + 8, 8 => 15,8
	yout <= y_out_tmp(15 downto 8);
end arch_filter_CSD;
