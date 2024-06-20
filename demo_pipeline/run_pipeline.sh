# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

python3 /app/demo_pipeline/configuration_generator.py

python3 /app/framework/runtime/runtime/runtime.py \
    --config "/tmp/generated_config/config.xml" \
    --install_dir "/app/framework/bin" \
    --schema_dir "/app/framework/doc/schema"

mkdir -p /out/qc-result-$INPUT_FILENAME
cp /app/framework/bin/*.xqar /out/qc-result-$INPUT_FILENAME
cp /app/framework/bin/*.txt /out/qc-result-$INPUT_FILENAME
chown -R $USER_ID:$GROUP_ID /out/qc-result-$INPUT_FILENAME
