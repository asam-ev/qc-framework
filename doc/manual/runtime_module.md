# Runtime module

The runtime module is responsible of executing the qc-framework modules specified in an input configuration file.

Input configuration file need to respect the [config xsd schema](../doc/schema/config_format.xsd)

The runtime module execute the following steps:

1. Validate input xml file given the schema
2. For each checker bundle specified in configuration, execute its process
3. Execute result pooling for collect results from all checker bundle executed in step 2
4. For each report module specified in configuration, execute its process

## Installation & Usage

The runtime module can be installed using pip.

```bash
pip install asam-qc-runtime@git+https://github.com/asam-ev/qc-framework@develop#subdirectory=runtime
```

Then, it can be executed as follows.

```bash
qc_runtime --config=$PATH_TO_CONFIG_FILE --manifest=$PATH_TO_MANIFEST_FILE
```

where

- `$PATH_TO_CONFIG_FILE` points to an xml file following the [config xsd schema](../doc/schema/config_format.xsd)
- `$PATH_TO_MANIFEST_FILE` points to the [manifest file](manifest_file.md) of the framework.

All the files generated during the runtime execution, including result files and report files, will be saved in the output folder `qc-output-YYYY-MM-DD-HH-MM-SS-*`.

Alternatively, users can specify a specific output folder for the runtime using the argument `--working_dir`.

```bash
qc_runtime --config=$PATH_TO_CONFIG_FILE --manifest=$PATH_TO_MANIFEST_FILE --working_dir=$PATH_TO_OUTPUT_FOLDER
```
