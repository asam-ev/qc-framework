# Framework module

The Framework module is responsible for the execution of checker bundles, result poolers and result reporters, based 
upon a defined configuration file. The main executable is implemented in the `runtime` submodule.

Input configuration file need to respect the [config xsd schema](../doc/schema/config_format.xsd)

The runtime module execute the following steps:

1. Validate input xml file given the schema
2. For each checker bundle specified in configuration, execute its process
3. Execute result pooling for collect results from all checker bundle executed in step 2
4. For each report module specified in configuration, execute its process

## Installation & Usage

The ASAM QC Framework can be installed using pip.

**From PyPi**

```bash
pip install asam-qc-framework
```

**From GitHub Repository**

```bash
pip install asam-qc-framework@git+https://github.com/asam-ev/qc-framework@main#subdirectory=qc_framework
```

The above command will install `asam-qc-framework` from the `main` branch. If you want to install `asam-qc-framework` from another branch or tag, replace `@main` with the desired branch or tag.

**From a local repository**

```bash
# From reporsitory root
pip install ./qc_framework
```

Then, it can be executed as follows.

```bash
qc_runtime --config=$PATH_TO_CONFIG_FILE --manifest=$PATH_TO_MANIFEST_FILE
```

where

- `$PATH_TO_CONFIG_FILE` points to an xml file following the [config xsd schema](../doc/schema/config_format.xsd)
- `$PATH_TO_MANIFEST_FILE` points to the [manifest file](manifest_file.md) of the framework.

All the files generated during the runtime execution, including result files and report files, will be saved in the output folder `qc-output-YYYY-MM-DD-HH-MM-SS-*`.

Alternatively, users can specify a specific output folder for the runtime using the argument `--working_dir`.

```bash
qc_runtime --config=$PATH_TO_CONFIG_FILE --manifest=$PATH_TO_MANIFEST_FILE --working_dir=$PATH_TO_OUTPUT_FOLDER
```

## Report Utilities

The Python framework integrates some reporting utilities. Reporting utilities convert  result `.xqar` 
files in other formats, e.g., for integration purposes in third-party software.

The currently available reporting utilities are:

 - **Text format**: converts result file in a human readable file
 - **JSON format**: converts result file in json format
 - **Github Pipeline test _"format"_** (or general CD/CI compatibility): provides a very synthetic text report, 
   but exits with a value different than zero if the are violated rules, so that a CD/CI pipeline can directly 
   intercept the issue.

Report utilities share the following command line options:

| Command Line Option | Type | Description |
|---------------------|------|-------------|
| `-c`, `--from-configuration` | `str` | Run the report utility from a configuration file as input (**framework mode**, cannot be used with `-x` or `-r`) |
| `-r`, `--from-result` | Flag | Run the report utility directly on `--param-InputFile` command line parameter (**standalone mode**, cannot be used with `-x` or `-c`) |
| `-x`, `--export-config` | Flag | Generate the default configuration script, output file can be defined with `-xo` (**configuration mode**, cannot be used with `-x` or `-r`) |
| `-xo`, `--export-config-output` | `str` | Defines a configuration file name for configuration output (to be used with `-x`) |
| `--param-[...]` | - | See below |

All configuration parameters can be changed with the command line parameters 
`--param-[NameOfConfigurationParameter]`. In general, the parameters are defined with this order of precedence:

 1. command line parameters override configuration file definitions
 2. configuration file override default definitions
 3. default definitions

Configuration parameters set in **configuration mode** are included in output file.

A sample manifest for report utilities is provided as example in 
[`framework_report_manifest.json`](../../demo_pipeline/manifests/framework_report_manifest.json).

### Text report

Text report can be launched as follows:

```bash
qc_report_text --help
# If python scripts are not on path
python -m qc_framework.report.text --help
```

And supports the following configuration parameters:

| Configuration Parameter | Type | Description | Default Value |
|-------------------------|------|-------------|---------------|
| `LineWidth` | `int` | Defines the maximum width for the output file line | Defaults to terminal width, if available |
| `OutputFile` | `str` | Defines the output filename | `"Report.txt"` |
| `InputFile` | `str` | Defines the input filename | `"Result.xqar"` |

<details>
<summary>Sample output, executed on the Checker Bundle Example result file</summary>

```
ASAM QUALITY FRAMEWORK v1.0.0 - RESULT REPORT
====================================================================================================

BUNDLES
----------------------------------------------------------------------------------------------------

 -> DemoCheckerBundle @ version-undefined
    Description: 
    Summary: Found 4 issues
    Checkers: 
     -> [COMPLETED] exampleChecker
        Description: This is a description
        Summary: 
        Addressed Rules:
         -> asam.net:xodr:1.0.0:first_rule_name
        Issues:
           1. [INFORMATION] @ asam.net:xodr:1.0.0:first_rule_name
              Description: This is an information from the demo usecase

     -> [COMPLETED] exampleInertialChecker
        Description: This is a description of inertial checker
        Summary: 
        Addressed Rules:
         -> asam.net:xodr:1.0.0:second_rule_name
        Issues:
           1. [INFORMATION] @ asam.net:xodr:1.0.0:second_rule_name
              Description: This is an information from the demo usecase
              Locations:
               -> Inertial @ [1; 2; 3]: inertial position

     -> [COMPLETED] exampleMetadataChecker
        Description: This is a description of metadata checker
        Summary: 
        Addressed Rules:
         -> asam.net:xodr:1.0.0:third_rule_name
        Metadata:
         -> run date: 2024/06/06 (Date in which the checker was executed)
         -> reference project: project01 (Name of the project that created the checker)

     -> [COMPLETED] exampleIssueRuleChecker
        Description: This is a description of checker with issue and the involved ruleUID
        Summary: 
        Addressed Rules:
         -> asam.net:xodr:1.0.0:fourth_rule_name
        Issues:
           1. [ERROR] @ asam.net:xodr:1.0.0:fourth_rule_name
              Description: This is an information from the demo usecase

     -> [SKIPPED] exampleSkippedChecker
        Description: This is a description of checker with skipped status
        Summary: Skipped execution

     -> [COMPLETED] exampleDomainChecker
        Description: This is a description of example domain info checker
        Summary: 
        Addressed Rules:
         -> asam.net:xodr:1.0.0:fifth_rule_name
        Issues:
           1. [INFORMATION] @ asam.net:xodr:1.0.0:fifth_rule_name
              Description: This is an information from the demo usecase
              Domain Specific Info:
               -> <DomainSpecificInfo name="test_domain">
                            <RoadLocation b="5.4" c="0.0" id="aa" />
                          </DomainSpecificInfo>


====================================================================================================

ADDRESSED RULES
----------------------------------------------------------------------------------------------------

Addressed rules: 5
 -> asam.net:xodr:1.0.0:fifth_rule_name
 -> asam.net:xodr:1.0.0:fourth_rule_name
 -> asam.net:xodr:1.0.0:third_rule_name
 -> asam.net:xodr:1.0.0:first_rule_name
 -> asam.net:xodr:1.0.0:second_rule_name

Flagged Rules: 4
  with ERROR: 1
   -> asam.net:xodr:1.0.0:fourth_rule_name
  with WARNING: 0
  with INFORMATION: 3
   -> asam.net:xodr:1.0.0:fifth_rule_name
   -> asam.net:xodr:1.0.0:first_rule_name
   -> asam.net:xodr:1.0.0:second_rule_name
----------------------------------------------------------------------------------------------------

NOTES
----------------------------------------------------------------------------------------------------

Rule UID format:
  <emanating-entity>:<standard>:x.y.z:rule_set.for_rules.rule_name

where    
  * Emanating Entity: a domain name for the entity (organization or company) that declares the rule UID
  * Standard: a short string that represents the standard or the domain to which the rule is applied
  * Definition Setting: the version of the standard or the domain to which the rule appears or is applied for the first time
  * Rule Full Name: the full name of the rule, as dot separated, snake lower case string. 
    The full name of a rule is composed by the rule set, a categorization for the rule, 
    and the rule name, a unique string inside the categorization. 
    The rule set can be nested (meaning that can be defined as an 
    arbitrary sequence of dot separated names, while the name is the snake 
    case string after the last dot of the full name)
```

</details>

### JSON report

JSON report can be launched as follows:

```bash
qc_report_json --help
# If python scripts are not on path
python -m qc_framework.report.json --help
```

And supports the following configuration parameters:

| Configuration Parameter | Type | Description | Default Value |
|-------------------------|------|-------------|---------------|
| `Indent` | `int` | Defines the indent value for JSON formatting. Set to 0 for minification | `2` |
| `OutputFile` | `str` | Defines the output filename | `"Report.txt"` |
| `InputFile` | `str` | Defines the input filename | `"Result.xqar"` |

<details>
<summary>Sample output, executed on the Checker Bundle Example result file</summary>

```json
{
  "version": "1.0.0",
  "bundles": {
    "DemoCheckerBundle": {
      "name": "DemoCheckerBundle",
      "version": "",
      "summary": "Found 4 issues",
      "description": "",
      "parameters": {},
      "checkers": {
        "exampleChecker": {
          "checkerId": "exampleChecker",
          "description": "This is a description",
          "summary": "",
          "status": "completed",
          "metadata": {},
          "addressedRule": [
            "asam.net:xodr:1.0.0:first_rule_name"
          ],
          "issues": [
            {
              "issueId": 0,
              "level": "INFORMATION",
              "description": "This is an information from the demo usecase",
              "ruleUid": "asam.net:xodr:1.0.0:first_rule_name",
              "locations": {
                "file": [],
                "xml": [],
                "inertial": []
              },
              "domainSpecificInfo": []
            }
          ]
        },
        "exampleInertialChecker": {
          "checkerId": "exampleInertialChecker",
          "description": "This is a description of inertial checker",
          "summary": "",
          "status": "completed",
          "metadata": {},
          "addressedRule": [
            "asam.net:xodr:1.0.0:second_rule_name"
          ],
          "issues": [
            {
              "issueId": 1,
              "level": "INFORMATION",
              "description": "This is an information from the demo usecase",
              "ruleUid": "asam.net:xodr:1.0.0:second_rule_name",
              "locations": {
                "file": [],
                "xml": [],
                "inertial": [
                  {
                    "description": "inertial position",
                    "x": 1.0,
                    "y": 2.0,
                    "z": 3.0
                  }
                ]
              },
              "domainSpecificInfo": []
            }
          ]
        },
        "exampleMetadataChecker": {
          "checkerId": "exampleMetadataChecker",
          "description": "This is a description of metadata checker",
          "summary": "",
          "status": "completed",
          "metadata": {
            "run date": {
              "key": "run date",
              "value": "2024/06/06",
              "description": "Date in which the checker was executed"
            },
            "reference project": {
              "key": "reference project",
              "value": "project01",
              "description": "Name of the project that created the checker"
            }
          },
          "addressedRule": [
            "asam.net:xodr:1.0.0:third_rule_name"
          ],
          "issues": []
        },
        "exampleIssueRuleChecker": {
          "checkerId": "exampleIssueRuleChecker",
          "description": "This is a description of checker with issue and the involved ruleUID",
          "summary": "",
          "status": "completed",
          "metadata": {},
          "addressedRule": [
            "asam.net:xodr:1.0.0:fourth_rule_name"
          ],
          "issues": [
            {
              "issueId": 2,
              "level": "ERROR",
              "description": "This is an information from the demo usecase",
              "ruleUid": "asam.net:xodr:1.0.0:fourth_rule_name",
              "locations": {
                "file": [],
                "xml": [],
                "inertial": []
              },
              "domainSpecificInfo": []
            }
          ]
        },
        "exampleSkippedChecker": {
          "checkerId": "exampleSkippedChecker",
          "description": "This is a description of checker with skipped status",
          "summary": "Skipped execution",
          "status": "skipped",
          "metadata": {},
          "addressedRule": [],
          "issues": []
        },
        "exampleDomainChecker": {
          "checkerId": "exampleDomainChecker",
          "description": "This is a description of example domain info checker",
          "summary": "",
          "status": "completed",
          "metadata": {},
          "addressedRule": [
            "asam.net:xodr:1.0.0:fifth_rule_name"
          ],
          "issues": [
            {
              "issueId": 3,
              "level": "INFORMATION",
              "description": "This is an information from the demo usecase",
              "ruleUid": "asam.net:xodr:1.0.0:fifth_rule_name",
              "locations": {
                "file": [],
                "xml": [],
                "inertial": []
              },
              "domainSpecificInfo": [
                "<DomainSpecificInfo name=\"test_domain\">\n          <RoadLocation b=\"5.4\" c=\"0.0\" id=\"aa\" />\n        </DomainSpecificInfo>"
              ]
            }
          ]
        }
      }
    }
  },
  "rules": {
    "addressed": [
      "asam.net:xodr:1.0.0:fourth_rule_name",
      "asam.net:xodr:1.0.0:second_rule_name",
      "asam.net:xodr:1.0.0:third_rule_name",
      "asam.net:xodr:1.0.0:first_rule_name",
      "asam.net:xodr:1.0.0:fifth_rule_name"
    ],
    "violated": {
      "error": [
        "asam.net:xodr:1.0.0:fourth_rule_name"
      ],
      "warning": [],
      "information": [
        "asam.net:xodr:1.0.0:fifth_rule_name",
        "asam.net:xodr:1.0.0:second_rule_name",
        "asam.net:xodr:1.0.0:first_rule_name"
      ]
    }
  }
}
```

</details>

### GitHub CI report

GitHub CI report can be launched as follows:

```bash
qc_report_github_ci --help
# If python scripts are not on path
python -m qc_framework.report.github_ci --help
```

And supports the following configuration parameters:

| Configuration Parameter | Type | Description | Default Value |
|-------------------------|------|-------------|---------------|
| `OutputFile` | `str` | Defines the output filename | `"Report.txt"` |
| `InputFile` | `str` | Defines the input filename | `"Result.xqar"` |

If there are violated rules, this report utility exits with an error value (`!= 0`).

### Custom Report Utilities

Users can implement custom executable by using the classes defined in the module 
`qc_framework.report.base`:

 * [`ReportFormatter`](qc_framework/qc_framework/report/base/report_formatter.py): use
   the class to implement the conversion logic of a result file. The class may be used
   directly with injection or via inheritance.
 * [`ReportApplication`](qc_framework/qc_framework/report/base/report_application.py): use
   the class to implement an executable with the same command line parameter of the other
   report utilities, and create command line parameters for configuration parameters. This
   class allows also to create configuration file. This class may be used directly, without
   inheritance.

For better documentation, please refer to classes `doc-strings`. Use the Text report as basic example:

 * [`TextFormatter`](qc_framework/qc_framework/report/text/text_formatter.py) 
   (inheritance)
 * [`main` for `TextFormatter`](qc_framework/qc_framework/report/text/__main__.py) 
   (direct `ReportApplication` usage)