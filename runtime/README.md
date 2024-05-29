# Runtime module

The runtime module is responsible of executing the qc-framework modules specified in an input configuration file.

Input configuration file need to respect the [config xsd schema](../doc/schema/config_format.xsd)

The runtime module execute the following steps:

1. Validate input xml file given the config_format.xsd schema
2. For each checker bundle specified in configuration, execute its process
3. Execute result pooling for collect results from all checker bundle executed in step 2
4. For each report module specified in configuration, execute its process+

## Installation & Usage

The runtime module is implemented in the [python script runtime.py](./runtime/runtime.py)

In order to use the runtime, you can set up a virtual environment using your preferred python 3.6+ e.g. with 3.8

```
python3.8 -m venv runtime_env
```

activate it and install the required dependencies

```
source runtime_env/bin/activate

pip3 install -r requirements.txt
```

Then from the same terminal you can execute the runtime:

```
python3 runtime/runtime.py --config=$PATH_TO_CONFIG_XML --install_dir=$PATH_TO_EXE --schema_dir=$PATH_TO_XSD 
```

where 

- `$PATH_TO_CONFIG_XML` points to an xml file following the [config xsd schema](../doc/schema/config_format.xsd)
- `$PATH_TO_EXE` points to the folder where framework executables are installed
- `$PATH_TO_XSD` points to a folder containig the config_format.xsd file

#### Note

- `$PATH_TO_EXE` and `$PATH_TO_XSD` args are temporarily supported until better handling of external directory will be implemented

