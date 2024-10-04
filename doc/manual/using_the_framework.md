<!---
Copyright 2023 CARIAD SE.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# Using the ASAM Quality Checker Framework

## Create Configuration File

Create a [configuration file](file_formats.md) to use the framework, following [the predefined configuration file schema](../schema/config_format.xsd) and the explanation in [the configuration file documentation](file_formats.md).

Example configuration files for running the official Checker Bundles for ASAM OpenDrive, OpenScenario XML and OTX can be found in [the templates folder](../../demo_pipeline/templates).

## Run the Checker Bundles

The Checker Bundles are run by the [quality checker framework](python_qc_framework.md).

```bash
qc_runtime --config "PATH_TO_CONFIG_FILE" --manifest "PATH_TO_MANIFEST_FILE"
```

The output of the framework are the `.xqar` [result files](file_formats.md) and any other output files from the specified report modules in the configuration file, such as `.txt` files for the text report module. If the ReportGUI is specified, the Report GUI will open.

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

A viewer application based on the [esmini simulator](https://esmini.github.io/) is 
provided together with the ASAM Quality Checker Framework. Please refer to the 
[esmini viewer plugin](esmini_viewer_plugin.md) installation guide for details.

The ASAM Quality Checker Framework also provide a binding mechanism which lets you
provide your own integration to third-party viewer application. Please refer to [the
documentation](viewer_interface.md) for details.

## Add Self Implemented CheckerBundles and ReportModules

You can create and add your own CheckerBundles and ReportModules to the framework.
Requirements for your own CheckerBundle can be found in the [User defined
modules](writing_user_defined_modules.md) documentation.
