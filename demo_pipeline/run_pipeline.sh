cd /app/framework/build/bin

python3 /app/demo_pipeline/configurator_generator.py

python3 /app/framework/runtime/runtime/runtime.py \
    --config "/app/demo_pipeline/generated_config/config.xml" \
    --install_dir  "/app/framework/build/bin" \
    --schema_dir   "/app/framework/doc/schema"

rm -rf /out/$INPUT_FILENAME
mkdir -p /out/$INPUT_FILENAME
cp /app/framework/build/bin/*.xqar /out/$INPUT_FILENAME
cp /app/framework/build/bin/*.txt /out/$INPUT_FILENAME
