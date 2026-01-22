# ============================================
# Image Processing Accelerator - HLS Build Script
# Run with: vitis_hls -f run_hls.tcl
# ============================================

# Create/Open Project
open_project image_pros

# Set Top Function
set_top image_pros

# Add Source Files
add_files src/image_processing.cpp
add_files src/image_processing.h

# Add Testbench Files
add_files -tb src/testbench.cpp

# Open Solution
open_solution "solution1" -flow_target vivado

# Target Device: Zynq-7020 (ZedBoard, PYNQ, etc.)
set_part {xc7z020clg400-1}

# Clock: 100 MHz (10ns period)
create_clock -period 10 -name default
set_clock_uncertainty 0.5

# ============================================
# Run C Simulation
# ============================================
puts "=========================================="
puts " Running C Simulation..."
puts "=========================================="
csim_design

# ============================================
# Run C Synthesis
# ============================================
puts "=========================================="
puts " Running C Synthesis..."
puts "=========================================="
csynth_design

# ============================================
# Run Co-Simulation (RTL Verification)
# ============================================
puts "=========================================="
puts " Running Co-Simulation..."
puts "=========================================="
cosim_design

# ============================================
# Export IP for Vivado
# ============================================
puts "=========================================="
puts " Exporting IP Catalog..."
puts "=========================================="
export_design -format ip_catalog -description "Image Processing Accelerator" -vendor "FPGA_Project" -version "1.0"

puts "=========================================="
puts " HLS Flow Complete!"
puts "=========================================="

exit
