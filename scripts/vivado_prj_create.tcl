source ../scripts/621_bd_rev1_2.tcl
make_wrapper -files [get_files ./myproj/moverlab.srcs/sources_1/bd/pssub/pssub.bd] -top
add_files -norecurse ./myproj/moverlab.srcs/sources_1/bd/pssub/hdl/pssub_wrapper.v
import_files -fileset constrs_1 ../xdc/constraints.xdc
