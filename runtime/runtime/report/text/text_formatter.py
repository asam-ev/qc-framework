# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from __future__ import annotations
from typing import TYPE_CHECKING, Dict, List, Any

import json
from ..base import ReportFormatter
from qc_baselib import IssueSeverity

if TYPE_CHECKING:
    from ..base.report_formatter import TStream
    from qc_baselib import Result

class TextFormatter(ReportFormatter):
    @property
    def module_name(self):
        return "report-text"
    
    @property
    def default_output_file_param_value(self) -> str:
        return "Report.txt"

    def _dump(self, result: "Result", output: "TStream"):
        output.write("Not yet implemented")