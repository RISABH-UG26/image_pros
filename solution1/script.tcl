############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
############################################################
open_project image_pros
set_top image_pros
add_files image_pros/src/image_processing.h
add_files image_pros/src/image_processing.cpp
add_files -tb image_pros/src/testbench.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado
set_part {xc7z020-clg400-1}
create_clock -period 20 -name default
config_export -display_name Image_Processor -format ip_catalog -library hls -output /home/risabh/image_pros -rtl verilog -vendor xylinx.com -version 1.0
set_clock_uncertainty 0.5
source "./image_pros/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog -output /home/risabh/image_pros
