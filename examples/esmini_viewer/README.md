# Esmini viewer plugin

This folder contain the plugin to add an odr viewer based on esmini-odrviewer application

## Installation

After building the framework, a new plugin will be shown in the `File` dropdown menu of the ReportGUI application

The plugin executes the command

```
odrviewer --density 0 --window 100 100 800 800 --odr YOUR_ODR
```

So in order to execute this you need to

1. Download esmini-demo from [esmini release page](https://github.com/esmini/esmini/releases)
2. Copy the `odrviewer` executable from the `esmini-demo/bin` directory to your install directory, where the ReportGUI executable is also generated

## Usage

The plugin checks the input file pointed by the xqar file you are inspecting:

- If it is an OTX file, the plugin does not show anything
- If it is an ODR file, the plugin shows it
- If it is an OSC file, the plugin get the correspoding openDRIVE file from the input openSCENARIO and shows that road network

Currently the plugin has only the functionality of showing the road network and it is presented for demonstration purposes.
