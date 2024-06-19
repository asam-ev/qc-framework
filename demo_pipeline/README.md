# Demo pipeline

The demo pipeline allow users to process OpenDRIVE and OpenSCENARIO files with respecting checkers and inspect the resulting `xqar` and `txt` files.

### Download and run

The demo pipeline is provided as a public Docker image in the Github container registry.

To process a file, the `docker run` command below can be used and the following information can be specified:
- The input folder which contains the input file.
- The name the input file.
- The output folder where the output files can be saved.

```
docker run \
    -e INPUT_FILENAME=YOUR_INPUT_FILENAME \
    -v YOUR_INPUT_FOLDER:/input_directory \
    -v YOUR_OUTPUT_FOLDER:/out \
    -e USER_ID=$(id -u) \
    -e GROUP_ID=$(id -g) \
    --rm --name demo_pipeline ghcr.io/asam-ev/qc-framework:demo-pipeline-latest
```

E.g. To process the file at `/home/user/xodr_files/test_ramp.xosc`

```
docker run \
    -e INPUT_FILENAME=test_ramp.xosc \
    -v /home/user/xodr_files:/input_directory \
    -v /home/user/output:/out \
    -e USER_ID=$(id -u) \
    -e GROUP_ID=$(id -g) \
    --rm --name demo_pipeline ghcr.io/asam-ev/qc-framework:demo-pipeline-latest
```

Alternatively, you can go to the input folder and execute the following command, which requires only the input file name to be specified. The output will be saved in the same folder.

```
cd /home/user/xodr_files

docker run \
    -e INPUT_FILENAME=test_ramp.xosc \
    -v $(pwd):/input_directory \
    -v $(pwd):/out \
    -e USER_ID=$(id -u) \
    -e GROUP_ID=$(id -g) \
    --rm --name demo_pipeline ghcr.io/asam-ev/qc-framework:demo-pipeline-latest
```

The docker image will automatically:
- Detect the type of file passed as input.
- Create the specific config according to [config schema](../doc/schema/config_format.xsd).
- Execute the runtime with specific checker, result pooling and text result application.

Currently the demo_pipeline will clone and execute:

- [OpenDRIVE checker @ develop branch](https://github.com/asam-ev/qc-opendrive/tree/develop)
- [OpenSCENARIO checker @ develop branch](https://github.com/asam-ev/qc-openscenarioxml/tree/develop)

After the execution, in the specified output folder you will find:

- Specific CheckerBundle `xqar` result file.
- ResultPooling `Result.xqar` result file.
- TextReport `Report.txt` text file.

### Local build instructions

In case of local build of demo_pipeline docker image, you can execute:

```
cd ..

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile.linux \
  --target demo_pipeline \
  -t demo_pipeline .
```
