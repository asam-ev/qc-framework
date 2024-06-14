# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

readonly USER_INPUT_FILE=$1
readonly OUTPUT_PATH=$2

INPUT_DIRECTORY=$(dirname "$USER_INPUT_FILE")
INPUT_NAME=$(basename "$USER_INPUT_FILE")

cd ..

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile.linux \
  --target demo_pipeline \
  -t demo_pipeline .

#  docker run \
#  -v $INPUT_DIRECTORY:/input_path \
#   -v "$OUTPUT_PATH":/out \
#   -e INPUT_NAME=$INPUT_NAME \
#  --rm  --name demo_pipeline demo_pipeline


 docker run \
 -v $INPUT_DIRECTORY:/input_path \
 -v $OUTPUT_PATH:/out \
 -it \
 --rm  --name demo_pipeline --entrypoint /bin/bash demo_pipeline
