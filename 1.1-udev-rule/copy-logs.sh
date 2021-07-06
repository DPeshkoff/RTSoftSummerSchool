#!/bin/sh
mkdir /media/usb-drive
mount /dev/sdc /media/usb-drive
cp -it /var/log/kern.log /media/usb-drive
cp -it /var/log/Xorg.x.log /media/usb-drive
umount /dev/sdc
rm -r /media/usb-drive
