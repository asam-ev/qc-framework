# Runtime module

The runtime module is responsible of executing the qc-framework modules specified in an input configuration file.

Input configuration file need to respect the [config xsd schema](../doc/schema/config_format.xsd)

The runtime module execute the following steps:

1. Validate input xml file given the config_format.xsd schema
2. For each checker bundle specified in configuration, execute its process
3. Execute result pooling for collect results from all checker bundle executed in step 2
4. For each report module specified in configuration, execute its process+


## Usage

The runtime module is implemented in the [python script runtime.py](../src/runtime/runtime.py)

It is executed by the docker specified in this folder.

To build the docker image:

```bash
./build.sh
```

To execute the runtime:

```bash
./run.sh XML_FILE OUTPUT_FOLDER
```

where

- XML_FILE is an xml file following the config xsd schema that specifies the steps to execute
- OUTPUT_FOLDER is a path on the host where xqar and txt report are saved

## Tests