# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from __future__ import annotations
from typing import TYPE_CHECKING, Optional, Union, IO, Dict, List, Any


from qc_baselib import IssueSeverity

if TYPE_CHECKING:
    from qc_baselib import Result
    from qc_baselib.models.common import ParamType
    from qc_baselib.models.result import CheckerType, MetadataType, RuleType, IssueType

from .constants import Constants


class JsonFormatter:
    r"""
    The class handles the dump of a ``Result`` obect as a valid json object.
    
    .. warning:: The class is not thread safe. Use a new object per 
                 thread to make conversions.
                 
    .. info:: The class converts up to the information present in the 
              ``Result`` class. This may lead to a schema of the output
              which is slightly different with respect to the one 
              supported by the c++ version of the JSON formatter.
    
    Example usage::
    
        json_formatter = JsonFormatter()
        # Dump in a string
        json_string = json_formatter.dump(result)
        # Dump in a new file
        json_string = json_formatter.dump(result, "report.json")
        # Write to an existing stream
        with open("report.json", "w") as fp:
            json_string = json_formatter.dump(result, fp)
    
    :param indent: optional indentation level for the output.
                   Set to 0 for a minified output
    """

    def __init__(self, indent: Optional[int] = None):
        self.indent = indent or Constants.DEFAULT_INDENT_LEVEL
        self.indent = int(self.indent)  # Forcing to be an integer. May raise

    def dump(
        self, result: "Result", output: Optional[Union[IO, str]] = None
    ) -> Optional[str]:
        r"""
        Dumps the content of a result file in a json format. Input arguments
        controls the data emission strategy.
        
         * if ``output`` parameter is ``None``, a json string is returned by the 
           method
         * if ``ouptut`` parameter is a ``str``, the result is written in a new
           file with ``output`` as filename
         * if ``output`` is an ``IO`` compatible object (stream), the result is
           written directly to the stream.
           
        :param result: the result object to dump
        :param output: the 
        """
        import json
        import io

        # Setup
        self.result = result

        self._addressed_rule_uids = set()
        self._violated_rule_uids = {
            IssueSeverity.ERROR: set(),
            IssueSeverity.WARNING: set(),
            IssueSeverity.INFORMATION: set(),
        }

        # Execute
        dump_ = self._dump()

        # Export to output
        if output is None:
            return json.dumps(dump_, indent=self.indent)

        if isinstance(output, io.IOBase):
            json.dump(dump_, output, indent=self.indent)
            return

        if isinstance(output, str):
            with open(output, "w") as fp:
                json.dump(dump_, fp, indent=self.indent)

    def _dump(self) -> Dict[str, Any]:
        return {
            "version": self.result.get_result_version(),
            "bundles": self._dump_checker_bundle_results(),
            "rules": self._dump_addressed_and_violated_rules()
        }
        
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
