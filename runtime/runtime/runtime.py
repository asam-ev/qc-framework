# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

import argparse
import os
import subprocess

from typing import List

from qc_baselib import Configuration

from runtime import models


FRAMEWORK_WORKING_DIR_VAR_NAME = "ASAM_QC_FRAMEWORK_WORKING_DIR"
FRAMEWORK_CONFIG_PATH_VAR_NAME = "ASAM_QC_FRAMEWORK_CONFIG_FILE"


def on_windows() -> bool:
    """Check if script is executed in Windows OS

    Returns:
        bool: True if executing the script on Windows
    """
    return os.name == "nt"


def run_module_command(module: models.Module, config_file_path: str) -> None:
    """Execute command specified in module configured with information in the
    provided configuration.

    Args:
        module (models.Module): module to be executed
        config (Configuration): xml configuration containing information for execution.
    """
    try:
        print(f"Executing command: {module.exec_command}")

        cmd_env = os.environ.copy()
        cmd_env[FRAMEWORK_WORKING_DIR_VAR_NAME] = os.getcwd()
        cmd_env[FRAMEWORK_CONFIG_PATH_VAR_NAME] = config_file_path

        process = subprocess.run(module.exec_command, capture_output=True, env=cmd_env)

        exit_code = process.returncode
        if exit_code == 0:
            print("Command executed successfully.")
            print("Output:")
            print(process.stdout.decode())
        else:
            print("Error occurred while executing the command.")
            print("Error message:")
            print(process.stderr.decode())
    except Exception as e:
        print(f"Error: {str(e)}")


def execute_modules(
    config_file_path: str,
    checker_bundles: List[models.Module],
    result_pooling: models.Module,
    report_modules: List[models.Module],
) -> None:
    # Checker bundles
    for checker_bundle in checker_bundles:
        run_module_command(checker_bundle, config_file_path)

    # Result pooling
    run_module_command(result_pooling, config_file_path)

    # Report modules
    for report_module in report_modules:
        run_module_command(report_module, config_file_path)


def execute_runtime(config_file_path: str, manifest_file_path: str) -> None:
    """Execute all runtime operations defined in the input manifest over the
    defined configuration.

    Args:
        config_file_path (str): input configuration xml file path
        manifest_file_path (str): input manifest json file path
    """

    config = Configuration()
    config.load_from_file(config_file_path)

    checker_bundles = []
    report_modules = []
    result_pooling = None

    with open(manifest_file_path, "rb") as framework_manifest_file:
        json_data = framework_manifest_file.read().decode()
        framework_manifest = models.FrameworkManifest.model_validate_json(json_data)

        for module_manifest_path in framework_manifest.manifest_file_path:
            with open(module_manifest_path, "rb") as module_manifest_file:
                json_data = module_manifest_file.read().decode()
                module_manifest = models.ModuleManifest.model_validate_json(json_data)

                for module in module_manifest.module:
                    if module.module_type == models.ModuleType.CHECKER_BUNDLE:
                        checker_bundles.append(module)
                    elif module.module_type == models.ModuleType.REPORT_MODULE:
                        report_modules.append(module)
                    elif module.module_type == models.ModuleType.RESULT_POOLING:
                        result_pooling = module
                    else:
                        raise RuntimeError(
                            f"Module of type {module.module_type.name} not supported."
                        )

    if result_pooling is None:
        raise RuntimeError(
            f"No result pooling module found in the provided framework manifest."
        )

    execute_modules(config_file_path, checker_bundles, result_pooling, report_modules)


def main():
    parser = argparse.ArgumentParser(description="Process XML configuration file.")
    parser.add_argument(
        "--config",
        type=str,
        help="Path to the XML configuration file",
        required=True,
    )
    parser.add_argument(
        "--manifest",
        type=str,
        help="Path to the JSON manifest file",
        required=True,
    )

    args = parser.parse_args()

    execute_runtime(args.config, args.manifest)


if __name__ == "__main__":
    main()
