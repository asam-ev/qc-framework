<!---
Copyright 2023 CARIAD SE.
 
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# How-to Build and Install

The ASAM Quality Checker Framework runs on Linux and Windows. The framework consists of both C++ and Python components.

## Toolchain

- GCC 7.5 or newer compiler under Linux or Visual Studio 16 2019 or newer
  compiler under Windows
- CMake 3.16 or newer
- Python 3.10 or newer

## 3rd Party Dependencies for C++ Components

- Xerces-C++
- Qt 5
- **Optional:** GoogleTest (only if tests are enabled)

Links to download the sources and the tested versions can be found in the
[license information appendix](licenses/readme.md).

On Linux, toolchain and 3rd party dependencies can be installed as follows.

```bash
apt update && apt install -y \
    g++ \
    g++-10 \
    make \
    build-essential \
    cmake \
    libgtest-dev \
    qtbase5-dev \
    libqt5xmlpatterns5-dev \
    libxerces-c-dev \
    pkg-config \
    python3.10-venv \
    git
```

## Build C++ components

- Use CMakeLists.txt within the main directory as source directory
- Do not forget to set `CMAKE_INSTALL_PREFIX`
- Do not forget to set `CMAKE_BUILD_TYPE` if using CMake generator `Unix
  Makefiles`

For Linux, an example CMake call to build the framework
looks like this (call from the repository root):

```bash
cmake -G "Unix Makefiles" -B./build -S . \
    -DCMAKE_INSTALL_PREFIX="/home/$(whoami)/qc-build" \
    -DENABLE_FUNCTIONAL_TESTS=ON  -DXERCES_ROOT="/usr" \
    -DQt5_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5/" \
    -DQt5XmlPatterns_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5XmlPatterns/"
cmake --build ./build --target install --config Release -j4
cmake --install ./build
```

For Windows Visual Studio 16 2019 an example CMake call to build the framework
looks like this (call from the repository root):

```bash
$ mkdir ../build
$ cmake -G "Visual Studio 16 2019" -A "x64" -T "v142" -B../build -S. ^
    -DCMAKE_INSTALL_PREFIX="<prefix>" ^
    -DENABLE_FUNCTIONAL_TESTS=ON ^
    -DGTest_ROOT="<gtest_root>" ^ 
    -DASAM_OPENDRIVE_XSD_DIR="<asam_opendrive_xsd_dir>" ^
    -DASAM_OPENSCENARIO_XSD_DIR="<asam_openscenario_xsd_dir>" ^ 
    -DXERCES_ROOT="<xerces_c_root>" ^
    -DQt5_ROOT="<qt5_root>"
$ cmake --build ../build --target ALL_BUILD --config Release
$ ctest --test-dir ../build -C Release
$ cmake --install ../build
```

With the following CMake values:

- _\<prefix\>_: The prefix CMake installs the package to
- _\<GTest_ROOT\>_: The root dir of the pre-built GoogleTest package
- _\<asam_opendrive_xsd_dir\>_: The directory containing the schema (*.xsd)
  files for OpenDRIVE downloaded from the ASAM website (multiple versions of
  the schema files in this directory are supported).
- _\<asam_openscenario_xsd_dir\>_: The directory containing the schema (*.xsd)
  files for OpenSCENARIO downloaded from the ASAM website (multiple versions of
  the schema files in this directory are supported).
- _\<xerces_c_root\>_: The root dir of the pre-built Xerces-C++ package
- _\<Qt5_ROOT\>_: The root dir of the pre-built qt5 package

### Options

| Name | Value | Description | Remarks |
| ---- | ----- | ----------- | ------- |
| ENABLE_FUNCTIONAL_TESTS | ON/OFF | choose whether the tests were created in the building process of the libraries or not | dependency to a valid gtest package needed (see <https://github.com/google/googletest>) |

## Setup Virtual Environment for Python Components

```bash
python3 -m venv runtime-venv
source runtime-venv/bin/activate
python3 -m pip install --no-cache-dir -r /app/framework/runtime/requirements.txt
```

## Install ASAM Checker Libraries

### ASAM OpenDrive Checker Library

```bash
git clone https://github.com/asam-ev/qc-opendrive.git
python3 -m venv opendrive-venv
source opendrive-venv/bin/activate
python3 -m pip install --no-cache-dir -r qc-opendrive/requirements.txt
cp qc-opendrive/xodrBundle qc-build/bin/xodrBundle
```

### ASAM OpenScenario XML Checker Library

```bash
git clone https://github.com/asam-ev/qc-openscenarioxml.git
python3 -m venv openscenario-venv
source openscenario-venv/bin/activate
python3 -m pip install --no-cache-dir -r qc-openscenarioxml/requirements.txt
cp qc-openscenarioxml/xoscBundle qc-build/bin/xoscBundle
```

### ASAM OTX Checker Library

```bash
git clone https://github.com/asam-ev/qc-otx.git
python3 -m venv otx-venv
source otx-venv/bin/activate
python3 -m pip install --no-cache-dir -r qc-otx/requirements.txt
cp qc-otx/otxBundle qc-build/bin/otxBundle
```
