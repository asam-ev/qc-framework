# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

python3 /app/demo_pipeline/configuration_generator.py

OUTPUT_DIR=/out/qc-result-$INPUT_FILENAME

qc_runtime \
    --config "/tmp/generated_config/config.xml" \
    --manifest "/app/demo_pipeline/manifests/framework_manifest.json" \
    --working_dir "$OUTPUT_DIR"

chown -R $USER_ID:$GROUP_ID $OUTPUT_DIR
