#!/bin/sh
echo "------Applying Tree overlay for pong-----"

dtc -O dtb -o PONG-GPIO-CFG-00A0.dtbo -b 0 -@ pong-gpio-cfg.dts
cp PONG-GPIO-CFG-00A0.dtbo /lib/firmware/
cd /lib/firmware/
export SLOTS=/sys/devices/bone_capemgr.8/slots
export PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins

echo "------Before-----"
cat $SLOTS
cat  $PINS|grep -iE "890|894|89c|834|828|83c|838"
echo PONG-GPIO-CFG > $SLOTS

echo "------After-----"
cat $SLOTS
cat  $PINS|grep -iE "890|894|89c|834|828|83c|838"

echo "------Done-----"
