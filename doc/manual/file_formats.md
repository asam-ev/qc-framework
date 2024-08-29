<!---
Copyright 2023 CARIAD SE.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# File Formats

## Configuration File (`*.xml`)

The runtime configuration settings are stored in a configuration file. This
file defines which Checker Bundles and what checkers are used, how they are
parameterized and whether the issues are warnings or errors. If a CheckerBundle
outputs errors that are not configured in this file, the result pooling removes
them later on. This way only the relevant results are included in the overall
result. Furthermore, it can be configured which Report Modules are started after
all Checker Bundles finish execution.

```xml
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<Config>

    <Param name="InputFile" value="test.xodr" />

    <CheckerBundle application="xodrBundle">
        <Param name="resultFile" value="xodr_bundle_report.xqar" />
        <Checker checkerId="semantic_xodr" maxLevel="1" minLevel="3" />
        <Checker checkerId="geometry_xodr" maxLevel="1" minLevel="3" />
        <Checker checkerId="performance_xodr" maxLevel="1" minLevel="3" />
        <Checker checkerId="smoothness_xodr" maxLevel="1" minLevel="3" />
    </CheckerBundle>

    <ReportModule application="TextReport">
        <Param name="strInputFile" value="Result.xqar" />
        <Param name="strReportFile" value="Report.txt" />
    </ReportModule>

</Config>
```

The runtime parses this file, then executes the configured Checker Bundles, and
hands them the configuration file. The parameter "application" to the XML tags
`<CheckerBundle/>` and `<ReportModule/>` specifies the name of the executable
to be used.

**NOTE ABOUT FILE NAMES IN CONFIGURATION FILE** 

The name `Result.xqar` is reserved for the output of the Result Pooling module. The framework automatically created the `Result.xqar` file in each execution. Therefore:
* The name `Result.xqar` **MUST NOT** be used as the name of the result `.xqar` file for any Checker Bundle.
* The file `Result.xqar` can be used as the input file for Report Modules.
* The result file of each checker bundle must have the postfix `.xqar`. It must be a file name (e.g., `my-bundle-result.xqar`) and must not contain any path (both asolute path and relative path are not allowed).
* All the result files and the automatically generated `Result.xqar` will be stored in the in the output folder `qc-output-YYYY-MM-DD-HH-MM-SS-*`. It is also possible to configure the output folder path. For more details see the [Runtime Module documentation](runtime_module.md).

## Result File (`*.xqar`)

The results of a checker are stored persistently as XML in the form of a result
file (*.xqar). The [Base library](cpp_base_library.md) can be used to write a
report. A report consists of problems (called issues) which contain syntactic
or semantic flaws.

- An issue consists of the following parts:
  - Identifier: Unique number for an issue
  - Level: Indicates whether it is an information, a warning, or an error.
  - Description: Description in the form of a text
  - Optional description of where the problem lies, if applicable several
    grouped locations inside a **Locations** tag:
    - **FileLocation** _(typically generated automatically by the
      ResultPooling)_
      - A reference to a file with row and column
      - Example: `<FileLocation column="0" row="223124"/>`
    - **XmlLocation**
      - Addressing in a XML file with help of a XPath expression
      - Example: `<XMLLocation xpath="/OpenDRIVE/road[@id=&quot;1&quot;]"/>`
    - **InertialLocation**
      - Position in inertial coordinates
      - Example: `<InertialLocation x="120.998703"
        y="0.0" z="-0.29552"/>`
  - Optional external files (e. g. Images of generated graphs such as speed
    over distance). Currently not supported.

The following example shows the results obtained by running two Checker Bundles,
one called SyntaxChecker and one SemanticChecker.

```xml
<CheckerResults version="1.0.0">
    <CheckerBundle name="SyntaxChecker" description="Syntax checker bundle" summary="Found 1 incident" build_date="23.05.2019" version="1.0">
        <Param name="InputFile" value="myTrack.xodr"/>
        <Param name="globalParameter" value="0.1"/>
        <Checker checkerId="xsdSchemaChecker" description="Checks the xsd validity"  summary="Found 1 issue">
            <Issue description="Row:8693 Column:12 invalid schema" issueId="1" level="1">
                <Locations description="empty content is not valid for content model '(lane+,userData*,include*)'">
                    <FileLocation column="12" row="8693"/>
                </Locations>
            </Issue>
        </Checker>
    </CheckerBundle>
    <CheckerBundle name="SemanticChecker" description="Semantic checker bundle" summary="Found some incidents" build_date="23.05.2019" version="1.0">
        <Param name="InputFile" value="myTrack.xodr"/>
        <Checker checkerId="roadIdChecker" description="Checks validity of the roadIds"  summary="Found 1 issue">
            <Issue description="Road with id=5 invalid" issueId="2" level="1">
                <Locations description="Road id is defined multiple times">
                    <XMLLocation xpath="/OpenDRIVE/road[@id='5']"/>
                    <FileLocation column="25" row="505"/>
                    <InertialLocation x="5.0" y="0.0" z="0.0"/>
                </Locations>
            </Issue>
        </Checker>
    </CheckerBundle>
</CheckerResults>
```

Regarding parametrization here an example of the section of the .xqar file
where the default parameters are shown:

```xml
<CheckerResults version="1.0.0">
    <CheckerBundle description="Checks the..." name="StaticXodrChecker" summary="173 issues...."> <!-- attribute file deleted! -->
        <Param name="GlobalAngleTolerance" value="0.2"/>
        <Param name="ElevationTolerance" value="0.1"/>
        (... more Params ...)
        <Checker checkerId="elevationChecker" description="Verify ..." summary="68 issues found...">
            <Issue description="#34: delta ...." issueId="0" level="2">
                <Locations description="Init section of scenario">
                    <XMLLocation ... />
                    (... more explicit location types ...)
                </Locations>
                (... more Locations ...)
            </Issue>
            (... more Issues...)
        </Checker>
        (... more Checkers...)
    </CheckerBundle>
    (... more Checker Bundles...)
</CheckerResults>
```
