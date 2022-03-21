library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
--use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity mult_4_49 is
port(
    -- custom instruction
    signal clk: in std_logic;                           -- CPU system clock (always required)
    signal dummy_clk: in std_logic;                     -- CPU system clock (always required)
    signal reset: in std_logic;                         -- CPU master asynchronous active high reset (always required)
    signal dummy_reset: in std_logic;                   -- CPU master asynchronous active high reset (always required)
    signal clk_en: in std_logic;                        -- Clock-qualifier (always required)

    signal start: in std_logic;                         -- Active high signal used to specify that inputs are valid (always required)
    signal done: out std_logic;                         -- Active high signal used to notify the CPU that result is valid (required for variable multi-cycle)
    signal dataa: in std_logic_vector(31 downto 0);     -- Operand A (always required)
    signal datab: in std_logic_vector(31 downto 0);     -- Operand B (optional)
    signal result: out std_logic_vector(31 downto 0);   -- result (always required)

    -- Avalon Master
    signal addr  : out std_logic_vector(31 downto 0);
    signal rdata : in std_logic_vector(31 downto 0);
    signal wdata : out std_logic_vector(31 downto 0);
    signal rd    : out std_logic;
    signal wr    : out std_logic;
    signal busy  : in std_logic

);
end entity mult_4_49;
        
architecture arch_ci_opt of mult_4_49 is
    -- local custom instruction signals
    type state_t is (INIT, S1, S2, S3, S4, S5, S6, S7, S8);
    signal state : state_t;

    signal u : std_logic_vector(63 downto 0);
    signal v : std_logic_vector(63 downto 0);
    signal z : std_logic_vector(107 downto 0);
    
    signal table : std_logic_vector(31 downto 0);
    signal len : unsigned(16 downto 0);
    signal i : unsigned(16 downto 0);
    
    signal MAR : std_logic_vector(31 downto 0);
    signal MDATA_IN : std_logic_vector(31 downto 0);
    signal MDATA_OUT : std_logic_vector(31 downto 0);
    
begin

    addr <= MAR;
    MDATA_OUT <= rdata;
    wdata <= MDATA_IN;

    process( clk, reset )

    begin
        wr <= '0';
        rd <= '0';

        if( reset = '1' ) then
            state <= INIT;
            result <= (others => '0');
            done <= '0';
            rd <= '0';
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
                        rd <= '1';   
                        wr <= '0';                      
                        len <= unsigned(datab); 
                        i <= (others => '0');
                        state <= S1;
                    end if;
                when S1 =>
                    if (i < len) then   
                        u(31 downto 0) <= MDATA_OUT;        -- load uL
                        MAR <= table + 1;
                        rd <= '1';
                        i <= i + 1; 
                        state <= S2;
                    else
                        done <= '1';
                        state <= INIT;
                    end if;
                when S2 =>
                    u(63 downto 32) <= MDATA_OUT;           -- load uH
                    MAR <= table + 2;
                    rd <= '1';
                    state <= S3;
                when S3 =>
                    v(31 downto 0) <= MDATA_OUT;            -- load vL
                    MAR <= table + 3;
                    rd <= '0'; 
                    state <= S4;
                when S4 =>
                    v(63 downto 32) <= MDATA_OUT;           -- load vH
                    state <= S5;
                when S5 =>
                    z(107 downto 0) <= u(53 downto 0) * v(53 downto 0);
                    state <= S6;
                when S6 =>
                    MAR <= table; 
                    MDATA_IN <= z(80 downto 49); 
                    wr <= '1';
                    state <= S7;
                when S7 =>
                    MAR <= table; 
                    MDATA_IN(21 downto 0) <= z(102 downto 81);
                    MDATA_IN(31 downto 22) <= (31 downto 22 => z(102));                     
                    wr <= '1';
                    state <= S8;
                when S8 =>
                    MAR <= table;
                    wr <= '0';
                    state <= S1;
                when others =>
                    done <= '0';
                    state <= INIT;
            end case;
        end if;
    end process;
end architecture arch_ci_opt;       
        
        
       
        
        
        
        
        
        
        
        
        
        
        
        
        