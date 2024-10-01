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
    from qc_baselib.models.common import ParamType
    from qc_baselib.models.result import CheckerType, MetadataType, RuleType, IssueType


class JsonFormatter(ReportFormatter):
    @property
    def module_name(self):
        return "report-json"
    
    @property
    def default_custom_parameters(self):
        return {"Indent": 2}
    
    @property
    def default_output_file_param_value(self) -> str:
        return "Report.json"

    def _dump(self, result: "Result", output: "TStream"):
        indent = int(self.get_param("Indent"))  # type: ignore
        self.result = result

        self._addressed_rule_uids = set()
        self._violated_rule_uids = {
            IssueSeverity.ERROR: set(),
            IssueSeverity.WARNING: set(),
            IssueSeverity.INFORMATION: set(),
        }

        dump_ = {
            "version": self.result.get_result_version(),
            "bundles": self._dump_checker_bundle_results(),
            "rules": self._dump_addressed_and_violated_rules()
        }

        json.dump(dump_, output, indent=indent)
        
    def _dump_addressed_and_violated_rules(self) -> Dict[str, Any]:
        return {
            "addressed": list(self._addressed_rule_uids),
            "violated": dict(((key.name.lower(), list(value)) for key, value in self._violated_rule_uids.items()))
        }

    def _dump_checker_bundle_results(self) -> Dict[str, Any]:
        result = {}
        for bundle in self.result.get_checker_bundle_results():
            result[bundle.name] = {
                "name": bundle.name,
                "version": bundle.version,
                "summary": bundle.summary,
                "description": bundle.description,
                "buildDate": bundle.build_date,
                "parameters": self._dump_params(bundle.params),
                "checkers": self._dump_checkers(bundle.checkers),
            }
        return result

    def _dump_params(self, params: List["ParamType"]) -> Dict[str, Dict[str, Any]]:
        return dict(((p.name, {"name": p.name, "value": p.value}) for p in params))  # type: ignore

    def _dump_checkers(self, checkers: List["CheckerType"]) -> Dict[str, Any]:
        result = {}
        for checker in checkers:
            result[checker.checker_id] = {
                "checkerId": checker.checker_id,
                "description": checker.description,
                "summary": checker.summary,
                "status": str(checker.status.value),
                "metadata": self._dump_metadata(checker.metadata),
                "addressedRule": self._dump_addressed_rule(checker.addressed_rule),
                "issues": self._dump_issues(checker.issues),
            }
        return result

    def _dump_metadata(
        self, metadata: List["MetadataType"]
    ) -> Dict[str, Dict[str, Any]]:
        return dict(
            (m.key, {"key": m.key, "value": m.value, "description": m.description})
            for m in metadata
        )

    def _dump_addressed_rule(self, addressed_rule: List["RuleType"]) -> List[str]:
        result = set((r.rule_uid for r in addressed_rule))
        self._addressed_rule_uids = self._addressed_rule_uids.union(result)
        return list(result)

    def _dump_issues(self, issues: List["IssueType"]) -> List[Dict[str, Any]]:
        result = []
        for issue in issues:
            self._violated_rule_uids[issue.level].add(issue.rule_uid)
            result.append(
                {
                    "issueId": issue.issue_id,
                    "level": issue.level.name,
                    "description": issue.description,
                    "ruleUid": issue.rule_uid,
                    "locations": self._dump_locations(issue.locations),
                    "domainSpecificInfo": self._dump_domain_specific_info(
                        issue.domain_specific_info
                    ),
                }
            )
        return result

    def _dump_locations(self, locations):
        file_locations = []
        xml_locations = []
        inertial_locations = []

        for location in locations:
            for file_location in location.file_location:
                file_locations.append(
                    {
                        "description": location.description,
                        "column": file_location.column,
                        "row": file_location.row,
                    }
                )
            for xml_location in location.xml_location:
                xml_locations.append(
                    {
                        "description": location.description,
                        "xpath": xml_location.xpath,
                    }
                )
            for inertial_location in location.inertial_location:
                inertial_locations.append(
                    {
                        "description": location.description,
                        "x": inertial_location.x,
                        "y": inertial_location.y,
                        "z": inertial_location.z,
                    }
                )
        return {
            "file": file_locations,
            "xml": xml_locations,
            "inertial": inertial_locations,
        }

    def _dump_domain_specific_info(self, domain_specific_info):
        return [dsi.name for dsi in domain_specific_info]
