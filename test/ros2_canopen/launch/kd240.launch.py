# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

import os
import sys

import launch
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python import get_package_share_directory
from launch import LaunchDescription


def generate_launch_description():
    ld = LaunchDescription()
    master_bin_path = os.path.join(
        get_package_share_directory("foc_motor"),
        "config",
        "motor_control",
        "master.bin",
    )
    if not os.path.exists(master_bin_path):
        master_bin_path = ""

    device_container = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [
                os.path.join(get_package_share_directory("canopen_core"), "launch"),
                "/canopen.launch.py",
            ]
        ),
        launch_arguments={
            "master_config": os.path.join(
                get_package_share_directory("foc_motor"),
                "config",
                "motor_control",
                "master.dcf",
            ),
            "master_bin": master_bin_path,
            "bus_config": os.path.join(
                get_package_share_directory("foc_motor"),
                "config",
                "motor_control",
                "bus.yml",
            ),
            "can_interface_name": "can0",
        }.items(),
    )

    ld.add_action(device_container)

    return ld
