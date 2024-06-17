# Demo pipeline

The demo pipeline allow users to process OpenDRIVE and OpenSCENARIO files with respecting checkers and inspect the resulting `xqar` and `txt` files

### Download and run

The demo pipeline is provided as docker image uploaded to Github container registry

To process a file it is needed to mount input and output folder and provide the filename to process

```
docker run \
    -v YOUR_INPUT_FOLDER:/input_directory \
    -v YOUR_OUTPUT_FOLDER:/out \
    -e INPUT_FILENAME=YOUR_INPUT_FILENAME \
    --rm --name demo_pipeline ghcr.io/asam-ev/qc-framework:latest
```

E.g. To process the file at `/home/user/xodr_files/test_ramp.xosc`

```
docker run \
    -v /home/user/xodr_files:/input_directory \
    -v /home/user/output:/out \
    -e INPUT_FILENAME=test_ramp.xosc \
    --rm --name demo_pipeline ghcr.io/asam-ev/qc-framework:latest
```


The docker image will automatically:
- Detect the type of file passed as input
- Create the specific config according to [config schema](../doc/schema/config_format.xsd)
- Execute the runtime with specific checker, result pooling and text result application

Currently the demo_pipeline will clone and execute:

- [OpenDRIVE checker @ develop branch](https://github.com/asam-ev/qc-opendrive/tree/develop)
- [OpenSCENARIO checker @ develop branch](https://github.com/asam-ev/qc-openscenarioxml/tree/develop)

After the execution, in the specified output folder you will find:

- Specific CheckerBundle `xqar` result file
- ResultPooling `Result.xqar` result file
- TextReport `Report.txt` text file

### Build instructions

In case of local build of demo_pipeline docker image, you can execute:

```
cd ..

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile.linux \
  --target demo_pipeline \
  -t demo_pipeline .
```
