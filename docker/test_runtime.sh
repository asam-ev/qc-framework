# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

cd ..

DOCKER_BUILDKIT=1 \
  docker build \
  -f docker/Dockerfile \
  --target runtime_test \
  -t runtime_test .

docker run --rm --name runtime_test runtime_test 

