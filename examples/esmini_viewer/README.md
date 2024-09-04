# Esmini viewer plugin


The plugin is based on two application, the esmini plugin from the current directory and the `EsminiIPCServer` application

`EsminiIPCServer` uses `esminiLib.hpp` library to interact with esmini player.

## Installation

After building the framework, a new plugin will be shown in the `File` dropdown menu of the ReportGUI application

The plugin starts a second application `EsminiIPCServer` that receives esmini command over the network and executes them.

So in order to execute this you need to

1. Have `EsminiIPCServer` application on your computer. Current PR where it is developed can be found [here](https://github.com/IVEX-AI/esmini/pull/1)
2. Add `EsminiIPCServer` to your system path

    - Linux install
        Long term installation:

        ```
        sudo mv YOUR_BIN_DIRECTORY/EsminiIPCServer /usr/local/bin/
        ```

        Short term installation:

        ```
        export PATH=$PATH:YOUR_BIN_DIRECTORY/
        ```
3. The `ReportGUI` application that uses this plugin needs to declare the environment variable `ASAM_QC_FRAMEWORK_INSTALLATION_DIR`, to point where the qc-framework is installed

    e.g.
    ```
    export ASAM_QC_FRAMEWORK_INSTALLATION_DIR=/home/user/qc-build/
    ```

## Architecture

The plugin is based on two application, the esmini plugin from the current directory and the `EsminiIPCServer` application

The two application sends information to each other in the form of JSON messages exchanged over TCP socket

Communication is by default set to happen at `127.0.0.1:8080`

### Protocol

#### Step 1 - Init scenario

- From esmini-viewer plugin

    1. Read `InputFile` param from currently shown result file.

        - If it is an OTX file, ignore it
        - If it is an XODR file, wrap it in a simple xosc file using [template file](./asam_files/template.xosc)
        - If it is an XOSC file, extract its XODR file and wrap it in a simple xosc file using [template file](./asam_files/template.xosc)


    2. Send the init json following the format

        ```
        nlohmann::json init_esmini_json = {
            {"function", "SE_Init"},
            {"args": {
                        "xosc_path": string
                        "disable_ctrls": int
                        "use_viewer": int
                        "threads": int
                        "record": int
                    }
            }
        };
        ```

- From `EsminiIPCServer` app
    1. Execute `SE_Init`` with passed args and wait


#### Step 2 - Update camera

- From esmini-viewer plugin

    1. Send the inertial location in JSON following the format:
        ```
        nlohmann::json inertial_location_json = {{"function", SE_ReportObjectPosXYH},
                                                    {"object_id", "Ego"},
                                                    {"x", ext_inertial_location->GetX()},
                                                    {"y", ext_inertial_location->GetY()},
                                                    {"z", ext_inertial_location->GetZ()},
                                                    {"h", ext_inertial_location->GetH()}};
        ```
- From `EsminiIPCServer` app

    1. Execute `SE_ReportObjectPosXYH` with passed arguments

#### Step 3 - Step

This is needed in order to make the location to be updated and to control esmini player playback

- From esmini-viewer plugin

    1. Send the step JSON following the format:
        ```
        nlohmann::json step_json = {{"function", SE_Step}}
        ```
- From `EsminiIPCServer` app

    1. Execute `SE_Step`

#### Step 4 - Close player

- From esmini-viewer plugin

    1. Send the close message in JSON following the format:
        ```
        nlohmann::json step_json = {{"function", SE_Close}}

        ```
- From `EsminiIPCServer` app

    1. Execute `SE_Close` with passed arguments
