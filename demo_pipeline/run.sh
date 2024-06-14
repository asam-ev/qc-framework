# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

readonly INPUT_FILE=$1
readonly OUTPUT_PATH=$2

INPUT_DIRECTORY=$(dirname "$INPUT_FILE")
INPUT_FILENAME=$(basename "$INPUT_FILE")

cd ..

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile.linux \
  --target demo_pipeline \
  -t demo_pipeline .

 docker run \
  -v $INPUT_DIRECTORY:/input_path \
  -v "$OUTPUT_PATH":/out \
  -e INPUT_FILENAME=$INPUT_FILENAME \
 --rm --name demo_pipeline demo_pipeline
