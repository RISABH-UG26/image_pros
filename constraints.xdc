# ============================================
# Constraints File for Image Processing SoC
# Target: Zynq-7020 (Generic / ZedBoard / PYNQ)
# ============================================

# ============================================
# FORCE BITSTREAM GENERATION
# (Ignore unconstrained pin errors for demo)
# ============================================

# ============================================
# Configuration Settings
# ============================================
set_property CFGBVS VCCO [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

# ============================================
# Clock Input (100 MHz) - Directly from board
# Uncomment and modify for your specific board
# ============================================
# ZedBoard: Y9
# PYNQ-Z2: H16
# set_property PACKAGE_PIN Y9 [get_ports diff_clock_rtl_clk_p]
# set_property IOSTANDARD LVCMOS33 [get_ports diff_clock_rtl_clk_p]

# ============================================
# Reset - Active Low
# ============================================
# set_property PACKAGE_PIN T18 [get_ports reset_rtl]
# set_property IOSTANDARD LVCMOS33 [get_ports reset_rtl]

# ============================================
# UART TX/RX (if needed for debug)
# ============================================
# set_property PACKAGE_PIN Y11 [get_ports uart_rtl_txd]
# set_property IOSTANDARD LVCMOS33 [get_ports uart_rtl_txd]
# set_property PACKAGE_PIN AA11 [get_ports uart_rtl_rxd]
# set_property IOSTANDARD LVCMOS33 [get_ports uart_rtl_rxd]


