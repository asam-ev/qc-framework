# Framework module

The Framework module is responsible for the execution of checker bundles, result poolers and result reporters, based 
upon a defined configuration file. The main executable is implemented in the `runtime` submodule.

Input configuration file need to respect the [config xsd schema](../doc/schema/config_format.xsd)

The runtime module execute the following steps:

1. Validate input xml file given the schema
2. For each checker bundle specified in configuration, execute its process
3. Execute result pooling for collect results from all checker bundle executed in step 2
4. For each report module specified in configuration, execute its process

## Installation & Usage

The ASAM QC Framework can be installed using pip.

```bash
pip install asam-qc-framework@git+https://github.com/asam-ev/qc-framework@develop#subdirectory=qc_framework
```

**Note:** The above command will install `asam-qc-framework` from the `develop` branch. If you want to install `asam-qc-framework` from another branch or tag, replace `@develop` with the desired branch or tag. It is also possible to install from a local directory.

```bash
# From reporsitory root
pip install ./qc_framework
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
