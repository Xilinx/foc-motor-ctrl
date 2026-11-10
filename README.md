# FOC Motor Control App
[![Docs](https://img.shields.io/badge/-Documention-blue)](https://xilinx.github.io/kria-apps-docs)
[![License](https://img.shields.io/badge/license-MIT-green)](./LICENSE)

This repository provides a field oriented control based motor control library and application interface to use it. This library provides both C++ interface as well as Python interface.

## How it is delivered

Starting with the **v0.6 release**, you do not have to build this repository
yourself to run it on a board. The libraries, the Dashboard Application and the
ROS 2 CANopen Application are built from the source tree by a Yocto recipe and
packaged into prebuilt **OCI (Docker) images**, published on Docker Hub. All
application dependencies and validated software components are bundled in the
image, so you no longer download and install the individual application
artifacts separately.

Two images are published for the application:

| Board | Docker image |
| ----- | ------------ |
| KD240 (motor server / dashboard) | `xilinx/kria-foc-motor-ctrl-server:0.6-edf-26.06` |
| KR260 (ROS 2 CANopen client)     | `xilinx/kria-foc-motor-ctrl-ros2-canopen:0.6-edf-26.06` |

On each board, pull its image:

```bash
# Pull the foc-motor-ctrl-server image on KD240
docker pull xilinx/kria-foc-motor-ctrl-server:0.6-edf-26.06

# Pull the foc-motor-ctrl-ros2-canopen image on KR260
docker pull xilinx/kria-foc-motor-ctrl-ros2-canopen:0.6-edf-26.06
```

* To run the application on the boards (firmware install,
  dashboard and CANopen flows), see the
  [FOC Motor Control Application Deployment Page](https://xilinx.github.io/kria-apps-docs/kd240/foc-motor-ctrl/0_6/build/html/docs/app_deployment.html).
* To **build the app from source**, use either the native build in
  [FOC Motor Control App Build Instructions](#foc-motor-control-app-build-instructions) or the
  [Yocto recipe](#building-with-the-yocto-recipe) described below.

## FOC Motor Control App Build Instructions

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
    BUILD_CANOPEN_APP      | ON, OFF         | OFF        | Choose to build and install canopen applications.

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

## Building with the Yocto recipe

Both images are built with Yocto from the
[meta-kria-apps](https://github.com/Xilinx/meta-kria-apps) layer. The native
build above covers the FOC Motor Control App only; the
steps below build the full OCI (Docker) images for each board.

1. Set up an AMD EDF build environment by following the
   [Yocto Project build setup instructions for EDF](https://edf.docs.amd.com/en/latest/osdev/operating-system-integration-and-development.html#yocto-project-build-setup-instructions-for-edf).
2. Add the meta-kria-apps layer:

   ```
   bitbake-layers add-layer /path/to/meta-kria-apps
   ```

3. Build the container image for your board:

   - KD240 motor control server and dashboard
     ([container-foc-motor-ctrl-server.bb](https://github.com/Xilinx/meta-kria-apps/blob/rel-v2026.1/recipes-containers/container-foc-motor-ctrl/container-foc-motor-ctrl-server.bb)).
     This pulls in the
     [foc-motor-ctrl](https://github.com/Xilinx/meta-kria-apps/blob/rel-v2026.1/recipes-apps/foc-motor-ctrl/foc-motor-ctrl.bb)
     recipe, which builds the library, the motor server and the dashboard
     from this repository.

     ```
     MACHINE=amd-cortexa53-mali-common bitbake container-foc-motor-ctrl-server
     ```

   - KR260 ROS 2 CANopen client
     ([container-foc-motor-ctrl-ros2-canopen.bb](https://github.com/Xilinx/meta-kria-apps/blob/rel-v2026.1/recipes-containers/container-foc-motor-ctrl/container-foc-motor-ctrl-ros2-canopen.bb)).
     This pulls in the
     [kria-motor-control](https://github.com/Xilinx/meta-kria-apps/blob/rel-v2026.1/recipes-apps/foc-motor-ctrl/kria-motor-control.bb)
     recipe, which builds the ROS 2 CANopen client from this repository.

     ```
     MACHINE=amd-cortexa53-mali-common bitbake container-foc-motor-ctrl-ros2-canopen
     ```

The resulting OCI images are written under `tmp/deploy/images/${MACHINE}/`.

### Bundled components

The container recipes are the source of truth for what each image bundles and
which revision each component is built from.

**KD240 server image** (`container-foc-motor-ctrl-server`):

| Component | Built by recipe | Source |
| --------- | --------------- | ------ |
| FOC library, motor server, dashboard | [recipes-apps/foc-motor-ctrl](https://github.com/Xilinx/meta-kria-apps/tree/rel-v2026.1/recipes-apps/foc-motor-ctrl) (`foc-motor-ctrl`) | [Xilinx/foc-motor-ctrl](https://github.com/Xilinx/foc-motor-ctrl) |
| Motor control IIO kernel drivers | [recipes-kernel/motor-control-iio-modules](https://github.com/Xilinx/meta-kria-apps/tree/rel-v2026.1/recipes-kernel/motor-control-iio-modules) | [Xilinx/motor-control-iio-modules](https://github.com/Xilinx/motor-control-iio-modules) |
| KD240 firmware (`kd240-motor-ctrl-qei`) | [recipes-firmware/kd240-motor-ctrl-qei-fw](https://github.com/Xilinx/meta-kria-apps/tree/rel-v2026.1/recipes-firmware/kd240-motor-ctrl-qei-fw) | [Xilinx/kria-vitis-platforms](https://github.com/Xilinx/kria-vitis-platforms) |

**KR260 client image** (`container-foc-motor-ctrl-ros2-canopen`):

| Component | Built by recipe | Source |
| --------- | --------------- | ------ |
| ROS 2 CANopen client (`kria_motor_control`) | [recipes-apps/foc-motor-ctrl](https://github.com/Xilinx/meta-kria-apps/tree/rel-v2026.1/recipes-apps/foc-motor-ctrl) (`kria-motor-control`) | [Xilinx/foc-motor-ctrl](https://github.com/Xilinx/foc-motor-ctrl) |
| KR260 firmware (`kr260-tsn-rs485pmod`) | [recipes-firmware/kr260-tsn-rs485pmod-fw](https://github.com/Xilinx/meta-kria-apps/tree/rel-v2026.1/recipes-firmware/kr260-tsn-rs485pmod-fw) | [Xilinx/kria-vitis-platforms](https://github.com/Xilinx/kria-vitis-platforms) |

## Run the Application

For detailed instructions on how to deploy and run the application, please refer to the [FOC Motor Control Application Deployment Page](https://xilinx.github.io/kria-apps-docs/kd240/foc-motor-ctrl/0_6/build/html/docs/app_deployment.html).

## License

Copyright (C) 2023-2026, Advanced Micro Devices, Inc.\
SPDX-License-Identifier: MIT
