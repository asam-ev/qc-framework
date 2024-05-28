#!/usr/bin/env bash

# Copyright 2024, IVEX NV
# All rights reserved.
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential


readonly XML_PATH=$1

docker stop runtime
docker rm runtime

set -e

./build.sh

XML_DIRECTORY=$(dirname "$XML_PATH")

docker run --name runtime \
    -v $XML_DIRECTORY:/data \
    runtime -config=$(basename "$XML_PATH")
