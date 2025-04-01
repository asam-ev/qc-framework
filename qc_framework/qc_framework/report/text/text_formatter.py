# SPDX-License-Identifier: MPL-2.0
# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from __future__ import annotations
from typing import TYPE_CHECKING

from ..base import ReportFormatter
from qc_baselib import IssueSeverity
import xml.etree.ElementTree as ETree

if TYPE_CHECKING:
    from ..base.report_formatter import TStream, TParams
    from qc_baselib import Result


class TextFormatter(ReportFormatter):
    
    @property
    def module_name(self) -> str:
        return "qc-report-text"
    
    @property
    def default_output_file_param_value(self) -> str:
        return "Report.txt"
    
    @property
    def default_custom_parameters(self) -> "TParams":
        from os import get_terminal_size
        try:
            columns = get_terminal_size().columns
        except OSError:
            columns = 100
            
        return {
            "LineWidth": columns
        }

    def _dump(self, result: "Result", output: "TStream"):    
        columns = self.get_param("LineWidth")
        
        line_sep1 = ("=" * columns)
        line_sep2 = ("-" * columns)
        
        def p(val, indent=0):
            """
            Print with indent on the dump stream
            """
            if indent > 0:
                print(" " * indent, file=output, flush=True, end="")
            print(val, file=output, flush=True)
        
        def S(indent=0): 
            r"""
            Shortcut for chapter separator
            """
            p(line_sep1, indent)
        
        def s(indent=0):
            r"""
            Shortcut for section separator
            """
            p(line_sep2, indent)

        def n(indent=0):
            r"""
            Shortcut for empty line inclusion
            """
            p("", indent)
        
        addressed_rules = set()
        violated_rules = {
            IssueSeverity.ERROR: set(),
            IssueSeverity.WARNING: set(),
            IssueSeverity.INFORMATION: set(),
        }

        p(f"ASAM QUALITY FRAMEWORK v{result.get_result_version()} - RESULT REPORT")
        S()
        n()
        
        p("BUNDLES")
        s()
        n()
        
        bundles = result.get_checker_bundle_results()
        for bundle in bundles:
            p(f" -> {bundle.name} @ {bundle.version or 'version-undefined'}")
            p(f"Description: {bundle.description}", 4)
            p(f"Summary: {bundle.summary}", 4)
            if len(bundle.params):
                p(f"Parameters: ", 4)
                for param in bundle.params:
                    p(f" -> {param.name}: {param.value} [{type(param.value).__name__}]", 4)
            if len(bundle.checkers):
                p(f"Checkers: ", 4)
                for checker in bundle.checkers:
                    p(f" -> [{checker.status.name}] {checker.checker_id}", 4)
                    p(f"Description: {checker.description}", 8)
                    p(f"Summary: {checker.summary}", 8)
                    if len(checker.addressed_rule):
                        p("Addressed Rules:", 8)
                        for rule in checker.addressed_rule:
                            addressed_rules.add(rule.rule_uid)
                            p(f" -> {rule.rule_uid}", 8)
                    if len(checker.issues):
                        p("Issues:", 8)
                        for issue_no, issue in enumerate(checker.issues):
                            violated_rules[issue.level].add(issue.rule_uid)
                            p(f" {issue_no+1: 3d}. [{issue.level.name}] @ {issue.rule_uid}", 8)
                            p(f"Description: {issue.description}", 14)
                            if len(issue.domain_specific_info):
                                p("Domain Specific Info:", 14)
                                for dsi in issue.domain_specific_info:
                                    dsi_lines = ETree.tostring(dsi, encoding="unicode").split("\n")
                                    p(f" -> {dsi_lines[0].strip()}", 14)
                                    for dsi_line in dsi_lines[1:]:
                                        p(f"    {dsi_line.rstrip()}", 14)
                            if len(issue.locations):
                                p("Locations:", 14)
                                for location in issue.locations:
                                    for file_loc in location.file_location:
                                        p(f" -> File @ col: {file_loc.column}, row: {file_loc.row}: {location.description}", 14)
                                    for xml_loc in location.xml_location:
                                        p(f" -> XML @ `{xml_loc.xpath}`: {location.description}", 14)
                                    for inertial_loc in location.inertial_location:
                                        p(f" -> Inertial @ [{inertial_loc.x:.4n}; {inertial_loc.y:.4n}; {inertial_loc.z:.4n}]: {location.description}", 14)    
                    if len(checker.metadata):
                        p("Metadata:", 8)
                        for metadata in checker.metadata:
                            p(f" -> {metadata.key}: {metadata.value} ({metadata.description})", 8)
                    n()
            if bundle.name != bundles[-1].name:
                s()
            n()
        S()
        n()
        p("ADDRESSED RULES")
        s()
        n()
        p(f"Addressed rules: {len(addressed_rules)}")
        for r in addressed_rules:
            p(f" -> {r}")
        n()

        p(f"Flagged Rules: {sum((len(x) for x in violated_rules.values()))}")
        for t, rs in violated_rules.items():
            p(f"with {t.name}: {len(rs)}", 2)
            for r in rs:
                p(f" -> {r}", 2)

        s()
        n()
        p("NOTES")
        s()
        n()
        p("Rule UID format:")
        p("  <emanating-entity>:<standard>:x.y.z:rule_set.for_rules.rule_name")
        n()
        p("where    ")
        p("  * Emanating Entity: a domain name for the entity (organization or company) that declares the rule UID")
        p("  * Standard: a short string that represents the standard or the domain to which the rule is applied")
        p("  * Definition Setting: the version of the standard or the domain to which the rule appears or is applied for the first time")
        p("  * Rule Full Name: the full name of the rule, as dot separated, snake lower case string. ")
        p("    The full name of a rule is composed by the rule set, a categorization for the rule, ")
        p("    and the rule name, a unique string inside the categorization. ")
        p("    The rule set can be nested (meaning that can be defined as an ")
        p("    arbitrary sequence of dot separated names, while the name is the snake ")
        p("    case string after the last dot of the full name)")