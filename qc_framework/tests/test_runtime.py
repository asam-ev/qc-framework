# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

import pytest
import os
import sys


import qc_framework.runtime as runtime


def on_windows() -> bool:
    """Check if script is executed in Windows OS

    Returns:
        bool: True if executing the script on Windows
    """
    return os.name == "nt"


def launch_main(monkeypatch, config_file_path: str, manifest_file_path: str):
    monkeypatch.setattr(
        sys,
        "argv",
        ["main.py", f"--config={config_file_path}", f"--manifest={manifest_file_path}"],
    )
    runtime.main()


def print_file(filename, separators=True):
    if os.environ.get("QC_FRAMEWORK_PRINT_FILES"):
        if separators:
            length = len(filename) + 2 + 6
            print("=" * length)
            print(f" BEGIN {filename} ")    
            print("=" * length)
        
        with open(filename) as fp:
            print(fp.read())

        if separators:
            length = len(filename) + 2 + 4
            print("=" * length)
            print(f" END {filename} ")    
            print("=" * length)


# The test expects the binaries configured in the `3step_config.xml` are present
# in the `../bin` relative path as configured in the
# `test_data/framework_manifest.json`
def test_3steps_manifest(monkeypatch):
    cwd = os.getcwd()
    config_xml = os.path.join(cwd, "tests", "test_data", "3step_config.xml")

    os_path = "linux"
    if on_windows():
        os_path = "windows"

    manifest_json = os.path.join(
        cwd, "tests", "test_data", os_path, "framework_manifest.json"
    )

    launch_main(monkeypatch, config_xml, manifest_json)

    checker_bundle_output_generated = False
    result_xqar_generated = False
    report_txt_generated = False

    for dirpath, _, files in os.walk("./"):
        for file in files:
            if "DemoCheckerBundle.xqar" in file:
                checker_bundle_output_generated = True
                print_file(f"{dirpath}/DemoCheckerBundle.xqar")
            if "Result.xqar" in file:
                result_xqar_generated = True
                print_file(f"{dirpath}/Result.xqar")
            if "Report.txt" in file:
                report_txt_generated = True
                print_file(f"{dirpath}/Report.txt")

            if "Report.json" in file:
                print_file(f"{dirpath}/Report.json")

    # Check that checker bundle output file is correctly generated
    assert checker_bundle_output_generated == True
    # Check that result file is correctly generated
    assert result_xqar_generated == True
    # Check that report txt file is correctly generated
    assert report_txt_generated == True
