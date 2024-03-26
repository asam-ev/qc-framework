<!---
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# Checker Library

## Introduction

A Checker Library is a collection of modules for one domain. Typically, it
contains a set of checks for one standard or to test the input file for one
specific Use Case or in one specific environment. The checks may be distributed
in multiple Checker Bundles in the Checker Library.

Each Checker Library needs to have a documentation what is included. Here we
have to distinguish between "Checks" and "Rules". A standard specify rules,
which are described in textual form. A check is a test implementation which
inspects the input document to verify if it is compliant to the standard.

## Rules

### Rule Characteristics

- Rules are atomic, easy understandable and independent of how many rules are
  necessary in the standard.
- A Rule should be part of a standard, as it is today in many ASAM standards.
- Rules can come from ASAM, but also from other organizations or can be company
  internal rules.
- Rules may be documented in the Checker Library if they are not explicitly
  written in the corresponding standard. The documentation can be used as a
  proposal to the standardization project to improve the standard itself.

### Rule Documentation Template

If a Checker Library needs to define and document own rules, then the following
template can be used.

**Rule ID:** _Unique identifier according the [Rule UID
Schema](rule_uid_schema.md)_

**Description:** _Clear textual description of the rule._

**Severity:** _Indicates the rule's importance level as asserted by the
standard (e.g., respecting the rule is mandatory or a recommendation)_

**Version range:** _Information for which versions of the standard this rule applies._

## Checks

### Check Characteristics

- A check should addresses exactly one rule. In reality, a check can address  
  several rules, or several checks might be necessary to check a certain rule.  
- Each check shall have a minimum of two example input files. The first
  demonstrating a rule violation and the second showcasing a successful rule
  validation.

### Check Documentation Template

**Name:** _The name shall be a unique ID within the Checker Library and
self-explanatory as possible._

**Version:** _The check itself has also a version number for identifying
changes in the results, which are based on implementation changes._

**Description:** _Clear textual description of the check._

**Parameters:** _Description of the parameters of this check. For a
well-defined standard, there should be no parameter necessary. It's often the
case that some are needed._

**Addressed Rules:** _Link to the standard and/or to rule definitions from the
Checker Library, if no appropriate rules are explicitly written in the
standard._

**Version range:** _Information how this check is implemented for which
versions._

**Approval Information:** _Who has approved and when was this check approved.
This may be empty if the check is not approved yet._

**Metadata:** _A check may log arbitrary metadata during execution (e.g., for
traceability). The metadata produced by a check shall be included and
described in its documentation, (e.g., if a check logs in metadata version and
configuration of a simulator, the checker documentation shall describe the
content of aforementioned metadata fields)._

## Manifest File

Each Checker Library needs to provide a manifest file with configuration
information for the framework. The format of this file is aligned with the one
for the [Result File (`*.xqar`)](file_formats.md#result-file-xqar) and follows
the same specification. The only difference is that it may contain additional
Report Modules, which are delivered as part of the Checker Library.

### Example

```xml
<CheckerResults version="1.0.0">
    <CheckerBundle name="CheckerBundle1" description="Description of the Checker Bundle">
        <Executable program="/usr/bin/python3"> <!-- apply to Linux only! Probably just "python"? -->
            <Arg>my_fancy_script.py</Arg> <!-- Mandatory arguments, followed by the configuration.xml or input file-->
            <Arg>--fancy-param=True</Arg>
        </Executable>
        <Param name="InputFile" value="myTrack.xodr"/> <!-- Global standard parameter for each module! -->
        <Param name="CheckerBundleParameter" value="0.1"/>
        <Checker checkerId="XsdChecker" description="Checks the schema validity">
            <AddressedRule uid="asam.net:qc:1.0.0:xsd_schema_validity">
            <MetaData name="XmlParser" value="Xerces-C++ 1.2.3"> <!-- Or part of CheckerBunde? Or both possible like parameter? -->
        </Checker>
        <Checker checkerId="RefLineChecker" description="Checks the reference line integrity">
            <AddressedRule uid="asam.net:xodr:1.6.0:road.planview.geometry.ref_line_exists">
            <AddressedRule uid="example.com:::custom_rule">
            <Param name="CheckerParameter" value="123"/> 
        </Checker>
    </CheckerBundle>
    <ReportModule name="ReportModule" description="Description of the Report Module">
        <Executable program="text_report"/> <!-- standard executable without additional arguments, except the configuration.xml or result file -->
        <Param name="InputFile" value="myTrack.xodr"/> <!-- Global standard parameter for each module! -->
    </ReportModule>    
</CheckerResults>
```

### Open Questions

- How does the framework find this Manifest file and the appropriate Checker
  Bundles and Report Modules?
- Is a `<ReportModule/>` from the technical point-of-view the same as a
  `<CheckerBundle/>` just without Checkers? Use the same element?
  --> rename to `<Module/>`?
- Path of executable?
  - (bin?) directory of Checker Library
  - can be found in $PATH
  - global path ;-?
- (...)
