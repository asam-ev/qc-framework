<!---
Copyright 2023 CARIAD SE.
Copyright 2024 ASAM e.V.
 
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# User Defined Modules

## Checker Bundles

### Checker Bundle Requirements

A Checker Bundle should accept a configuration file and output a result file (see [File Formats](file_formats.md) for configuration and result file details)

**Note:** The [Python Base Library](https://github.com/asam-ev/qc-baselib-py) and the [C++ Base Library](cpp_base_library.md) help to read and write the file formats. 

It is recommended to use the Python Base Library if possible as it contains the most advanced development. The C++ Base Library is complete in terms of functionality, but it is not as easy to use as the Python Base Library.

A minimal example of using the Python Base Library to implement a Checker Bundle:
* https://github.com/asam-ev/qc-baselib-py/tree/develop/examples/json_validator

A minimal example of using the C++ Base Library to implement a Checker Bundle:
* https://github.com/asam-ev/qc-framework/tree/main/examples/checker_bundle_example

Here are some other advanced example Checker Bundles using the Python Base Library:
* https://github.com/asam-ev/qc-openscenarioxml
* https://github.com/asam-ev/qc-opendrive
* https://github.com/asam-ev/qc-otx

### Run Checker Bundle Based on a Configuration

Your Checker Bundle should provide an executable command that accept the XML configuration file at `$ASAM_QC_FRAMEWORK_CONFIG_FILE` and output the result file at `$ASAM_QC_FRAMEWORK_WORKING_DIR`. For example:

```bash
cd $ASAM_QC_FRAMEWORK_WORKING_DIR && /home/user/.venv/bin/python -m qc_opendrive.main -c $ASAM_QC_FRAMEWORK_CONFIG_FILE
```

Your Checker Bundle should also provide manifest files or manifest file templates for users to register it with the framework in different environments (e.g., Linux and Windows). [Details about manifest files can be found here](manifest_file.md).

## Report Modules

### Report Module Requirements

A Report Module read a result file in `.xqar` format (see [File Formats](file_formats.md)) and convert it to any other desired formats, such as:
* A more human-readable text file.
* A specific file format to be integrated into other applications, such as GitHub WorkFlow Command format.

A Report Module can also be a GUI application, which visualizes the result file.

A Report Module should accept a configuration file which define the result xqar file to be read. For example:

**configuration.xml**
```xml
  <ReportModule application="TextReport">
    <Param name="strInputFile" value="Result.xqar"/>
  </ReportModule>
```

Similar to Checker Bundle, it is recommended to use the [Python Base Library](https://github.com/asam-ev/qc-baselib-py) to implement a Report Module.

A minimal example of using the Python Base Library to implement a Report Module:
* https://github.com/asam-ev/qc-baselib-py/tree/develop/examples/report_module_text

Here are some other advanced example Report Modules using the C++ Base Library:
* https://github.com/asam-ev/qc-framework/tree/main/src/report_modules/report_module_text
* https://github.com/asam-ev/qc-framework/tree/main/src/report_modules/report_module_gui
* https://github.com/asam-ev/qc-framework/tree/main/src/report_modules/report_module_github_ci

### Run Report Module Based on a Configuration

Your Report Module should provide an executable command that accept the XML configuration file at `$ASAM_QC_FRAMEWORK_CONFIG_FILE` and output any result file at `$ASAM_QC_FRAMEWORK_WORKING_DIR`. For example:

```bash
cd $ASAM_QC_FRAMEWORK_WORKING_DIR && /home/user/qc-framework/bin/TextReport $ASAM_QC_FRAMEWORK_RESULT_FILE
```

Your Report Module should also provide manifest files or manifest file templates for users to register it with the framework in different environments (e.g., Linux and Windows). [Details about manifest files can be found here](manifest_file.md).
