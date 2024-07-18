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

### Run One Single CheckerBundle Based on a Configuration

Your CheckerBundle should accept a xml file containing its configuration parameters.
All **modules which follow this convention are a part of the framework** and
**can be executed from the runtime**.

    > checker_bundle_exe configuration.xml

### Determine Checkers and Parameters

If the CheckerBundle executable is called with the command line argument
`--defaultConfig`, it should write a report file (.xqar, see Base library for
details) containing all checkers with their parameters and no issues. Each
parameter should contain default values. The name of the file has to
corresponds to the executable name (myCheckerBundle.exe should write
myCheckerBundle.xqar).

    > checker_bundle_exe --defaultConfig

### Check a Single File With One Single CheckerBundle

A CheckerBundle can be called with one command line parameter and optional
parameter settings. The first parameter defines the file which should be
checked. The additional options are specific for the CheckerBundle and not
defined here. As in the previous cases the CheckerBundle should write in the
current directory an (.xqar) file with the same name as the executable.

This approach is used when testing only one CheckerBundle.

    > checker_bundle_exe track.xodr [-o1 -o2 ...]

## ReportModules

### ReportModule Requirements

- You have to provide an executable
- Your own module shall visualize the results based on the result format
- Your module shall take a module configuration, based on the configuration
  format.
- Your module shall provide a default configuration by the parameter
  {{--defaultConfig}}
- The executable needs execution rights and its location should be the bin
  folder (see [File Formats](file_formats.md) for configuration details)

### Determine Configurations for ReportModules

If a ReportModule executable is called with the command line argument
`--defaultConfig`, it should write a configuration file (.xml) containing all
the parameters. Each parameter should contain default values. The name of the
file has to corresponds to the executable name (myReportModule.exe should write
myReportModule.xml).

    > report_module_exe --defaultConfig
