# SPDX-License-Identifier: MPL-2.0
# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from __future__ import annotations

from argparse import ArgumentParser
import datetime
import os
import subprocess

from typing import TYPE_CHECKING, Dict

from qc_baselib import Configuration
from .models import FrameworkManifest, ModuleManifest, ModuleType

if TYPE_CHECKING:
    from .models import Module


FRAMEWORK_WORKING_DIR_VAR_NAME = "ASAM_QC_FRAMEWORK_WORKING_DIR"
FRAMEWORK_CONFIG_PATH_VAR_NAME = "ASAM_QC_FRAMEWORK_CONFIG_FILE"


def run_module_command(
    module: "Module", config_file_path: str, working_dir: str
) -> None:
    r"""
    Execute command specified in module configured with information in the
    provided configuration.

    Args:
        module: module to be executed
        config_file_path: defines the configuration file path
        working_dir: defines the working directory
    """
    try:
        print(f"Executing command: {os.path.expandvars(module.exec_command)}")

        cmd_env = os.environ.copy()
        cmd_env[FRAMEWORK_WORKING_DIR_VAR_NAME] = working_dir
        cmd_env[FRAMEWORK_CONFIG_PATH_VAR_NAME] = config_file_path

        process = subprocess.run(
            module.exec_command,
            env=cmd_env,
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )

        exit_code = process.returncode
        if exit_code == 0:
            print("Command executed successfully.")
            print("Output:")
        else:
            print("Error occurred while executing the command.")
            print("Error message:")

        print(process.stdout.decode())

    except Exception as e:
        print(f"Error: {str(e)}")


def execute_modules(
    config_file_path: str,
    available_checker_bundles: Dict[str, "Module"],
    available_result_pooling: "Module",
    available_report_modules: Dict[str, "Module"],
    working_dir: str,
) -> None:
    r"""
    Execute the full sequence of operations, checker bundles, result
    pooling and result reporters as required as defined by the configuration file.
    Each module needs to be declared via manifest file.

    Args:
        config_file_path: path of the current configuration file
        available_checker_bundles: available bundle modules as declared in manifests
        available_result_pooling: result pooler module
        available_report_modules: available report modules as declared in manifests
        working_dir: working directory to be used for current execution
    """
    config = Configuration()
    config.load_from_file(config_file_path)

    checker_bundles = []
    report_modules = []

    for bundle in config.get_all_checker_bundles():
        name = bundle.application
        if name not in available_checker_bundles:
            raise RuntimeError(
                f"Checker bundle {name} is not available in the framework manifest."
            )

        checker_bundles.append(available_checker_bundles[name])

    for report in config.get_all_report_modules():
        name = report.application
        if name not in available_report_modules:
            raise RuntimeError(
                f"Report module {name} is not available in the framework manifest."
            )

        report_modules.append(available_report_modules[name])

    # Checker bundles
    print(f"Executing checker bundles")
    for checker_bundle in checker_bundles:
        print(f"Executing checker module:  {checker_bundle.name}")
        run_module_command(checker_bundle, config_file_path, working_dir)

    # Result pooling
    print(f"Executing result pooling:  {available_result_pooling.name}")
    run_module_command(available_result_pooling, config_file_path, working_dir)

    # Report modules
    print(f"Executing report modules")
    for report_module in report_modules:
        print(f"Executing report module:  {report_module.name}")
        run_module_command(report_module, config_file_path, working_dir)


def execute_runtime(
    config_file_path: str, manifest_file_path: str, working_dir: str
) -> None:
    r"""
    Execute all runtime operations defined in the input manifest over the
    defined configuration.

    Args:
        config_file_path: input configuration xml file path
        manifest_file_path: input manifest json file path
        working_dir: working directory
    """

    checker_bundles = {}
    report_modules = {}
    result_pooling = None

    with open(manifest_file_path, "rb") as framework_manifest_file:
        json_data = framework_manifest_file.read().decode()
        framework_manifest = FrameworkManifest.model_validate_json(json_data)

        for raw_module_manifest_path in framework_manifest.manifest_file_path:
            resolved_module_manifest_path = os.path.expandvars(raw_module_manifest_path)
            print(f"Registering manifest file at {resolved_module_manifest_path}")

            with open(resolved_module_manifest_path, "rb") as module_manifest_file:
                json_data = module_manifest_file.read().decode()
                module_manifest = ModuleManifest.model_validate_json(json_data)

                for module in module_manifest.module:
                    if module.module_type == ModuleType.CHECKER_BUNDLE:
                        if module.name in checker_bundles:
                            raise RuntimeError(
                                f"{module.name} is used by multiple checker bundles on framework manifest."
                            )
                        checker_bundles[module.name] = module
                    elif module.module_type == ModuleType.REPORT_MODULE:
                        if module.name in report_modules:
                            raise RuntimeError(
                                f"{module.name} is used by multiple report modules on framework manifest."
                            )
                        report_modules[module.name] = module
                    elif module.module_type == ModuleType.RESULT_POOLING:
                        if result_pooling is not None:
                            raise RuntimeError(
                                "Multiple result pooling modules defined on framework manifest. There must be exactly one result pooling module defined."
                            )
                        result_pooling = module
                    else:
                        raise RuntimeError(
                            f"Module of type {module.module_type.name} not supported."
                        )

    if result_pooling is None:
        raise RuntimeError(
            f"No result pooling module found in the provided framework manifest. There must be exactly one result pooling module defined."
        )

    execute_modules(
        config_file_path,
        checker_bundles,
        result_pooling,
        report_modules,
        working_dir,
    )


def main() -> None:
    r"""
    Runtime command line application
    """
    parser = ArgumentParser(
        prog="qc_runtime",
        description="Executes checker bundles for quality checking via configuration file",
        epilog="Quality Framework, Copyright 2024, ASAM e.V.: License MPLv2",
    )
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

    parser.add_argument(
        "--working_dir",
        type=str,
        help="Working directory where all the output files are generated.",
        required=False,
    )

    args = parser.parse_args()

    working_dir = None
    if args.working_dir is None:
        formatted_now = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M_%S_%f")
        working_dir = os.path.join(os.getcwd(), f"qc-output-{formatted_now}")
    else:
        working_dir = os.path.abspath(args.working_dir)

    os.makedirs(working_dir, exist_ok=True)

    execute_runtime(args.config, args.manifest, working_dir)


if __name__ == "__main__":
    main()
