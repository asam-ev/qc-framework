#!/usr/bin/env bash

# Copyright 2023, IVEX NV
# All rights reserved.
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential


cp -r ../doc .
DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile \
  --target runtime \
  -t runtime .

rm -rf doc