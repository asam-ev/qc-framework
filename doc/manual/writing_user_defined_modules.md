<!---
Copyright 2023 CARIAD SE.
 
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# User Defined Modules

If an own module is called without any command line arguments or options, then
it should print a short help how to use it.

## CheckerBundles

### CheckerBundle Requirements

- You should provide an executable
- Your own module should adhere to the following rules.
- The executable needs execution rights and its location should be the bin
  folder (see [File Formats](file_formats.md) for configuration details)

**Note:** The [Python Base Library](https://github.com/asam-ev/qc-baselib-py) and the [C++ Base Library](cpp_base_library.md) help to read and write the
file formats.

Example Checker Bundles using the Python Base Library can be found on the following repositories:
* https://github.com/asam-ev/qc-openscenarioxml
* https://github.com/asam-ev/qc-opendrive
* https://github.com/asam-ev/qc-otx

### Run One Single CheckerBundle Based on a Configuration

Your CheckerBundle should accept a xml file containing its configuration parameters.
All **modules which follow this convention are a part of the framework** and
**can be executed from the runtime**.

    > checker_bundle_exe configuration.xml

**Note:** Any Checker Bundle can have an executable using Bash script, even if the Checker Bundle is implemented in intepreted languages like Python.

### Check a Single File With One Single CheckerBundle

A CheckerBundle can be called with one command line parameter. 
The first parameter defines the configuration file. 
The CheckerBundle should write in the current directory an (.xqar) 
file with the same name as the executable.

**_Note: The above instruction will change after the Manifest file is supported._**

This approach is used when testing only one CheckerBundle.

    > checker_bundle_exe config.xml

## ReportModules

### ReportModule Requirements

- You have to provide an executable
- Your own module shall visualize the results based on the result format
- Your module shall take a module configuration, based on the configuration
  format.
- The executable needs execution rights and its location should be the bin
  folder (see [File Formats](file_formats.md) for configuration details)

Here is [an example ReportModule implemented in Python](https://github.com/asam-ev/qc-baselib-py/tree/develop/examples/report_module_text).
