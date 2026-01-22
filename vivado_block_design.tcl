# ============================================
# Vivado Block Design Script
# Image Processing Accelerator System
# Target: Zynq-7020
# ============================================
#
# INSTRUCTIONS:
# 1. First export IP from Vitis HLS (csynth_design + export_design)
# 2. Open Vivado, create new project
# 3. In Tcl Console: cd /home/risabh/image_pros
# 4. In Tcl Console: source vivado_block_design.tcl
# ============================================

# ============================================
# Project Settings
# ============================================
set project_name "image_proc_soc"
set project_dir "/home/risabh/image_pros/vivado_project"
set part_name "xc7z020clg400-1"

# Path to HLS exported IP
set hls_ip_path "/home/risabh/image_pros/solution1/impl/ip"

# ============================================
# Create Project
# ============================================
create_project $project_name $project_dir -part $part_name -force
set_property target_language Verilog [current_project]

# ============================================
# Add HLS IP Repository
# ============================================
puts "Adding HLS IP repository..."
set_property ip_repo_paths $hls_ip_path [current_project]
update_ip_catalog -rebuild

# ============================================
# Create Block Design
# ============================================
create_bd_design "system"

# ============================================
# Add MicroBlaze Processor
# ============================================
puts "Adding MicroBlaze processor..."
create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:11.0 microblaze_0

# Apply MicroBlaze block automation (creates local memory, debug, etc.)
apply_bd_automation -rule xilinx.com:bd_rule:microblaze \
    -config { \
        axi_intc {0} \
        axi_periph {Enabled} \
        cache {None} \
        clk {New Clocking Wizard} \
        cores {1} \
        debug_module {Debug Only} \
        ecc {None} \
        local_mem {64KB} \
        preset {None} \
    } [get_bd_cells microblaze_0]

# ============================================
# Add Image Processing IP (from HLS)
# ============================================
puts "Adding Image Processing IP..."
create_bd_cell -type ip -vlnv xilinx.com:hls:image_pros:1.0 image_pros_0

# ============================================
# Add AXI GPIO for Control
# ============================================
puts "Adding AXI GPIO..."
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0
set_property -dict [list \
    CONFIG.C_GPIO_WIDTH {8} \
    CONFIG.C_ALL_OUTPUTS {1} \
] [get_bd_cells axi_gpio_0]

# ============================================
# Add AXI BRAM Controller for Image Data
# ============================================
puts "Adding BRAM for image storage..."
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_0
create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_0

set_property -dict [list \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.use_bram_block {Stand_Alone} \
    CONFIG.Write_Width_A {32} \
    CONFIG.Read_Width_A {32} \
    CONFIG.Write_Depth_A {65536} \
] [get_bd_cells blk_mem_gen_0]

# ============================================
# Add UART for Debug Output
# ============================================
puts "Adding UART..."
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uartlite_0
set_property -dict [list \
    CONFIG.C_BAUDRATE {115200} \
] [get_bd_cells axi_uartlite_0]

# ============================================
# Connect IP to AXI Interconnect
# ============================================
puts "Running connection automation..."

# Connect Image Processing IP control interface
apply_bd_automation -rule xilinx.com:bd_rule:axi4 \
    -config { Clk_master {/clk_wiz_1/clk_out1} Clk_slave {Auto} Clk_xbar {Auto} \
              Master {/microblaze_0 (Periph)} Slave {/image_pros_0/s_axi_control} \
              ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}} \
    [get_bd_intf_pins image_pros_0/s_axi_control]

# Connect GPIO
apply_bd_automation -rule xilinx.com:bd_rule:axi4 \
    -config { Clk_master {/clk_wiz_1/clk_out1} Clk_slave {Auto} Clk_xbar {Auto} \
              Master {/microblaze_0 (Periph)} Slave {/axi_gpio_0/S_AXI} \
              ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}} \
    [get_bd_intf_pins axi_gpio_0/S_AXI]

# Connect BRAM Controller
apply_bd_automation -rule xilinx.com:bd_rule:axi4 \
    -config { Clk_master {/clk_wiz_1/clk_out1} Clk_slave {Auto} Clk_xbar {Auto} \
              Master {/microblaze_0 (Periph)} Slave {/axi_bram_ctrl_0/S_AXI} \
              ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}} \
    [get_bd_intf_pins axi_bram_ctrl_0/S_AXI]

# Connect UART
apply_bd_automation -rule xilinx.com:bd_rule:axi4 \
    -config { Clk_master {/clk_wiz_1/clk_out1} Clk_slave {Auto} Clk_xbar {Auto} \
              Master {/microblaze_0 (Periph)} Slave {/axi_uartlite_0/S_AXI} \
              ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}} \
    [get_bd_intf_pins axi_uartlite_0/S_AXI]

# Connect BRAM to BRAM Controller
connect_bd_intf_net [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA] \
                    [get_bd_intf_pins blk_mem_gen_0/BRAM_PORTA]

# ============================================
# Create External Ports
# ============================================
puts "Creating external ports..."

# UART pins
create_bd_port -dir I uart_rxd
create_bd_port -dir O uart_txd
connect_bd_net [get_bd_ports uart_rxd] [get_bd_pins axi_uartlite_0/rx]
connect_bd_net [get_bd_ports uart_txd] [get_bd_pins axi_uartlite_0/tx]

# GPIO output (optional - for LEDs or debug)
create_bd_port -dir O -from 7 -to 0 gpio_out
connect_bd_net [get_bd_ports gpio_out] [get_bd_pins axi_gpio_0/gpio_io_o]

# Reset button
apply_bd_automation -rule xilinx.com:bd_rule:board -config { Manual_Source {Auto}}  [get_bd_pins rst_clk_wiz_1_100M/ext_reset_in]
apply_bd_automation -rule xilinx.com:bd_rule:board -config { Manual_Source {Auto}}  [get_bd_pins clk_wiz_1/clk_in1]

# ============================================
# Validate and Save Design
# ============================================
puts "Validating design..."
regenerate_bd_layout
validate_bd_design
save_bd_design

# ============================================
# Generate Output Products
# ============================================
puts "Generating output products..."
generate_target all [get_files system.bd]

# ============================================
# Create HDL Wrapper
# ============================================
puts "Creating HDL wrapper..."
make_wrapper -files [get_files system.bd] -top
add_files -norecurse $project_dir/$project_name.srcs/sources_1/bd/system/hdl/system_wrapper.v

# ============================================
# Done!
# ============================================
puts ""
puts "=========================================="
puts " Block Design Created Successfully!"
puts "=========================================="
puts ""
puts " Next Steps:"
puts " 1. Review block design in Vivado"
puts " 2. Run Synthesis (Flow Navigator)"
puts " 3. Run Implementation"
puts " 4. Generate Bitstream"
puts " 5. Export Hardware: File -> Export -> Export Hardware (Include bitstream)"
puts " 6. Create Vitis Application Project"
puts "=========================================="
