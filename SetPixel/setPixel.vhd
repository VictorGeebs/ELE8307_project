--Copyright (C) 1991-2004 Altera Corporation
--Any megafunction design, and related net list (encrypted or decrypted),
--support information, device programming or simulation file, and any other
--associated documentation or information provided by Altera or a partner
--under Altera's Megafunction Partnership Program may be used only to
--program PLD devices (but not masked PLD devices) from Altera.  Any other
--use of such megafunction design, net list, support information, device
--programming or simulation file, or any other related documentation or
--information is prohibited for any other purpose, including, but not
--limited to modification, reverse engineering, de-compiling, or use with
--any other silicon devices, unless such use is explicitly licensed under
--a separate agreement with Altera or a megafunction partner.  Title to
--the intellectual property, including patents, copyrights, trademarks,
--trade secrets, or maskworks, embodied in any such megafunction design,
--net list, support information, device programming or simulation file, or
--any other related documentation or information provided by Altera or a
--megafunction partner, remains with Altera, the megafunction partner, or
--their respective licensors.  No other licenses, including any licenses
--needed under any third party's intellectual property, are provided herein.
--Copying or modifying any file, or portion thereof, to which this notice
--is attached violates this copyright.


-- VHDL Custom Instruction Template File for Multi-Cycle Logic

library ieee;
use ieee.std_logic_1164.all;
-- use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;
use ieee.numeric_std.all;


entity setPixel is
port(
	-- custom instruction
	signal clk: in std_logic;							-- CPU system clock (always required)
	signal dummy_clk: in std_logic;							-- CPU system clock (always required)
	signal reset: in std_logic;							-- CPU master asynchronous active high reset (always required)
	signal dummy_reset: in std_logic;							-- CPU master asynchronous active high reset (always required)
	signal clk_en: in std_logic;						-- Clock-qualifier (always required)
		
	-- signal N: in std_logic_vector(1 downto 0);			-- Selects which version of the instruction is required
	signal N: in std_logic_vector(0 downto 0);			-- Selects which version of the instruction is required
	signal start: in std_logic;							-- Active high signal used to specify that inputs are valid (always required)
	signal done: out std_logic;							-- Active high signal used to notify the CPU that result is valid (required for variable multi-cycle)
	signal dataa: in std_logic_vector(31 downto 0);		-- Operand A (always required)
	signal datab: in std_logic_vector(31 downto 0);		-- Operand B (optional)
	signal result: out std_logic_vector(31 downto 0);	-- result (always required)

	-- Avalon Master
	signal addr : out std_logic_vector(31 downto 0);
    signal readdata : in std_logic_vector(7 downto 0);
	signal rd	: out std_logic;
	signal writedata : out std_logic_vector(7 downto 0);
	signal wr 	: out std_logic;
	signal busy : in std_logic

);
end entity setPixel;

architecture a_custominstruction of setPixel is
	-- local custom instruction signals
	type t_etat is (init, s1,s2, init_busy, s1_busy, s2_busy); --Vous pouvez rajouter des etats ici.
	signal etat : t_etat;
	signal color_addr_reg : std_logic_vector(31 downto 0);
	signal vga_addr_reg : std_logic_vector(31 downto 0);
	
	shared variable color_value : unsigned(7 downto 0);
    shared variable length : unsigned(31 downto 0);
    shared variable count : unsigned(31 downto 0);

begin
	process( clk, reset )
	begin
		if( reset = '1' ) then
			etat <= init;
			done <= '0';
			wr <= '0';
            rd <= '0';
			
		elsif( rising_edge(clk) ) then						
			case etat is 
                when init =>
                    done <= '0';    
                    if (start = '1') then
                        color_addr_reg <= dataa;
                        vga_addr_reg <= datab;
                        length := 640; -- Change value to change vector length
                        count := 0;
                        
                        addr <= dataa; --std_logic_vector(color_addr_reg)
                        wr <= '0';
                        rd <= '1';                        
                        
                        -- Change busy to save clock cycles
                        if (busy = '0') then
                            rd <= '0';
                            etat <= s1;
                        else
                            etat <= init_busy;
                        end if;
                    end if;
                
                when init_busy =>
                    done <= '0';
                    if (busy = '0') then
                        rd <= '0';
                        etat <= s1;
                    end if;    
                
				when s1 =>
					done <= '0';
                    if (count < length) then
                        
                        addr <= vga_addr_reg;
                        wr <= '1';
                        rd <= '0';
                        
                        count := count + 1;
                        vga_addr_reg <= vga_addr_reg + 1;
                        color_addr_reg <= color_addr_reg + 1;
                        
                        etat <= s1_busy;
                    else
                        valid <= '1';
                        wr <= '0';
                        rd <= '0';
                        etat <= init;
                    end if;
                
                when s1_busy =>
                    done <= '0';
                    if (busy = '0') then
                        wr <= '0';
                        etat <= s2;
                    end if;   
					
				when s2 =>
                    done <= '0';
                    addr <= color_addr_reg;
                    wr <= '0';
                    rd <= '1';
                    
                    etat <= s2_busy;
                
                when s2_busy =>
                    done <= '0';
                    if (busy = '0') then
                        rd <= '0';
                        etat <= s1;
                    end if; 
			end case;
			
		end if;		
	end process;	

end architecture a_custominstruction;