#!/bin/sh

DEVICE=/dev/sdg


usage() {
    echo "usage: $1 <bin-file>"
    echo "  - where <bin-file> is the output (program.bin) of:"
    echo "  arm-elf-objcopy -Obinary program.elf  program.bin"
    echo 
    echo
    echo "NB! The loader script (e.g. lpc2148_gcc.ld) shall contain this line: "
    echo "   FLASH (rx) : ORIGIN = 0x00002000, LENGTH = 0x00080000"
    echo "otherwise the USB boot loader won't work."
    echo    
    echo "Before running this script, press SW1, then SW3, release SW3 then SW1."
    echo "Then the board will be ready to receive the USB download."
    echo "Wait some time for the download to complete before resetting the "
    echo "board using SW3, which will start the downloaded application."
    echo
    echo "NB! Don't use an RS232 cable on UART0 having handshake signals; this"
    echo "will interfere with the reset signal."
    echo
    echo "The USB image burned into Blueboard is found in the blueboard project"
    echo "and is named USB_Boot.hex"
    echo
    echo See also http://wiki.openrobotino.org/index.php?title=Updating_the_firmware
    echo
    exit 1
}

if [ $# -ne 1 ]; then
    usage $0
fi

if [ ! -f $1 ]; then
    usage $0
fi

# Wait for the system mounting our device:
echo "Waiting for device $DEVICE to mount \c"
while [ `df -lk | grep "/media/NGX CRPDLBD" | wc -l` -eq 0 ]
do
    echo ".\c"
    sleep 1
done
echo
echo "Device $DEVICE is mounted."

df -lk | grep ^$DEVICE > /dev/null

# If the system has mounted the removable drive, unmount it: 
if [ $? -eq 0 ]; then
  umount $DEVICE
fi

echo "Emptying file system"
# Clear the device's file system: 
mdel -i $DEVICE ::*.*

echo "Copying firmware to device"
# Copy the new firmware to the device: 
# mcopy -i $DEVICE $1 ::firmware.bin
mcopy -i $DEVICE $1 ::ngx\ tech.bin

echo "Done."