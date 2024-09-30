# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


class Constants:
    MODULE_NAME = "qc-report-json"
    DEFAULT_INPUT_RESULT_EXTENSION = ".xqar"
    DEFAULT_INPUT_CONFIG_EXTENSION = ".xml"
    DEFAULT_INPUT_RESULT_FILENAME = f"Result{DEFAULT_INPUT_RESULT_EXTENSION}"
    DEFAULT_INPUT_CONFIG_FILENAME = f"Config{DEFAULT_INPUT_CONFIG_EXTENSION}"
    DEFAULT_OUTPUT_CONFIG_FILENAME = f"{MODULE_NAME}{DEFAULT_INPUT_CONFIG_EXTENSION}"
    DEFAULT_OUTPUT_REPORT_FILENAME = f"{MODULE_NAME}.json"
    DEFAULT_INDENT_LEVEL = 2
    # Parameters key names for the tool
    CONFIG_INPUT_FILE_PARAM = "InputFile"
    CONFIG_OUTPUT_FILE_PARAM = "ReportFile"
    CONFIG_INDENT_PARAM = "Indent"
