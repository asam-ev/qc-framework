# Demo pipeline

The demo pipeline allow users to process OpenDRIVE and OpenSCENARIO XML files with respecting checkers and inspect the resulting `xqar` and `txt` files.

### Docker installation

Docker is required to run the demo pipeline. To check if Docker is already installed on your system, the following command can be used for both Linux and Windows.

```
docker run hello-world
```

If Docker is installed, the following text will be shown.

```
docker run hello-world

Hello from Docker!
This message shows that your installation appears to be working correctly.

To generate this message, Docker took the following steps:
 1. The Docker client contacted the Docker daemon.
 2. The Docker daemon pulled the "hello-world" image from the Docker Hub.
    (amd64)
 3. The Docker daemon created a new container from that image which runs the
    executable that produces the output you are currently reading.
 4. The Docker daemon streamed that output to the Docker client, which sent it
    to your terminal.

To try something more ambitious, you can run an Ubuntu container with:
 $ docker run -it ubuntu bash

Share images, automate workflows, and more with a free Docker ID:
 https://hub.docker.com/

For more examples and ideas, visit:
 https://docs.docker.com/get-started/
```

If Docker is not available on your system, you can follow the [Docker Engine Installation Guide for Linux](https://docs.docker.com/engine/install/ubuntu/#install-using-the-repository) or the [Docker Desktop Installation Guide for Windows](https://docs.docker.com/desktop/install/windows-install/). Please note the terms and conditions of Docker Desktop for Windows. 

### Download and run

The demo pipeline is provided as a public Docker image in the [Github container registry](https://github.com/asam-ev/qc-framework/pkgs/container/qc-framework).

To process a file, the `docker run` command below can be used and the following information can be specified:
- The input folder which contains the input file.
- The name the input file.
- The output folder where the output files can be saved.

### For Linux users

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

### For Windows users

The only difference between Linux and Windows is that on Windows, you don't need to set the `USER_ID` and `GROUP_ID` on the `docker run` command.

```
docker run \
    -e INPUT_FILENAME=YOUR_INPUT_FILENAME \
    -v YOUR_INPUT_FOLDER:/input_directory \
    -v YOUR_OUTPUT_FOLDER:/out \
    --rm --name demo_pipeline ghcr.io/asam-ev/qc-framework:demo-pipeline-latest
```

E.g. To process the file at `C:\Users\user\input_folder\test_ramp.xosc`

```
docker run \
    -e INPUT_FILENAME=test_ramp.xosc \
    -v C:\Users\user\input_folder:/input_directory \
    -v /home/user/output:/out \
    --rm --name demo_pipeline ghcr.io/asam-ev/qc-framework:demo-pipeline-latest
```

Alternatively, you can go to the input folder and execute the following command, which requires only the input file name to be specified. The output will be saved in the same folder.

```
cd C:\Users\user\input_folder

docker run \
    -e INPUT_FILENAME=test_ramp.xosc \
    -v $(pwd):/input_directory \
    -v $(pwd):/out \
    --rm --name demo_pipeline ghcr.io/asam-ev/qc-framework:demo-pipeline-latest
```

### Output

The docker image will automatically:
- Detect the type of file passed as input.
- Create the specific config according to [config schema](../doc/schema/config_format.xsd).
- Execute the runtime with specific checker, result pooling and text result application.

Currently the demo_pipeline will clone and execute:

- [OpenDRIVE checker @ develop branch](https://github.com/asam-ev/qc-opendrive/tree/develop)
- [OpenSCENARIO XML checker @ develop branch](https://github.com/asam-ev/qc-openscenarioxml/tree/develop)

After the execution, in the specified output folder you will find:

- Specific CheckerBundle `xqar` result file.
- ResultPooling `Result.xqar` result file.
- TextReport `Report.txt` text file.

Some OpenDrive and OpenScenario XML test files are available to try out.
- [OpenDrive test files](https://github.com/asam-ev/qc-opendrive/tree/develop/tests/data)
- [OpenScenario XML test files](https://github.com/asam-ev/qc-openscenarioxml/tree/develop/tests/data)


### Update to the latest version

The demo pipeline is built every day at 4am UTC. To update the demo pipeline on your local machine, use the following command.

```
docker pull ghcr.io/asam-ev/qc-framework:demo-pipeline-latest
```


### Local build instructions

The image can only be built on Linux. To build the Docker image locally, you can execute:

```
cd ..

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile.linux \
  --target demo_pipeline \
  -t demo_pipeline .
```
