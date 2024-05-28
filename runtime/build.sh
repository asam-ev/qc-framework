#!/usr/bin/env bash

# Copyright 2024, IVEX NV
# All rights reserved.
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential

mkdir -p framework
cp -r ../examples ./framework/
cp -r ../include ./framework/
cp -r ../scripts ./framework/
cp -r ../src ./framework/
cp -r ../test ./framework/
cp -r ../doc ./framework/
cp -r ../CMakeLists.txt ./framework/CMakeLists.txt
cp -r ../version ./framework/version
cp -r ../licenses ./framework/licenses

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile \
  --target runtime \
  -t runtime .

rm -rf framework