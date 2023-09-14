<table class="sphinxhide">
 <tr>
   <td align="center"><img src="../../media/xilinx-logo.png" width="30%"/><h1> Kria&trade; KD240 Drive Starter Kit</h1>
   </td>
 </tr>
 <tr>
 <td align="center"><h1> Software Architecture </h1>

 </td>
 </tr>
</table>

# Software Architecture of the Platform

## Introduction

This section describes software components involved in the design and their
relation with each other. The application requires hw platform as described in
[Hardware Architecture of the Platform](./hw_description.md) for the stack
explained in this section to work.

The software stack here provides a comprehensive library that can be
interfaced using various user interfaces and efficiently driver the motor
through Kria Drive SOM board.

Following diagram illustrates the top-level architecture and arrangement of
various software components:

![Software Architecture Overview](media/sw_arch.jpg)]

- Kernel: Ubuntu Linux kernel
  - Drivers:
    - `xilinx_adc_hub`: IIO driver for ADC HUB
    - `hls_qei_axi`: IIO driver for QEI Sensor
    - `hsl_foc_periodic`: IIO driver for Sensor based field oriented controller.
    - `hls_pwm_gen`: IIO driver for PWM GEN
    - `hsl_svpwm_duty`: IIO driver for SVPWM
- Middleware
  - IIO Framework & libiio library
  - Generic UIO framework
- Application & library
  - Motor Control Library (includes UIO driver for the custom Motor Control IP)
  - Bokeh dashboard

## Kernel Drivers

All the kernel drivers developed for the platform hardware are IIO based and
adhere to IIO kernel framework. The IIO (Industrial I/O) subsystem covers many
sensor and ADC devices in the industry. It provides following features to fetch
and control various parameters:

- Divide stream and type of data into logical channels.
- Each channel can have specific attributes along with device attributes to
fine tune the behavior.
- Efficient data collection using buffers to get deterministic data with
timestamps and sync with other channels.
- Interrupt handling to provide user side event handling.

Following drivers are added to support this APP:

- ADC Hub (xilinx_adc_hub): Provides interface to monitor current, voltage and
related faults that occur during the operation of the motor.
- QEI Encoder (hls_qei_axi): External encoder to monitor the real time speed
and position of the motor.
- FOC (hsl_foc_periodic): Field oriented controller driver to support various
operational modes and state of the motor.
- PWM_GEN (hls_pwm_gen): PWM_GEN provides PWM signal generation for motor
control.
- SVPWM_GEN (hsl_svpwm_duty): SVPWM provides space vector PWM to calcualte
phase ratio for the motor.

There is also custom Motor control IP, which is responsible for the design
specific glue logic and driving the motor's Gate Driver. The driver for this
IP is a UIO based and the device will instantiate the uio device for this IP.
The userspace driver for this is implemented inside the motor
control library in the control block.

For more details of IIO subsystem refer to
[Analog Devices Wiki](https://wiki.analog.com/software/linux/docs/iio/iio).

For more details on kernel drivers for IIO refer to
[Kernel Documentation](https://static.lwn.net/kerneldoc/driver-api/iio/core.html).

## Middleware

### Userspace IIO Library
libIIO is the user space library to easily access the kernel IIO subsystem.
It also provides various methods and command line interfaces, packaged in
libiio-utils, to access the IIO devices. There are other python wrappers
around this library, but the motor control library uses native C/C++ library.

For example, to list all the channels in the FOC driver:
```
ubuntu@kria:~$ iio_attr -c
IIO context has 7 devices:
        iio:device0, ams: found 30 channels
        iio:device1, ina260-u14: found 6 channels
        iio:device2, hls_foc_periodic: found 11 channels
        iio:device3, hls_qei_axi: found 3 channels
        iio:device4, xilinx_adc_hub: found 9 channels
        iio:device5, hls_svpwm_duty: found 4 channels
        iio:device6, hls_pwm_gen: found 4 channels

ubuntu@kria:~$ iio_attr -c  hls_foc_periodic
dev 'hls_foc_periodic', channel 'current0', id 'Id' (input, index: 0, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'current1', id 'Iq' (input, index: 1, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'current2', id 'I_alpha' (input, index: 2, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'current3', id 'I_beta' (input, index: 3, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'current4', id 'I_homopolar' (input, index: 4, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'rot5', id 'speed_pi_out' (input, index: 5, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'rot6', id 'torque_pi_out' (input, index: 6, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'intensity7', id 'flux' (input, index: 7, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'rot8', id 'speed' (input, index: 8, format: le:U32/32>>0), found 3 channel-specific attributes
dev 'hls_foc_periodic', channel 'angl9' (input, index: 9, format: le:U32/32>>0), found 2 channel-specific attributes
dev 'hls_foc_periodic', channel 'timestamp' (input, index: 10, format: le:S64/64>>0), found 0 channel-specific attributes

```

Refer to the libIIO [Documentation](https://analogdevicesinc.github.io/libiio/v0.23/index.html) for more details.

### Userspace Access to UIO
The custom motor control IP is accessed from the motor control library using
the UIO interface. A device tree node with compatible string as `generic-uio`
is added to connect this hardware to the UIO subsystem.

The device is now accessible in the user space through `/dev/uioX` device.
Information about the device is obtained by scanning `/sys/class/uio/uioX`,
where `X` is the uio device id automatically assigned by the kernel.

For more information on the UIO interface refer to the kernel
[documentation](https://www.kernel.org/doc/html/v4.18/driver-api/uio-howto.html)


## Application & Library

The motor control library is a C++ library that provides APIs for the front end
application to control and fetch motor parameters. It also has a pybind11 wrapper
to support the library APIs through python front end. The front end GUI included
with this application is based on a python bokeh server, which can be accessed
in a web browser in the network.

### Motor Control Library

The library can be divided into 4 blocks:
- Communication
- State Management & Coordination
- Event/Fault Management
- Control

![Motor Control Library](media/sw_lib_interface.jpg)]

#### Communication
The external user interfacing, exposes the supported APIs through the C++ shared
library and Python module.

The library allows the user application to fetch following values:
- Speed
- Position
- Current (Phase A, Phase B, Phase C, DC Link)
- Voltage (Phase A, Phase B, Phase C, DC Link)
- Fault status
  - Over current for A, B, C, DC
  - Over & under voltage for DCLink
  - RMS power fault
  - Phase imbalance fault
- Intermediate FOC calculations (example: iq, id, i_alpha, etc)

The library allows the user application to set following motor controls:
- Clear faults
- Speed
- Torque
- Open loop parameters (vq & vd)
- Gain P/I gains (for current, speed and flux controller)

For details list of APIs refer to
[motor-control.hpp](https://github.com/xilinx/foc-motor-ctrl/blob/main/lib/include/motor-control/motor-control.hpp) fle.

Python binding for the application is provided using the pybind11 library.

#### State Management & Coordination
This is the central part of the library, which receives and processes all the
request from the communication block and is responsible for:
- Executing the initialization sequence for the motor.
- Maintaining the current state of the platform and handling the state
transitions.
- Splitting, merging and coordinating the request with various handlers in the
control block.

#### Event/Fault Management
The Event Manager is responsible for configuring and monitoring events / faults
from various hardware blocks. This block is aware of event capabilities of
various control blocks and configures the events accordingly.

It caches the event status in realtime and returns that to user when requested.
The event is monitored in a separate thread which waits on epoll events for the
registered events. Optionally, it also provides callback function registration
in case additional action is expected when a realtime event occurs.

#### Control
This block is responsible for hardware access. There are interface classes
which implement common interfaces for all the iio based control handlers and
all the uio based control handlers. It also provides abstraction for the sensor
interface to support more than one sensors (only one used at a given time).

Following are the various handlers for controlling respective hardware:
- FOC: For controlling and fetching data from the FOC hardware through libiio
framework
- ADCHub: For controlling and fetching data from the ADCHub hardware through
libiio framework.
- Sensor: Abstraction for controlling and fetching data from the Sensor
hardware through libiio framework (QEI in this case).
- PWM_Gen: For controlling the PWM_GEN hardware through libiio framework.
- SVPWM: For controlling the SVPWM hardware through libiio framework.
- MC_UIO: For controlling custom Motor Control IP hardware through UIO framework.
- SW_AvgPower: Software based average power calculator to generate fault
and control gate driver through MC_UIO.

#### Additional Threads
Following are the additional threads implemented in the library:

- Speed Ramp: When a new *speed* set point is set, the speed is ramped up or
down in FOC control handler using the constant ramp rate defined in default_config.h.
- Torque Ramp: When new *torque* set point is set, the speed is ramped up or
down in FOC control handler using the constant ramp rate defined in default_config.h.
- Event Monitor: The event manager waits on any event using epoll_wait in its
own thread.

### Bokeh Server (GUI)

A python based bokeh server is implemented to provide a GUI interface to the
application. It imports the motor control python library. The plot updates are
handled by an update function which is called at the interval specified in the
Refresh Interval text box. For each update, the function requests the number of
samples specified in the Sample Size text box and update the plots with new data.

For more details on the GUI usage, please refer the
[Application Deployment](./app_deployment.md) page for the details on the
components and its usage.

## Next Steps

* [Application Deployment](./app_deployment.md)
* Go back to the [KD240 FOC Motor Control landing page](../foc_motor_control_landing.rst)

### License

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.

You may obtain a copy of the License at
[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0).


Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

<p align="center">Copyright&copy; 2023 AMD</p>
