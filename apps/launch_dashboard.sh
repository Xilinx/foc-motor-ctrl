#!/bin/bash
#
# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT
#

PROJ=foc-motor-ctrl
FW_NAME=kd240-motor-ctrl-qei
PREFIX=/opt/xilinx/xlnx-app-kd240-${PROJ}
LIBPATH=${PREFIX}/lib
DASHBOARD_PATH=${PREFIX}/share/${PROJ}/dashboard

# Fetch the board IP:
IP_ADDR=$(ip -4 addr show eth0 | grep -oE "inet ([0-9]{1,3}[\.]){3}[0-9]{1,3}" | cut -d ' ' -f2)

if [ "$EUID" -ne 0 ]
	then
		echo "Please enter the password for sudo access"
		SUDO=sudo
		ENV="PYTHONPATH=$LIBPATH LD_LIBRARY_PATH=$LIBPATH"
		sudo -v
else
	SUDO=
	ENV=
	export PYTHONPATH=$LIBPATH
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

# Suppress high-speed mode warning
SUPPRESS_ERR="WARNING: High-speed mode not enabled"

# Run the Bokeh server
$SUDO ${ENV} \
        bokeh serve --show --allow-websocket-origin=${IP_ADDR}:5006 \
        ${DASHBOARD_PATH} 2> >(grep -v "$SUPPRESS_ERR" >&2) &

# Print the information for the host machine.
echo "To the access the Application, enter \"$IP_ADDR:5006\" in the host machine's browser."
