##################################################
FOC Motor Control Application
##################################################

*******************************
Overview
*******************************

.. toctree::
   :maxdepth: 1

   Introduction <src/introduction.md>

Features
================================

* Dashboard with customizable plots

  - Three control modes (speed, torque, and open loop)
  - Control loop parameter tuning
  - Buffer based data collection
  - Fault monitoring
  - Live analysis plots
  - Configurable sample window size
* IIO Linux kernel drivers for hardware IP
* AMD Vitis™ Motor Control Libraries based HLS IP
* C++ and Python library for using entire motor control stack

*******************************
Quick Start
*******************************

.. toctree::
   :maxdepth: 1

   Setting Up the Board and Application deployment <src/app_deployment.md>

***************************
Tutorials
***************************

.. toctree::
   :maxdepth: 1

   Building the Design Components <https://xilinx.github.io/kria-apps-docs/kd240/building_the_design.html>
   Building the Hardware Design Using Vivado <https://xilinx.github.io/kria-apps-docs/kd240/build_vivado_design.html>
   Generating Custom Firmware <https://xilinx.github.io/kria-apps-docs/kd240/generating_custom_firmware.html>
   Building the Application <https://github.com/Xilinx/foc-motor-ctrl/blob/main/README.md#build-instructions>

******************
Architecture
******************

.. toctree::
   :maxdepth: 1

   Hardware Architecture <src/hw_description.md>
   Software Architecture <src/sw_arch.md>

*******************************
Repository
*******************************

.. toctree::
  :maxdepth: 1

  Software Repository <https://github.com/Xilinx/foc-motor-ctrl>

*******************************
Other
*******************************

.. toctree::
  :maxdepth: 1

  Known Issues <src/known_issues.md>

..
   License
   Licensed under the Apache License, Version 2.0 (the "License"); you may not
   use this file except in compliance with the License.

   You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
   WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
   License for the specific language governing permissions and limitations under
   the License.
