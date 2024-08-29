# Manifest File

Manifest files represent the system used by Checker Bundles and Report Modules to register themselves with a framework installation. Manifest files depend on the operating system of the user. Manifest files are the way to tell the framework how to invoke Checker Bundles and Report Modules.

Below are the complete sets of example manifest files for all modules provided by the Quality Checker Framework. Users of the framework can modify the file paths in the example manifest files to create their own manifest files for their environment.
* [Example manifest files for Linux](../../manifest_examples/linux/)
* [Example manifest files for Windows](../../manifest_examples/windows/)

There are two types of manifest files in the framework: **Framework manifest files** and **Module manifest files**.

## Framework Manifest File

A framework manifest file is a JSON file containing a list of module manifest file paths.
The [runtime module](runtime_module.md) uses the framework manifest file to discover
and execute Checker Bundles and Report Modules.

The framework manifest file must follow the JSON format as in the example below.

**manifest.json**
```json
{
  "manifest_file_path": [
    "/home/user/qc-opendrive/manifest.json",
    "/home/user/qc-openscenarioxml/manifest.json",
    "/home/user/qc-otx/manifest.json",
    "/home/user/qc-osi/manifest.json"
    "/home/user/text-report/manifest.json",
    "/home/user/report-gui/manifest.json",
  ]
}
```

## Module Manifest File

Each Checker Bundle or Report Module needs to provide a manifest file for the framework.
Below are an example manifest files for a Checker Bundle and a Report Module

**Example manifest file for a Checker Bundle**

```json
{
  "module": [
    {
      "name": "qc-opendrive",
      "exec_type": "executable",
      "module_type": "checker_bundle",
      "exec_command": "cd $ASAM_QC_FRAMEWORK_WORKING_DIR && /home/user/.venv/bin/python -m qc_opendrive.main -c $ASAM_QC_FRAMEWORK_CONFIG_FILE"
    }
  ]
}
```

**Example manifest file for a Report Module**

```json
{
  "module": [
    {
      "name": "TextReport",
      "exec_type": "executable",
      "module_type": "report_module",
      "exec_command": "cd $ASAM_QC_FRAMEWORK_WORKING_DIR && /home/user/qc-framework/bin/TextReport $ASAM_QC_FRAMEWORK_CONFIG_FILE"
    }
  ]
}
```

Note that, it is possible to specify the manifest of multiple modules in one file. For example:

```json
{
  "module": [
    {
      "name": "qc-opendrive",
      "exec_type": "executable",
      "module_type": "checker_bundle",
      "exec_command": "cd $ASAM_QC_FRAMEWORK_WORKING_DIR && /home/user/.venv/bin/python -m qc_opendrive.main -c $ASAM_QC_FRAMEWORK_CONFIG_FILE"
    },
    {
      "name": "TextReport",
      "exec_type": "executable",
      "module_type": "report_module",
      "exec_command": "cd $ASAM_QC_FRAMEWORK_WORKING_DIR && /home/user/qc-framework/bin/TextReport $ASAM_QC_FRAMEWORK_CONFIG_FILE"
    }
  ]
}
```

The manifest file is also used internally by the framework to specify the [Result Pooling](architecture.md#workflow-asam-quality-checker-framework) module. The Result Pooling module is provided by the framework. Standard users are not expected to implement a result pooling module.

```json
{
  "module": [
    {
      "name": "ResultPooling",
      "exec_type": "executable",
      "module_type": "result_pooling",
      "exec_command": "cd $ASAM_QC_FRAMEWORK_WORKING_DIR && /home/user/qc-framework/bin/ResultPooling $ASAM_QC_FRAMEWORK_WORKING_DIR $ASAM_QC_FRAMEWORK_CONFIG_FILE"
    }
  ]
}
```

The manifest of each module must contain the following information.

* `name`: The name of the module. This name will be used in the [configuration file](file_formats.md).
* `exec_type`: The type of module execution. Currently, only `executable` is supported as `exec_type`. This type supports the execution of any command in the `exec_command`.
* `module_type`: The type of the module. It must have one of the following value.
  * `checker_bundle`
  * `report_module`
  * `result_pooling`
* `exec_command`: The command to be executed when the corresponding Checker Bundle or Report Module is invoked by the framework. The command has access to the environment variables defined by the framework (see the next section: Framework Environment Variables). The `exec_command` must accept the configuration file defined in `ASAM_QC_FRAMEWORK_CONFIG_FILE` and output any files to the directory defined in `ASAM_QC_FRAMEWORK_WORKING_DIR`.

## Framework Environment Variables

The Quality Checker Framework provides the following environment variables to be used in the `exec_command` in module manifest files.

| Environment variable | Meaning |
| --- | --- |
| `ASAM_QC_FRAMEWORK_CONFIG_FILE` | Path to the configuration file |
| `ASAM_QC_FRAMEWORK_WORKING_DIR` | Path to the working directory of the framework, where all the output files should be generated |

## Register a Checker Bundle or Report Module to the Framework

To register a Checker Bundle or Report Module with the framework:
* First, a module manifest file must be provided.
* Second, the path to the module manifest file must be included in the framework manifest file.
* Last, the framework manifest file can be provided to the [runtime module](runtime_module.md) when triggering a framework execution.
  ```bash
  qc_runtime --config=$PATH_TO_CONFIG_FILE --manifest=$PATH_TO_MANIFEST_FILE
  ```
