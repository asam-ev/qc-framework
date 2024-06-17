# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

python3 /app/demo_pipeline/configuration_generator.py

python3 /app/framework/runtime/runtime/runtime.py \
    --config "/tmp/generated_config/config.xml" \
    --install_dir "/app/framework/build/bin" \
    --schema_dir "/app/framework/doc/schema"

rm -rf /out/$INPUT_FILENAME
mkdir -p /out/$INPUT_FILENAME
cp /app/framework/build/bin/*.xqar /out/$INPUT_FILENAME
cp /app/framework/build/bin/*.txt /out/$INPUT_FILENAME
