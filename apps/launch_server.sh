#!/bin/bash
#
# Copyright (C) 2024 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT
#

PROJ=foc-motor-ctrl
FW_NAME=kd240-motor-ctrl-qei
PREFIX=/opt/xilinx/xlnx-app-kd240-${PROJ}
LIBPATH=${PREFIX}/lib
SERVER=${PREFIX}/bin/fmc_canopen
CAN_IF=can0
SLAVE_ID=4

if [ "$EUID" -ne 0 ]
	then
		echo "Please enter the password for sudo access"
		SUDO=sudo
		ENV="LD_LIBRARY_PATH=$LIBPATH"
		sudo -v
else
	SUDO=
	ENV=
	export LD_LIBRARY_PATH=$LIBPATH
fi

# check if the fw is loaded

FW_STATUS=$($SUDO xmutil listapps | grep ${FW_NAME} | awk '{print $6}' | cut -d',' -f1)

if [ "$FW_STATUS" -eq 0 ]; then
	echo "Firmware $FW_NAME is loaded"
else
	echo "fw $FW_NAME is not loaded !!"
	echo "Run following command to load the firmware first"
	echo "	sudo xmutil loadapp $FW_NAME"
	exit 1
fi

# check if the CAN interface is up
if [ ! -f /sys/class/net/${CAN_IF}/operstate ]; then
  echo "Error: Can interface ${CAN_IF} not found!"
  exit 1
fi

operstate=$(cat /sys/class/net/${CAN_IF}/operstate)

if [[ "$operstate" == "down" ]]; then
   echo "Error: Can interface - ${CAN_IF} is not up!"
   exit 1
fi

# Suppress high-speed mode warning
SUPPRESS_ERR="WARNING: High-speed mode not enabled"

# Run the Bokeh server
$SUDO ${ENV} \
        ${SERVER} -i ${CAN_IF} -n ${SLAVE_ID}  2> >(grep -v "$SUPPRESS_ERR" >&2) &

echo "To kill the server run 'sudo killall fmc_canopen'"
