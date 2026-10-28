<!--
Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT
-->

# Building the Hardware Design and Firmware

## Overview

The motor control application runs on the prebuilt `kd240-motor-ctrl-qei` PL
firmware, so most users do not need to build anything - see
[Setting up the Board and Application Deployment](app_deployment.md). This page
is for users who want to rebuild the firmware or modify the hardware design.

The firmware is the prebuilt release artifact of the kria-vitis-platforms
release listed below. The meta-kria-apps `kd240-motor-ctrl-qei-fw` recipe
fetches this artifact unmodified.

For a description of the hardware design, see
[Hardware Architecture](hw_description.md).

## Release Information

The rest of this page does not name a release. Use the versions in this table.

| Item | Version |
|------|---------|
| AMD EDF release | 26.06 |
| AMD Vivado&trade; and AMD Vitis&trade; tools | 2026.1 |
| kria-vitis-platforms release | [v1.2](https://github.com/Xilinx/kria-vitis-platforms/releases/tag/v1.2) |
| meta-kria-apps firmware recipe | [rel-v2026.1](https://github.com/Xilinx/meta-kria-apps/tree/rel-v2026.1/recipes-firmware/kd240-motor-ctrl-qei-fw) |

## Prerequisites

* AMD Vivado&trade; Design Suite, with `bootgen` and `dtc` on the `PATH`
* AMD Vitis&trade;. The motor control HLS IP is synthesized with Vitis.
* A TSN Subsystem IP license, because the design includes the TSN IP. To
  request an evaluation license, email
  [1gtsn_eval_request@amd.com](mailto:1gtsn_eval_request@amd.com).

Use the tool version listed in [Release Information](#release-information).

## Getting the Sources

Set `KVP_TAG` to the kria-vitis-platforms release listed in
[Release Information](#release-information), then clone:

```bash
KVP_TAG=v1.2
git clone --branch ${KVP_TAG} --recursive https://github.com/Xilinx/kria-vitis-platforms.git
cd kria-vitis-platforms
```

The `--recursive` flag is required. The HLS IP is built from the
`common/Vitis_Libraries` submodule.

The design sources are in `kd240/platforms/kd240_motor_ctrl_qei`:

* `scripts/` - Tcl scripts that create the Vivado project and block design
* `dtsi/` - device tree overlay source for the firmware
* `ip/` - link to the shared `kd240/platforms/ip` directory, which holds the
  motor control IP (`qei`, `foc`, `svpwm_duty`, `pwm_gen`, and others)
* `xdc/` - constraints

## Building the Firmware

Source the Vivado and Vitis settings scripts, then run the following from the
repository root:

```bash
make firmware FW=kd240-motor-ctrl-qei
```

This first synthesizes the `qei`, `foc`, `svpwm_duty`, and `pwm_gen` HLS IP
with Vitis. It then builds the Vivado project and bitstream, converts the
bitstream to a `.bin` with `bootgen`, compiles the `.dtsi` into a `.dtbo`, and
packages them with `shell.json` into `kd240/firmware/kd240-motor-ctrl-qei.tar.gz`.
This is the same layout as the release artifact:

```text
kd240-motor-ctrl-qei/
+-- LICENSE
+-- checksum.md5
+-- lib/firmware/xilinx/kd240-motor-ctrl-qei/
    +-- kd240-motor-ctrl-qei.bin
    +-- kd240-motor-ctrl-qei.dtbo
    +-- shell.json
```

Use `JOBS=<n>` to change the number of synthesis jobs (default 8). Run
`make help` to list all valid `FW` values.

## Modifying the Hardware Design

1. Generate the Vivado project, which is also the first step of the firmware
   build:

    ```bash
    make -C kd240/platforms/kd240_motor_ctrl_qei xsa
    ```

2. Open `kd240/platforms/kd240_motor_ctrl_qei/project/kd240_motor_ctrl_qei.xpr`
   in Vivado, then edit and validate the block design.

3. If the change affects the PL address map or interrupts, update the overlay
   source in `dtsi/` to match.

4. In Vivado, run **Generate Bitstream**, then run
   `make firmware FW=kd240-motor-ctrl-qei`. The build does not rerun Vivado
   when `project/kd240_motor_ctrl_qei.xsa` already exists. It packages the
   bitstream at
   `project/kd240_motor_ctrl_qei.runs/impl_1/kd240_motor_ctrl_qei_wrapper.bit`.

> _**Note:**_ `make clean` deletes the `project/` directory, including any
> changes made in the Vivado GUI. To make a change permanent, update
> `scripts/config_bd.tcl`. `make clean` does not remove the built HLS IP. To
> rebuild it, run `make -C kd240/platforms/kd240_motor_ctrl_qei clean_ip`.

## Deploying Custom Firmware

Copy the contents of `lib/firmware/xilinx/kd240-motor-ctrl-qei/` from the
tarball to `/lib/firmware/xilinx/kd240-motor-ctrl-qei/` on the target, in place
of the step that installs firmware from the container. Then load the firmware
with `dfx-mgr-client` as described in
[Setting up the Board and Application Deployment](app_deployment.md).

## Next Steps

* Go back to the [KD240 FOC Motor Control Landing Page](../foc_motor_control_landing)

<p class="sphinxhide" align="center">Copyright&copy; 2026 Advanced Micro Devices, Inc</p>
