# Manifest File

Manifest files represent the system used by Checker Bundles and Report Modules to register themselves with a framework installation. Manifest files depend on the operating system of the user. Manifest files are the way to tell the framework how to invoke Checker Bundles and Report Modules. 

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
      "exec_command": "/home/user/.venv/bin/python -m qc_opendrive.main -c $ASAM_QC_FRAMEWORK_CONFIG_FILE -o $ASAM_QC_FRAMEWORK_OUTPUT_DIR"
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
      "exec_command": "cd /home/user/qc-framework/bin && TextReport $ASAM_QC_FRAMEWORK_RESULT_FILE $ASAM_QC_FRAMEWORK_OUTPUT_DIR"
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
      "exec_command": "/home/user/.venv/bin/python -m qc_opendrive.main -c $ASAM_QC_FRAMEWORK_CONFIG_FILE -o $ASAM_QC_FRAMEWORK_OUTPUT_DIR"
    },
    {
      "name": "TextReport",
      "exec_type": "executable",
      "module_type": "report_module",
      "exec_command": "cd /home/user/qc-framework/bin && TextReport $ASAM_QC_FRAMEWORK_RESULT_FILE $ASAM_QC_FRAMEWORK_OUTPUT_DIR"
    }
  ]
}
```

The manifest of each module must contain the following information.

* `name`: The name of the module. This name will be used in the [configuration file](file_formats.md).
* `exec_type`: The type of module execution. Currently, only `executable` is supported as `exec_type`. The `executable` type supports the execution of any command in the `exec_command`.
* `module_type`: The type of the module. It must be either a `checker_bundle` or a `report_module`.
* `exec_command`: The command to be executed when the corresponding Checker Bundle or Report Module is invoked by the framework.

## Register a Checker Bundle or Report Module to the Framework

To register a Checker Bundle or Report Module with the framework:
* First, a module manifest file must be provided.
* Second, the path to the module manifest file must be included in the framework manifest file. 
* Last, the framework manifest file can be provided to the [runtime module](runtime_module.md) when triggering a framework execution.
  ```bash
  qc_runtime --config=$PATH_TO_CONFIG_FILE --manifest=$PATH_TO_MANIFEST_FILE
  ```