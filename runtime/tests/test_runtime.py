# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

import pytest
import os
import sys


import runtime.runtime as runtime


def launch_main(monkeypatch, config_file_path: str, manifest_file_path: str):
    monkeypatch.setattr(
        sys,
        "argv",
        ["main.py", f"--config={config_file_path}", f"--manifest={manifest_file_path}"],
    )
    runtime.main()


def test_3steps_manifest(monkeypatch):
    cwd = os.getcwd()
    config_xml = os.path.join(cwd, "tests", "test_data", "3step_config.xml")

    manifest_json = os.path.join(cwd, "tests", "test_data", "framework_manifest.json")

    launch_main(monkeypatch, config_xml, manifest_json)

    checker_bundle_output_generated = False
    result_xqar_generated = False
    report_txt_generated = False

    for _, _, files in os.walk("./"):
        for file in files:
            if "DemoCheckerBundle.xqar" in file:
                checker_bundle_output_generated = True
            if "Result.xqar" in file:
                result_xqar_generated = True
            if "Report.txt" in file:
                report_txt_generated = True

    # Check that checker bundle output file is correctly generated
    assert checker_bundle_output_generated == True
    # Check that result file is correctly generated
    assert result_xqar_generated == True
    # Check that report txt file is correctly generated
    assert report_txt_generated == True
