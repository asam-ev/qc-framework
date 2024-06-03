# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

readonly OUTPUT_DIR=$1

cd ..

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile.linux \
  --target unit_test \
  -t unit_test .

docker run --rm \
  -v "$OUTPUT_DIR":/out \
  --name unit_test unit_test

