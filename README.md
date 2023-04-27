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
3. Build and install the library
```bash
mkdir -p build
cd build/
cmake ..
make
sudo make install
```

> This will install the application in default installation prefix `/usr/local/`. To change the installation location add the location prefix to the cmake command above as `cmake -DCMAKE_INSTALL_PREFIX=<installation/location> ..`

4. Export the library path
```bash
export LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}
export PYTHONPATH=/usr/local/lib:${PYTHONPATH}
```

##Run test application

There are two test application, c++ application and python application for testing this library.
During buildi, if `-DBUILD_TEST=ON` is provided cmake, the cpp_libtest application is build in `<repo>/build/test`

```
#### Run the C++ test application
```
cpp_libtest
```

#### Run the python test application
```
python3 /usr/local/bin/py_libtest.py
```

## License

Copyright (C) 2023, Advanced Micro Devices, Inc.\
SPDX-License-Identifier: MIT
