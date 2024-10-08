# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from __future__ import annotations
from typing import TYPE_CHECKING

import json
from ..base import ReportFormatter
from qc_baselib import IssueSeverity

if TYPE_CHECKING:
    from ..base.report_formatter import TStream
    from qc_baselib import Result

class GithubCIFormatter(ReportFormatter):
    def __init__(self, *args, **kwargs):
        self._has_issue = False
        super().__init__(*args, **kwargs)

    @property
    def module_name(self):
        return "qc-report-github-ci"
    
    @property
    def default_output_file_param_value(self) -> str:
        return "Report.txt"
    
    @property
    def has_issue(self) -> bool:
        return self._has_issue

    def _dump(self, result: "Result", output: "TStream"):
        self._has_issue = False

        for bundle in result.get_checker_bundle_results():
            for checker in bundle.checkers:
                for issue in checker.issues:
                    issue_level = issue.level.name.lower()
                    bundle_name = bundle.name
                    checker_id = checker.checker_id
                    rule_uid = issue.rule_uid
                    description = issue.description

                    report_line = f"{issue_level}: {bundle_name} / {checker_id} / {rule_uid}: {description}\n"
                    output.write(report_line)

                    if issue.level == IssueSeverity.ERROR:
                        self._has_issue = True
