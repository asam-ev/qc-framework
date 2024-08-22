<!---
Copyright 2023 CARIAD SE.
 
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# Architecture

## High-level Requirements

The following requirement list was the foundation for the design of the
framework:

- Possibility to include a common ASAM rule set, which validates the rules from
  the specification document
- Write own rule sets, implemented in any programming language
- Extract meta information (e.g. if the file contain specific objects and the
  location of them)
- Configuration sets and parametrization of rules
- Possibility to include results based on the analysis of simulation runs
- Human and machine readable results
- Interactive usage or run in an automated workflow
- Locate the results in the XML source code and in a visualization of the map

## Terms / Definitions

- **Input File:** The file which should be checked by the Quality Checker Framework
- **Rule:** A textual description/definition about the input file
  characteristics.
- **Check/Checker:** A software module that tests rules compliance or creates
  statistical information about the input file. This can be a static or a
  dynamic test
- **Checker Bundle:** An executable or script that includes one or more
  Checkers. Checker Bundles allow checks to be better structured and divided
  into logical groups. In short they:
  - are a summary of several Checkers
    - with similar functions
    - from one supplier
    - developed in one distinct development environment/programming language
  - allow for files only be read/prepared once via parser
  - can be started via external scripting/automation
- **Result File:** Checker Bundles write their results in a XML based `*.xqar`
  file. See [File Formats Reference](file_formats.md#result-file-xqar) for
  details and examples
- **Report Module:** Output / Transmission / Visualization Results
- **Checker Library:** Collection of Checker Bundles and/or Report Modules for
  one domain

- **Quality Checker Framework**, that:
  - feeds the input file to the individual Checker Bundles
  - selects and configures the checker
  - summarizes the results (Result Pooling)
  - calls the report modules so that the user can view or further process the
    results
  - provides standard Checkers and standard report modules

### Relations between Terms / Definitions

```mermaid
flowchart LR
    framework[fa:fa-laptop-code Quality Checker Framework]
    checker_library_1["fa:fa-folder-open Checker Library A Directory"]
    checker_library_2["fa:fa-folder-open Checker Library B Directory"]

    checker_bundle_1_1[fa:fa-cubes Checker Bundle A1.exe/py]
    checker_bundle_1_2[fa:fa-cubes Checker Bundle A2.exe/py]
    checker_bundle_1_3[fa:fa-cubes Checker Bundle A3.exe/py]
    checker_bundle_2_1[fa:fa-cubes Checker Bundle B1.exe/py]
    checker_bundle_2_2[fa:fa-cubes Checker Bundle B2.exe/py]
    report_module_2_1[fa:fa-cubes Optional specific Report Module B3.exe/py]

    check_1_1_1[fa:fa-cube Check A1.1]
    check_1_1_2[fa:fa-cube Check A1.2]
    check_1_2_1[fa:fa-cube Check A2.1]
    check_1_3_1[fa:fa-cube Check A3.1]
    check_1_3_2[fa:fa-cube Check A3.2]
    check_1_3_3[fa:fa-cube Check A3.3]
    check_2_1_1[fa:fa-cube Check B1.1]
    check_2_1_2[fa:fa-cube Check B1.2]
    check_2_2_1[fa:fa-cube Check B2.1]

    rule_uid_1(fa:fa-scale-balanced Rule 1 from standard)
    rule_uid_2(fa:fa-scale-balanced Rule 2 from standard)
    rule_uid_3(fa:fa-scale-balanced Rule 3 from standard)
    rule_uid_4(fa:fa-scale-balanced Rule 4 from standard)
    rule_uid_5(fa:fa-scale-balanced Rule 5 from standard)
    rule_uid_6(fa:fa-scale-balanced Rule 6 from checker library)
    rule_uid_7(fa:fa-scale-balanced Rule 7 from checker library)
    rule_uid_8(fa:fa-scale-balanced Rule 8 from standard)
    rule_uid_9(fa:fa-scale-balanced Rule 9 from standard)
    rule_uid_10(fa:fa-scale-balanced Rule 10 from standard)
    rule_uid_11(fa:fa-scale-balanced Rule 11 from standard)
    rule_uid_12(fa:fa-scale-balanced Rule 12 from standard)

    framework --executes modules from--> checker_library_1
    framework --executes modules from--> checker_library_2
    
    checker_library_1 --contains --> checker_bundle_1_1
    checker_library_1 --contains --> checker_bundle_1_2
    checker_library_1 --contains --> checker_bundle_1_3

    checker_library_2 --contains--> checker_bundle_2_1
    checker_library_2 --contains--> checker_bundle_2_2
    checker_library_2 --contains--> report_module_2_1
    
    checker_bundle_1_1 --has --> check_1_1_1 --check --> rule_uid_1
    check_1_1_1 --check --> rule_uid_2
    checker_bundle_1_1 --has --> check_1_1_2 --check --> rule_uid_3
    checker_bundle_1_2 --has --> check_1_2_1 --check --> rule_uid_4
    checker_bundle_1_3 --has --> check_1_3_1 --check --> rule_uid_5
    checker_bundle_1_3 --has --> check_1_3_2 --check --> rule_uid_6
    checker_bundle_1_3 --has --> check_1_3_3 --check --> rule_uid_7
    check_1_3_3 --check --> rule_uid_8
    check_1_3_3 --check --> rule_uid_9

    checker_bundle_2_1 --has --> check_2_1_1 --check --> rule_uid_10
    checker_bundle_2_1 --has --> check_2_1_2 --check --> rule_uid_11
    checker_bundle_2_2 --has --> check_2_2_1 --check --> rule_uid_12
```

## Workflow ASAM Quality Checker Framework

![Architecture_Workflow](images/workflow.drawio.png)

## Properties / Requirements

- ASAM Quality Checker Framework
  - Runs on Windows & Linux, Local & Server
  - Configuration via XML file
    - Selection of Checker Bundles
    - Selection of individual Checkers in the bundles and their optional
      parameters → also defines the order in the overall report
    - Configuration which Checkers output what level (Info/Issue) → leads to
      overall result "red" or "yellow"
    - Selection of Report Modules
- Checker
  - See [Checks](checker_library.md#checks) in the chapter about Checker
    Libraries
- Checker Bundle
  - Runs probably only on certain platforms, because a third party product
    is not available for all platforms
- Result Pooling
  - Summarizes all results (overview and detailed view possible)
  - Gives each incident a unique Id → assignment results in different report
    modules
  - If necessary sorts and lists all information first and appends warnings and
    errors. Within these results, information is sorted by a central
    configuration. There is also the possibility for summarizing and filtering.
- Report Module
  - Runs maybe only on certain platforms (e.g. Web server or local graphical
    application)

## Parameterization and Sequence Control

A Checker Bundle and the included Checkers can be parameterized. A distinction
must be made between

- Globale Parameter
  - InputFile --> File to be validated by the Checker Bundles
- Parameter for the whole Checker Bundle --> "Checker Bundle Parameter"
  - Parameter that is relevant for one Checker Bundle
  - Can be used by different Checkers in this Checker Bundle
- Parameter for single Checkers --> "Checker Parameter"
