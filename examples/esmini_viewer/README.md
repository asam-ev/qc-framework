# Esmini viewer plugin


The esmini plugin is a plugin to open esmini simulator to visualize an input OpenDRIVE file

The plugin depends on esminiLib API. More information about the API can be found on the [official documentation](https://esmini.github.io/#_esmini_lib_programming)

## Installation

After building the framework, a new plugin will be shown in the `File` dropdown menu of the ReportGUI application

The plugin depends on a single file, the esminiLib library that is included in each [esmini release](https://github.com/esmini/esmini/releases).

- If you are working on windows, the library you will need is `esminiLib.dll`

- If you are working on Linux, the library is `libesminiLib.so`

The library file needs to be downloaded on user local computer.

Then, the application searches for the library file following this approach:

1. Check if library file is present in the folder pointed by the `ESMINI_LIB_PATH` environment variable
2. Check if library file is present in the folder where the `ReportGUI` executable is present

So in order to use the plugin you can either:

- Define the ESMINI_LIB_PATH environment variable to where you downloaded the library file from esmini

    e.g.
    ```
    export ESMINI_LIB_PATH=/home/user/Downloads/esmini-demo/bin
    ```

- Copy the library file to the location where ReportGUI application is present

    e.g.
    ```
    cp /home/user/Downloads/esmini-demo/bin/libesminiLib.so ~/qc-build/bin/
    ```



## Additional information

#### Which OpenDRIVE file to render

In order to establish which openDRIVE file to render in esmini, the plugin reads the `InputFile`` param from currently shown result file:

- If it is an OTX file, ignore it
- If it is an XODR file, wrap it in a simple and empty xosc file
- If it is an XOSC file, extract its XODR file and wrap it in a simple and empty xosc file

The template empty scenario file is created starting from [the string defined here](./xml_util.h#30)

#### esminiLib function

The plugin uses the following function from esminiLib:

- [`SE_Init`](https://github.com/esmini/esmini/blob/master/EnvironmentSimulator/Libraries/esminiLib/esminiLib.cpp#L645): used to initialize the player to visualize the right OpenDRIVE file
- [`SE_GetIdByName`](https://github.com/esmini/esmini/blob/master/EnvironmentSimulator/Libraries/esminiLib/esminiLib.cpp#L1424): to retrieve the `marker` object used to move the camera in the scenario
- [`SE_ReportObjectPosXYH`](https://github.com/esmini/esmini/blob/master/EnvironmentSimulator/Libraries/esminiLib/esminiLib.cpp#L1191): used to move the camera at specified location
- [`SE_Step`](https://github.com/esmini/esmini/blob/master/EnvironmentSimulator/Libraries/esminiLib/esminiLib.cpp#L955): used to allow the esmini player to advance
- [`SE_Close`](https://github.com/esmini/esmini/blob/master/EnvironmentSimulator/Libraries/esminiLib/esminiLib.cpp#L939): used to close the esmini player
