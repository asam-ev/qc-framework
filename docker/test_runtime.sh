#!/usr/bin/env bash

# Copyright 2024, IVEX NV
# All rights reserved.
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential

cd ..

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile \
  --target runtime_test \
  -t runtime_test .

docker run --rm --name runtime_test runtime_test 