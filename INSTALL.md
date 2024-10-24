<!---
Copyright 2023 CARIAD SE.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# How-to Build and Install

The ASAM Quality Checker Framework runs on Linux and Windows. The framework consists of both C++ and Python modules.

## Toolchain

- GCC 7.5 or newer compiler under Linux or Visual Studio 16 2019 or newer
  compiler under Windows
- CMake 3.16 or newer
- Python 3.10 or newer (otherwise, Conda can be used to install Python 3.10 or newer)

## 3rd Party Dependencies for C++ Modules

- Xerces-C++
- Qt 5
- **Optional:** GoogleTest (only if tests are enabled)

Links to download the sources and the tested versions can be found in the
[license information appendix](licenses/readme.md).

## Build and install C++ modules

For some operating systems, prebuilt binaries are available from the [release page](https://github.com/asam-ev/qc-framework/releases). If you cannot use the prebuilt binaries, follow the instructions below to build and install the C++ modules from source.

- Use CMakeLists.txt within the main directory as source directory
- Do not forget to set `CMAKE_INSTALL_PREFIX`
- Do not forget to set `CMAKE_BUILD_TYPE` if using CMake generator `Unix
  Makefiles`

### Build on Linux

On Linux, toolchain and 3rd party dependencies can be installed as follows (example for Ubuntu 22.04).

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

An example CMake call to build the framework
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

**Prebuilt binaries**

Prebuilt binaries for Ubuntu OS are available on the [release page](https://github.com/asam-ev/qc-framework/releases) in the Assets section, with the name `qc-framework-executables-linux_x64.zip`. To use the prebuilt binaries, you need to install few libraries.

```bash
apt-get update && apt-get install -y libxerces-c-dev qtbase5-dev libqt5xmlpatterns5-dev
```

Binaries are tested on Ubuntu 20.04 and Ubuntu 22.04


### Build on Windows

#### Prerequisites

##### Install latest CMake

https://www.google.com/search?client=firefox-b-d&q=cmake

##### Install visual studio build tools

From https://visualstudio.microsoft.com/it/downloads/?q=build+tools

Based on the version downloaded at this step, you can use your build tools version in the commands below

E.g. from `-G "Visual Studio 17 2022"` to `-G "Visual Studio 16 2019"`

#### Install dependancies


##### Set environment variable

```
$env:WORKING_PATH="C:\Users\USER_WORKING_PATH"
```

##### Install gtest

```
Write-Output "Installing gtest..."

git clone https://github.com/google/googletest.git -b release-1.10.0
cd googletest
mkdir build
cd build
cmake -G "Visual Studio 17 2022" `
-Dgtest_force_shared_crt=ON `
-DCMAKE_INSTALL_PREFIX="$env:WORKING_PATH\gtest-Out" ..
cmake --build . --config Release
cmake --build . --config Release --target install

Write-Output "Gtest installed."
```


##### Install Xerces


```
Write-Output "Setting up Xerces-C++..."

$xercesZip = "$env:WORKING_PATH\xerces-c-3.2.5.zip"
Invoke-WebRequest -Uri "https://dlcdn.apache.org/xerces/c/3/sources/xerces-c-3.2.5.zip" -OutFile $xercesZip
Expand-Archive -Path $xercesZip -DestinationPath "$env:WORKING_PATH"
cd "$env:WORKING_PATH\xerces-c-3.2.5"
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX="$env:WORKING_PATH\Xerces-Out" ..
cmake --build . --config Debug
cmake --build . --config Debug --target install
```

#### Install qt

Download qt 15 source from

https://www.qt.io/offline-installers

And save its download location

```
$env:QT_DOWNLOAD_PATH=YOUR_DOWNLOAD_PATH
```

And extract it

Then, following this - https://doc.qt.io/qt-5/windows-building.html


Create the file `qtvars.cmd` and place it in the qt5 download directory

```
REM Set up Microsoft Visual Studio 2017, where <arch> is amd64, x86, etc.
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
SET _ROOT="$env:QT_DOWNLOAD_PATH"
SET PATH=%_ROOT%\qtbase\bin;%_ROOT%\gnuwin32\bin;%PATH%
REM Uncomment the below line when using a git checkout of the source repository
REM SET PATH=%_ROOT%\qtrepotools\bin;%PATH%
SET _ROOT=
```

Execute

```
cmd.exe /E:ON /V:ON /k "$env:QT_DOWNLOAD_PATH\qt5vars.cmd"
```

and

```
cd $env:QT_DOWNLOAD_PATH

.\configure -prefix "$env:WORKING_PATH\Qt5-Out" -opensource -confirm-license -release -nomake tests -nomake examples -skip qt3d -skip qtcanvas3d -skip qtcharts -skip qtdatavis3d -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtlottie -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquick3d -skip qtquickcontrols -skip qtquickcontrols2 -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qtsvg -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebglplugin -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras   -opengl desktop -no-icu -no-openssl -no-glib -no-dbus -no-sql-odbc -no-qml-debug -no-warnings-are-errors -no-iconv -optimize-size -ltcg -nomake examples
```

Now that the build configuration is done, you can build qt. To speed up the build process you can use `jom` tool [available here](https://wiki.qt.io/Jom). The tool can be downloaded from [this link](http://download.qt.io/official_releases/jom/jom.zip)

To build with `jom` just run the command from the `$env:QT_DOWNLOAD_PATH` folder:

```
$env:JOM_DOWNLOAD_PATH/jom.exe
```

#### Build and install framework

To build the framework you first need to clone the repo in a desired location:

```
$env:GIT_LOCATION=YOUR_DESIRED_GIT_CLONE_PATH
cd $env:GIT_LOCATION
git clone https://github.com/asam-ev/qc-framework.git
```

```
Write-Output "Setting up QC-Framework..."

cd "$env:GIT_LOCATION\qc-framework"
mkdir build

cmake -H"$env:GIT_LOCATION\qc-framework" -S. -Bbuild `
-G "Visual Studio 17 2022" -A x64 `
-DCMAKE_INSTALL_PREFIX="$env:WORKING_PATH\QC-Framework-Out" `
-DENABLE_FUNCTIONAL_TESTS="$env:TEST_ENABLED" `
-DGTest_ROOT="$env:WORKING_PATH\gtest-Out" `
-DQt5_ROOT="$env:WORKING_PATH\Qt5-Out" `
-DXercesC_ROOT="$env:WORKING_PATH\Xerces-Out"

cmake --build build --target ALL_BUILD --config Release

cmake --install build

Write-Output "All installations and setups are complete!"
```

With the following CMake values:

- _\<prefix\>_: The prefix CMake installs the package to
- _\<GTest_ROOT\>_: The root dir of the pre-built GoogleTest package
- _\<xerces_c_root\>_: The root dir of the pre-built Xerces-C++ package
- _\<Qt5_ROOT\>_: The root dir of the pre-built qt5 package

After builds complete, you may need to manually copy the xerces.dll library to the framework build bin folder:

```
Move-Item $env:WORKING_PATH\xerces-c-3.2.5\*.dll $env:INSTAL_PREFIX\bin\
```

**Prebuilt binaries**

Prebuilt binaries for Windows OS are available on the [release page](https://github.com/asam-ev/qc-framework/releases) in the Assets section, with the name `qc-framework-executables-windows-2022.zip`.

Prebuilt binaries are tested on windows 2022.


### Options

| Name | Value | Description | Remarks |
| ---- | ----- | ----------- | ------- |
| ENABLE_FUNCTIONAL_TESTS | ON/OFF | choose whether the tests were created in the building process of the libraries or not | dependency to a valid gtest package needed (see <https://github.com/google/googletest>) |

## Setup Virtual Environment for Python Modules

We recommend installing Python modules in a virtual environment. Virtual environment for Python modules can be setup with both [virtualenv](https://docs.python.org/3/library/venv.html) (if Python 3.10 or newer is available on your computer) and [Conda](https://docs.anaconda.com/miniconda/) (if Python 3.10 or newer is not available on your computer).

Using Virtualenv:

```bash
python3 -m venv .venv
source .venv/bin/activate
```

Using Conda:

```bash
conda create -y -n .venv python=3.10
conda activate .venv
```

## Install Python Modules

Python modules can be installed using `pip`.

**From PyPi**

```bash
pip install asam-qc-framework
```

**From GitHub repository**

```bash
pip install asam-qc-framework@git+https://github.com/asam-ev/qc-framework@main#subdirectory=qc_framework
```

The above command will install `asam-qc-framework` from the `main` branch. If you want to install `asam-qc-framework` from another branch or tag, replace `@main` with the desired branch or tag.

**From a local repository**

```bash
pip install /home/user/qc-framework/qc_framework
```

## Install ASAM Checker Bundles

Standard ASAM Checker Bundles are implemented in Python and can be installed using `pip`.

### ASAM OpenDrive Checker Bundle

**From PyPi**

```bash
pip install asam-qc-opendrive
```

**From GitHub repository**

```bash
pip install asam-qc-opendrive@git+https://github.com/asam-ev/qc-opendrive@main
```

The above command will install `asam-qc-opendrive` from the `main` branch. If you want to install `asam-qc-opendrive` from another branch or tag, replace `@main` with the desired branch or tag.

**From a local repository**

```bash
pip install /home/user/qc-opendrive
```

To test the installation:

```bash
qc_opendrive --help
```

### ASAM OpenScenario XML Checker Bundle

**From PyPi**

```bash
pip install asam-qc-openscenarioxml
```

**From GitHub repository**

```bash
pip install asam-qc-openscenarioxml@git+https://github.com/asam-ev/qc-openscenarioxml@main
```

The above command will install `asam-qc-openscenarioxml` from the `main` branch. If you want to install `asam-qc-openscenarioxml` from another branch or tag, replace `@main` with the desired branch or tag.

**From a local repository**

```bash
pip install /home/user/qc-openscenarioxml
```

To test the installation:

```bash
qc_openscenario --help
```

### ASAM OTX Checker Bundle

**From PyPi**

```bash
pip install asam-qc-otx
```

**From GitHub repository**

```bash
pip install asam-qc-otx@git+https://github.com/asam-ev/qc-otx@main
```

The above command will install `asam-qc-otx` from the `main` branch. If you want to install `asam-qc-otx` from another branch or tag, replace `@main` with the desired branch or tag.

**From a local repository**

```bash
pip install /home/user/qc-otx
```

To test the installation:

```bash
qc_otx --help
```

## Register ASAM Checker Bundles

Both standard ASAM Checker Bundles and custom Checker Bundles must be registered with the framework before they can be used. [The registration is done using manifest files](doc/manual/manifest_file.md).
