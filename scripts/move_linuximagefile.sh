rm -rf ./sw_files/
mkdir ./sw_files
mkdir ./sw_files/image
mkdir ./sw_files/boot

cp ./peta/petamover/images/linux/image.ub ./sw_files/image

cp ./peta/petamover/images/linux/u-boot.elf ./sw_files/boot/
cp ./peta/petamover/images/linux/pmufw.elf ./sw_files/boot/
cp ./peta/petamover/images/linux/bl31.elf ./sw_files/boot/
cp ./peta/petamover/images/linux/zynqmp_fsbl.elf ./sw_files/boot/

cp ./bif/*.bif ./sw_files/boot

