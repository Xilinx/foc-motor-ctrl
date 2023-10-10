# FOC Motor Control App
[![Docs](https://img.shields.io/badge/-Documention-blue)](https://xilinx.github.io/kria-apps-docs)
[![prebuilt](https://img.shields.io/badge/-Prebuilt_Images-blueviolet)](#prebuilt-images)
[![License](https://img.shields.io/badge/license-MIT-green)](./LICENSE)

This repository provides a field oriented control based motor control library and application interface to use it. This library provides both C++ interface as well as Python interface.

## Build Instructions

1. Install build prerequisites:

    ```bash
    sudo apt install cmake python3-dev python3-pybind11 libiio-dev
    ```

2. Clone the repo into workspace:

    ```bash
    git clone https://github.com/Xilinx/foc-motor-ctrl.git
    cd foc-motor-ctrl
    ```

3. Configure the build:

    ```bash
    mkdir -p build
    cd build/
    cmake -DCMAKE_INSTALL_PREFIX=/opt/xilinx/xlnx-app-kd240-foc-motor-ctrl -DBUILD_DASHBOARD_APP=ON ..
    ```

    **Project specific configuration options**
    Options                | Possible Values | Default    | Description
    -----------------------|-----------------|------------|-------------
    BUILD_TEST             | ON, OFF         | OFF        | Choose to build and install test applications for the library.
    BUILD_DASHBOARD_APP    | ON, OFF         | OFF        | Choose to build and install dashboard applications.
    BUILD_CMDLINE_APP      | ON, OFF         | OFF        | Choose to build and install command line applications.
    BUILD_CANOPEN_APP      | ON, OFF         | OFF        | Choose to build and install canopen  applications.

    **Other useful cmake configurations**
    Options                | Possible Values | Default    | Description
    -----------------------|-----------------|------------|-------------
    CMAKE_INSTALL_PREFIX   | install location|`/usr/local`| Provide custom install location.

4. Build the libraries, applications and tests.

    ```
    make
    ```

5. Install the libraries, applications and tests.

    ```
    sudo make install
    ```

    > With above configurations, the libraries are installed in `/opt/xilinx/xlnx-app-kd240-foc-motor-ctr/lib`.

## Run the Application

### Install Prerequisites

```
# Install the firmware
sudo apt install xlnx-firmware-kd240-motor-ctrl-qei

# Install the app dependencies
sudo apt install libiio-utils libiio0 python3-pybind11 python3-bokeh=2.4.3-0ubuntu1
```

#### Run the Application

```
export PATH=${PATH}:/opt/xilinx/xlnx-app-kd240-foc-motor-ctr/bin
start_motor_dashboard
```

> The `start_motor_dashboard` script is renamed from `apps/launch_dashboard.sh`
and is designed to run with `/opt/xilinx/xlnx-app-kd240-foc-motor-ctr` as the default
prefix. The script sets the library path automatically and launches the bokeh
server with the IP address of the board. Update the script if different prefix
is being used.

## License

Copyright (C) 2023, Advanced Micro Devices, Inc.\
SPDX-License-Identifier: MIT
