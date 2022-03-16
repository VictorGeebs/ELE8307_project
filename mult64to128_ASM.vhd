library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
--use IEEE.std_logic_arith.all;
--use IEEE.std_logic_unsigned.all;

entity mult_4_49 is
port(
	-- custom instruction
	signal clk: in std_logic;							-- CPU system clock (always required)
	signal dummy_clk: in std_logic;						-- CPU system clock (always required)
	signal reset: in std_logic;							-- CPU master asynchronous active high reset (always required)
	signal dummy_reset: in std_logic;					-- CPU master asynchronous active high reset (always required)
	signal clk_en: in std_logic;						-- Clock-qualifier (always required)

	signal start: in std_logic;							-- Active high signal used to specify that inputs are valid (always required)
	signal done: out std_logic;							-- Active high signal used to notify the CPU that result is valid (required for variable multi-cycle)
	signal dataa: in std_logic_vector(31 downto 0);		-- Operand A (always required)
	signal datab: in std_logic_vector(31 downto 0);		-- Operand B (optional)
	signal result: out std_logic_vector(31 downto 0);	-- result (always required)

	-- Avalon Master
	signal addr : out std_logic_vector(31 downto 0);
	signal data : out std_logic_vector(7 downto 0);
	signal wr 	: out std_logic;
	signal busy : in std_logic

);
end entity mult_4_49;

architecture arch_ci of mult_4_49 is
	-- local custom instruction signals
	type state_t is (INIT, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12);
	signal state : state_t;

begin

	process( clk, reset )

	begin
		if( reset = '1' ) then
			state <= INIT;
			result <= (others => '0');
			done <= '0';
			wr  <= '0';
			state <= INIT;			
		elsif( rising_edge(clk) ) then	
			case state is 		
				when INIT => 
				    if (start = '0') then
					    done <= '0'; 
					    state <= INIT;
					else
					    done <= '0'; 
					    table <= dataa;
                        MAR <= dataa;	
                        wr <= '0';						
					    len <= datab; 
					    i <= 0;
						state <= INIT;
					end if;
                when S1 =>
                    if (i < len) then	
                        uL <= MDATA_OUT; 			-- load uL
						MAR <= table + 1; 
						i <= i + 1; 
						state <= S2;
					else
					    done <= '1';
						state <= INIT;
                    end if;
                when S2 =>
				    uH <= MDATA_OUT; 				-- load uH
					MAR <= table + 2;
					state <= S3;
                when S3 =>
				    vL <= MDATA_OUT; 				-- load vL
					MAR <= table + 3; 
					state <= S4;
                when S4 =>
				    vL <= MDATA_OUT;  			-- load vH
					state <= S5;
                when S5 =>
				    sign <= u(63) xor v(63);
				    if (u(63) = '1') then
					    u <= not(u) + 1;
					end if;
					if (v(63) = '1') then
					    v = not(v) + 1;
					end if;
					state <= S6;
				when S6 =>
				    z1 <= uL * vL; 
					z2 <= uH * vL; 
					z3 <= uL * vH; 
					z4 <= uH * vH; 
					state = S7;
				when S7 =>
				    tmp <= (z1 >> 32) + (z2 & 0xffffffff) + (z3 & 0xffffffff); 
					state = S8;
				when S8 =>
				    z(31 downto 0) <= (tmp << 32) | (z1 & 0xffffffff);
					z(63 downto 32) <= (tmp >> 32) + (z2 >> 32) + (z3 >> 32) + z4; 
					state <= S9;
				when S9 =>
				    z <= ((low >> 49) + (high << 15)) & ((low >> 49) + (high << 15));
					if (sign = '1') then
					    z <= not(z) + 1;
					end if;
					state <= S10;
				when S10 =>
				    MAR <= table; 
					MDATA_IN = z(31 downto 0); 
					wr <= '1';
					state <= S11;
				when S11 =>
				    MAR <= table; 
					MDATA_IN <= z(63 downto 32); 
					wr <= '1';
					state <= S11;
				when S12 =>
				    MAR <= table;
                    wr <= '0';
					state <= S1;
				when others =>
				    done <= '0';
					state <= INIT;
			end case;
		end if;
	end process;
end architecture arch_ci;		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		