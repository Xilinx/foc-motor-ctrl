# FOC Motor Control App
[![Docs](https://img.shields.io/badge/-Documention-blue)](https://xilinx.github.io/kria-apps-docs)
[![License](https://img.shields.io/badge/license-MIT-green)](./LICENSE)

This repository provides a field oriented control based motor control library and application interface to use it. This library provides both C++ interface as well as Python interface.

## Build Instructions

1. Install build prerequisites:

- IIO library and python binding
    ```bash
    sudo apt install cmake python3-dev python3-pybind11 libiio-dev
    ```
- Lely core libraries
    ```bash
    sudo add-apt-repository ppa:lely/ppa
    sudo apt-get update

    sudo apt-get install liblely-coapp-dev liblely-co-tools python3-dcf-tools
    sudo apt-get install pkg-config
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
    cmake -DCMAKE_INSTALL_PREFIX=/opt/xilinx/xlnx-app-kd240-foc-motor-ctrl -DBUILD_DASHBOARD_APP=ON -DBUILD_CANOPEN_APP=ON ..
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
    SKIP_POST_INSTALL      | ON, OFF         | OFF        | Skip the post install that can be handled by packaging system.

4. Build the libraries, applications and tests.

    ```
    make
    ```

5. Install the libraries, applications and tests.

    ```
    sudo make install
    ```

    > With above configurations, the libraries are installed in `/opt/xilinx/xlnx-app-kd240-foc-motor-ctrl/lib`.

## Run the Application

For detailed instructions on how to deploy and run the application, please refer to the [FOC Motor Control Application Deployment Page](https://xilinx.github.io/kria-apps-docs/kd240/foc-motor-ctrl/0_5/build/html/docs/app_deployment.html).

## License

Copyright (C) 2023-2025, Advanced Micro Devices, Inc.\
SPDX-License-Identifier: MIT
