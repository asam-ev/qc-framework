<!---
Copyright 2023 CARIAD SE.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# Using the ASAM Quality Checker Framework

## Create Configuration File

Create a [configuration file](file_formats.md) to use the framework, following [the predefined configuration file schema](https://github.com/asam-ev/qc-framework/blob/develop/doc/schema/config_format.xsd).

Example configuration files for running the official Checker Bundles for ASAM OpenDrive, OpenScenario XML and OTX can be found in [the templates folder](https://github.com/asam-ev/qc-framework/tree/develop/demo_pipeline/templates).

## Run the Checker Bundles

The Checker Bundles can be run using the [runtime component](../../runtime/README.md).

```bash
source runtime-venv/bin/activate
python3 runtime/runtime/runtime.py \
    --config "PATH_TO_YOUR_CONFIG_FILE" \
    --install_dir "qc-build/bin" \
    --schema_dir "doc/schema"
```

The output of the runtime components are the `.xqar` [result files](file_formats.md) and any other output files from the specified report modules in the configuration file, such as `.txt` files for the text report module. If the ReportGUI is specified, the Report GUI will open.

## Reporting

The reporting of issues can be done in a text file, in a GUI and/or graphically
in the 3D model of the road network. The foundation for this is a file in the
XML base Checker Result Format (XQAR) containing all issues. The file format
specification can be found on the page [File formats](file_formats.md).

The ASAM Quality Checker Framework contains the following report modules:

- TextReport - generating a human readable text file with all issues
- ReportGUI

### Using the ReportGUI

This GUI can be used to filter reported issues. Just click on a CheckerBundle
entry to show all issues from this CheckerBundle. The list of Checkers can be
used to filter issues from one Checker in one CheckerBundle. The window on the
bottom left contains all the information about the selected issue. The source
code on the right side moves to the location in the file, if the issue contains
a valid FileLocation.

![Reporting GUI](images/reporting_gui.png)

Additionally it is possible to link issues that correspond to a 3D error with a
viewer application. A 3D error is an error in the 3D model of the road network.
It gets marked with a little icon in the left part of the ReportGUI window.
This viewer is not part of the Open Source distributution of the ASAM Quality
Checker Framework framework, but we provide a binding mechanism which let's you
provide your own implementation. Please refer to [the
documentation](viewer_interface.md) for details.

## Add Self Implemented CheckerBundles and ReportModules

You can add your own CheckerBundles and ReportModules to the framework.
Just add the executables to the folder where the framework executables are installed.
This will make the executables available to the runtime component.
In our example above, the folder is `qc-build/bin`.

Executables for Checker Bundles written in any programming languages can be created using 
bash script, even for intepreted languages like Python.

Requirements for your own CheckerBundle can be found in the [User defined
modules](writing_user_defined_modules.md) documentation.
