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
- A rule should be part of a standard, as it is today in many ASAM standards.
- Rules can come from ASAM, but also from other organizations or can be company
  internal rules.
- Rules may be documented in the Checker Library if they are not explicitly
  written in the corresponding standard. The documentation can be used as a
  proposal to the standardization project to improve the standard itself.
- A rule may have a Rule Version, a counter value representing the current version of the rule
  content. If content of the rule changes without changing the underlying logic
  for validation, this counter increases. If the change in the rule forces a
  modification in the validation logic, the change shall be treated as a new
  rule.

### Rule Documentation Template

If a Checker Library needs to define and document own rules, then the following
template can be used.

**Rule ID:** _Unique identifier according the [Rule UID
Schema](rule_uid_schema.md)_

**Description:** _Clear textual description of the rule._

**Severity:** _Indicates the rule's importance level as asserted by the
standard (e.g., respecting the rule is mandatory or a recommendation)_

**Version range:** _Information for which versions of the standard this rule
applies._

## Checks

### Check Characteristics

- A check should address exactly one rule, 
  unless there is a strong reason not to. In exceptional cases, 
  a check may address multiple rules, or multiple checks may be 
  required to check a particular rule.
- Each check shall have a minimum of two example input files. The first
  demonstrating a rule violation and the second showcasing a successful rule
  validation.

### Check Documentation Template

**Check ID:** _The ID shall be unique within the Checker Library and
self-explanatory as possible._

**Version:** _The check itself has also a version number for identifying
changes in the results, which are based on implementation changes._

**Description:** _Clear textual description of the check._

**Parameters:** _Description of the parameters of this check. For a
well-defined standard, there should be no parameter necessary. It's often the
case that some are needed._

**Addressed Rules:** _Link (see [Rule UID Schema](rule_uid_schema.md)) to the
standard and/or to rule definitions from the Checker Library, if no appropriate
rules are explicitly written in the standard._

**Metadata:** _A check may log arbitrary metadata during execution (e.g., for
traceability). The metadata produced by a check shall be included and described
in its documentation, (e.g., if a check logs in metadata version and
configuration of a simulator, the checker documentation shall describe the
content of aforementioned metadata fields)._

All information are part of the Result File (`*.xqar`). So, the documentation
of the Checker Library containing the list of implemented checks and their
details can be generated automatically after implementation of the Checkers.
