<!---
Copyright 2023 CARIAD SE.
 
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# How-to Build and Install QC4OpenX

QC4OpenX runs on Linux and Windows.

## Toolchain

- GCC 7.5 or newer compiler under Linux or Visual Studio 16 2019 or newer compiler under Windows
- CMake 3.16 or newer (get it here: <https://cmake.org/download>)

## 3rd Party Dependencies

- dev_essential
- Xerces-C++
- Qt 5
- **Optional:** GoogleTest (only if tests are enabled)

Links to download the sources and the tested versions can be found in the
[license information appendix](licenses/readme.md).

## 3rd Party Data File Dependencies

- ASAM OpenDRIVE XML Schema files (*.xsd) (request download here:
  <https://www.asam.net/standards/detail/opendrive>).

  Currently supported versions are:
  - 1.1
  - 1.2
  - 1.3D
  - 1.4H
  - 1.5M
  - 1.6.1
  - 1.7.0

- ASAM OpenSCENARIO XML Schema files (*.xsd) (request download here:
  <https://www.asam.net/standards/detail/openscenario>).

  Currently supported versions are:
  - 0.9.1
  - 1.0.0
  - 1.1.0
  
The XSD files are required for the basic test of validating the formal correctness of a OpenDRIVE or
OpenSCENARIO file. All ASAM files are free of charge and require only a valid email address to
enable the download.

## Preparation

Before we can proceed to build and install QC4OpenX, we need to setup all 3rd party dependencies.
Download, build and install the above listed.

After the download of at least one ASAM OpenDRIVE and one ASAM OpenSCENARIO specification the XSD
files from the archives need to be placed in some arbitrary folders. Older versions have only a
single XSD file, whereas newer versions have multiple files. Nevertheless, extract all XSD files
into an empty folder. After a successful installation of QC4OpenX this folder can be deleted.

## Build QC4OpenX

- Use CMakeLists.txt within the main directory as source directory
- Do not forget to set `CMAKE_INSTALL_PREFIX`
- Do not forget to set `CMAKE_BUILD_TYPE` if using CMake generator `Unix Makefiles`

For Windows Visual Studio 16 2019 an example CMake call to build the QC4OpenX looks like this (call
from the repository root):

```bash
$ mkdir ../build
$ cmake -G "Visual Studio 16 2019" -A "x64" -T "v142" -B../build -S. ^
    -DCMAKE_INSTALL_PREFIX="<prefix>" ^
    -DENABLE_FUNCTIONAL_TESTS=ON ^
    -DGTest_ROOT="<gtest_root>" ^ 
    -DASAM_OPENDRIVE_XSD_DIR="<asam_opendrive_xsd_dir>" ^
    -DASAM_OPENSCENARIO_XSD_DIR="<asam_openscenario_xsd_dir>" ^ 
    -DXERCES_ROOT="<xerces_c_root>" ^
    -Ddev_essential_ROOT="<dev_essential_root>" ^
    -DQt5_ROOT="<qt5_root>"
$ cmake --build ../build --target ALL_BUILD --config Release
$ ctest --test-dir ../build -C Release
$ cmake --install ../build
```

With the following CMake values:

- _\<prefix\>_: The prefix CMake installs the package to
- _\<GTest_ROOT\>_: The root dir of the pre-built GoogleTest package
- _\<asam_opendrive_xsd_dir\>_: The directory containing the schema (*.xsd) files for OpenDRIVE
  downloaded from the ASAM website (multiple versions of the schema files in this directory are
  supported).
- _\<asam_openscenario_xsd_dir\>_: The directory containing the schema (*.xsd) files for
  OpenSCENARIO downloaded from the ASAM website (multiple versions of the schema files in this
  directory are supported).
- _\<xerces_c_root\>_: The root dir of the pre-built Xerces-C++ package
- _\<dev_essential_ROOT\>_: The root dir of the pre-built dev_essential package
- _\<Qt5_ROOT\>_: The root dir of the pre-built qt5 package

### Options

| Name | Value | Description | Remarks |
| ---- | ----- | ----------- | ------- |
| ENABLE_FUNCTIONAL_TESTS | ON/OFF | choose whether the tests were created in the building process of the libraries or not | dependency to a valid gtest package needed (see <https://github.com/google/googletest>) |
