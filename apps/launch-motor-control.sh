#!/bin/bash
#
# Copyright (C) 2024-2026 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT
#

usage() {
    echo "Usage: $0 [--dashboard|--canopen-server]  (default: dashboard)"
    exit 1
}

case "${1:-}" in
    --dashboard)      MODE="dashboard" ;;
    --canopen-server) MODE="canopen-server" ;;
    "")               MODE="dashboard" ;;
    *)                usage ;;
esac

PROJ=foc-motor-ctrl
FW_NAME=kd240-motor-ctrl-qei

# Check if firmware is installed and loaded
echo "Checking firmware status..."
FW_ROW=$(dfx-mgr-client -listPackage | grep "${FW_NAME}")

if [ -z "$FW_ROW" ]; then
    echo "ERROR: Firmware '$FW_NAME' is not installed."
    echo "Please install the firmware before proceeding."
    exit 1
fi

# Extract slotLoc value (4th column) - if it's 0, firmware is loaded
SLOT_LOC=$(echo "$FW_ROW" | awk '{print $4}')
if [ "$SLOT_LOC" != "0" ]; then
    echo "ERROR: Firmware '$FW_NAME' is not loaded (slotLoc: $SLOT_LOC)."
    echo "Load it with: dfx-mgr-client -loadByName ${FW_NAME}"
    exit 1
fi

echo "Firmware $FW_NAME is loaded"

# Suppress high-speed mode warning
SUPPRESS_ERR="WARNING: High-speed mode not enabled"

if [ "$MODE" = "dashboard" ]; then
    DASHBOARD_PATH=/usr/share/foc-motor-ctrl/dashboard
    PORT=3006
    
    # Fetch board IP from ethernet interface (eth*/end*)
    # Ethernet interface is named eth0 on Ubuntu-based images and end0 on AMD EDF-based images
    IP_ADDR=$(ip -4 -o addr show | awk '$2 ~ /^(eth|end)/ {print $4}' | cut -d/ -f1 | head -n1)
    
    # If no IP is found, print a clear error and exit.
    if [ -z "$IP_ADDR" ]; then
        echo "ERROR: No IP address found on Ethernet interface."
	echo "Please check your Ethernet connection and IP assignment."
        exit 1
    fi

    cleanup() {
        killall bokeh >/dev/null 2>&1 || true
        fuser -k ${PORT}/tcp >/dev/null 2>&1 || true
    }
    trap 'echo "Stopping Bokeh..."; cleanup; exit 0' INT TERM HUP
    # Ensure no stale server/process is holding the dashboard port.
    cleanup

    # Run the Bokeh server with default configuration
    echo "To access the Application, open http://${IP_ADDR}:${PORT} in the host machine browser."
    echo "Starting Bokeh server"
    bokeh serve --port ${PORT} --show --allow-websocket-origin=${IP_ADDR}:${PORT} \
        ${DASHBOARD_PATH} 2> >(grep -v "$SUPPRESS_ERR" >&2)

elif [ "$MODE" = "canopen-server" ]; then
    PREFIX=/opt/xilinx/xlnx-app-kd240-${PROJ}
    SERVER=/usr/bin/fmc_canopen
    CAN_IF=can0
    SLAVE_ID=4
    EDS=/usr/share/foc-motor-ctrl/foc-mc.eds

    # Check if the CAN interface exists
    if [ ! -f /sys/class/net/${CAN_IF}/operstate ]; then
        echo "ERROR: CAN interface ${CAN_IF} not found!"
        exit 1
    fi

    # Check if the CAN interface is up
    if [ "$(cat /sys/class/net/${CAN_IF}/operstate)" = "down" ]; then
        echo "ERROR: CAN interface ${CAN_IF} is not up!"
        exit 1
    fi

    # Suppress high-speed mode warning
    SUPPRESS_ERR="WARNING: High-speed mode not enabled"
    
    # Run the Motor server
    ${SERVER} -i ${CAN_IF} -n ${SLAVE_ID} -e ${EDS} \
        2> >(grep -v "$SUPPRESS_ERR" >&2) &

    echo "Motor server started."
    echo "To kill the server run: killall fmc_canopen"
fi
