# FOC Motor Control App
[![Docs](https://img.shields.io/badge/-Documention-blue)](https://xilinx.github.io/kria-apps-docs)
[![prebuilt](https://img.shields.io/badge/-Prebuilt_Images-blueviolet)](#prebuilt-images)
[![License](https://img.shields.io/badge/license-MIT-green)](./LICENSE)

This repository provides Field oriented control based motor control library and application interface to use it.
This library provides both C++ interface as well as Python interface.

## Build Instructions

1. Install Prerequisites:
```bash
sudo apt install python3-dev python3-pybind11 libiio-dev
```

2. Clone the repo into workspace
```bash
git clone https://github.com/Xilinx/foc-motor-ctrl.git
cd foc-motor-ctrl
```

3. Configure Build
```bash
mkdir -p build
cd build/
cmake -DBUILD_TEST=ON ..
```
**Project specific configuration options**
Options                | Possible Values | Default    | Description
-----------------------|-----------------|------------|-------------
BUILD_TEST             | ON, OFF         | OFF        | Choose to build & install test applications for the library.
BUILD_DASHBOARD_APP    | ON, OFF         | OFF        | Choose to build & install dashboard applications.
BUILD_CMDLINE_APP      | ON, OFF         | OFF        | Choose to build & install command line applications.
BUILD_CANOPEN_APP      | ON, OFF         | OFF        | Choose to build & install canopen  applications.

**Other useful cmake configurations**
Options                | Possible Values | Default    | Description
-----------------------|-----------------|------------|-------------
CMAKE_INSTALL_PREFIX   | install location|`/usr/local`| Provide custom install location.

> Provide the build options to cmake with -D\<option\>=\<val\>. For example `cmake -DCMAKE_INSTALL_PREFIX=~/install -DBUILD_TEST=ON -DBUILD_DASHBOARD_APP=ON ..`

4. Build the libraries, applications and tests.
```
make
```

5. Install the libraries, applications and tests
```
sudo make install
```

> Build and Install will be based on the build configuation above. `sudo` is not required for unprevilleged paths.

## Run test application

#### Export the library path
```bash
export LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}
export PYTHONPATH=/usr/local/lib:${PYTHONPATH}
```

#### Run the C++ test application
```
cpp_libtest
```

#### Run the python test application
```
py_libtest
```

## License

Copyright (C) 2023, Advanced Micro Devices, Inc.\
SPDX-License-Identifier: MIT

