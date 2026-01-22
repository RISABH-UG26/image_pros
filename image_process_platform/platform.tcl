# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/risabh/workspace/image_process_platform/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/risabh/workspace/image_process_platform/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {image_process_platform}\
-hw {/home/risabh/image_proscess/image_process.xsa}\
-proc {microblaze_0} -os {standalone} -out {/home/risabh/workspace}

platform write
platform generate -domains 
platform active {image_process_platform}
platform generate
