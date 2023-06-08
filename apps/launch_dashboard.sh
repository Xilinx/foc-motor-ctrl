#!/bin/bash
#
# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT
#

# Fetch the board IP:
IP_ADDR=$(ip -4 addr show eth0 | grep -oE "inet ([0-9]{1,3}[\.]){3}[0-9]{1,3}" | cut -d ' ' -f2)
LIBPATH=/usr/local/lib
DASHBOARD_PATH=/opt/xilinx/motor-control/dashboard

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

# Run the Bokeh server
$SUDO ${ENV} \
        bokeh serve --show --allow-websocket-origin=${IP_ADDR}:5006 --num-procs 0 \
        ${DASHBOARD_PATH} &

# Print the information for the host machine.
echo "To the access the Application, enter \"$IP_ADDR:5006\" in the host machine's browser."
