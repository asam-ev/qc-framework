<!---
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# Rule UID Schema

## Introduction

The document describes the composition of the unique identifier (UID) to be
associated with quality checker rules.

The document describes composition of rule UIDs, declares some of the concepts
to be used for ASAM rules, and provides an example for an UID for a rule of the
ASAM OpenDRIVE standard.

## Rule UID Concepts

The UID is a string which encapsulate a sequence of **concepts** that allow to
identify immediately a rule across the different domains. The concept are
ordered and separated via the separation character **`:`**.

The concepts for the rule UID are:

* **Emanating Entity**: a domain name for the entity (organization or company)
  that declares the rule UID
* **Standard**: a short string that represents the standard or the domain to
  which the rule is applied
* **Definition Setting**: the version of the standard or the domain to which
  the rule appears or is applied **for the first time**
* **Rule Full Name**: the full name of the rule, as dot separated, string of
  letters, digits and underscores, with the first character a letter. The
  full name of a rule is composed by the **rule set**, a categorization for
  the rule, and the rule **name**, a unique string inside the categorization.
  The rule set can be nested (meaning that it can be defined as an arbitrary
  sequence of dot separated names, while the name is the string after the last
  dot of the full name). Unless there are strong other requirements, it is recommended
  to use snake lower case names for both rule set and rule name. In any case the rule set
  and rule names for a given standard should conform to one uniform format, and new
  rule sets and names should use this format.

To provide a visual description for a rule UID:

```text
<emanating-entity>:<standard>:x.y.z:rule_set.for_rules.rule_name
```

> Third party rule UID creators (i.e., emanating entities different than ASAM)
> should still fill all the concepts above. If that is not possible, concepts
> shall be left blank, but separation `:` is still required (i.e.,
> `example.com:::rulename`is valid)

UID are designed to be queried, e.g., implementations may use UNIX pattern
matching.

### Full Pattern

```pcre
^
 (?P<ENTITY>(\w+(\.\w+)+)):
 (?P<STANDARD>([a-z]+))?:
 (?P<STD_VERSION>([0-9]+(\.[0-9]+)+))?:
 (?P<RULESET>((\p{L}[\w_]*)\.)*)(?P<NAME>\p{L}[\w_]*)
$
```

### ASAM Concepts

* Emanating Entity: `asam.net`
* Standards:
  * Quality Checker Framework: `asam.net:qc`
  * OpenDRIVE: `asam.net:xodr`
    * v1.4.0: `asam.net:xodr:1.4.0`
    * v1.5.0: `asam.net:xodr:1.5.0`
    * v1.6.0: `asam.net:xodr:1.6.0`
    * v1.6.1: `asam.net:xodr:1.6.1`
    * v1.7.0: `asam.net:xodr:1.7.0`
    * v1.8.0: `asam.net:xodr:1.8.0`
  * OpenSCENARIO XML: `asam.net:xosc`
    * v1.0.0: `asam.net:xosc:1.0.0`
    * v1.1.0: `asam.net:xosc:1.1.0`
    * v1.1.1: `asam.net:xosc:1.1.1`
    * v1.2.0: `asam.net:xosc:1.2.0`
    * v1.3.0: `asam.net:xosc:1.3.0`
  * OpenSCENARIO DSL: `asam.net:osc`
    * v2.0.0: `asam.net:osc:2.0.0`
    * v2.1.0: `asam.net:osc:2.1.0`

Rule full names are defined in domain-specific Quality Checker subgroups. Third
party emanating entities should use the standard and definition setting
concepts defined by ASAM.

## Example

Lets start with an example for a rule UID, and lets break down its concepts. The
example references a rule listed in the subsection **Rules** of one of the ASAM
Standards for OpenDRIVE. The Rules subsections were introduced for the first
time in revision 1.6 of the standard. Note that a rule can start being applied 
to earlier versions of the standard.

> [ASAM OpenDRIVE Standard 1.6.0, Chapter 7.2 - Road Reference
> Line](https://releases.asam.net/OpenDRIVE/1.6.0/ASAM_OpenDRIVE_BS_V1-6-0.html#_road_reference_line),
> subsection Rule, first point:
> > **Each road shall have a reference line**

we can identify the following concepts:

* **emanating entity**: ASAM `asam.net`
* **standard**: OpenDRIVE `xodr`
* **definition settings**: the rule starts being applied to ASAM OpenDrive files from version `1.4.0`
* **Full name**:
  * **rule-set**: Road Geometry definitions `road.geometry`
  * **name**: summarizes the content of the rule, e.g. `refline_exists`. It is
     always the last group in dot-notation strings.

The complete UID would be something like:

```text
asam.net:xodr:1.4.0:road.geometry.refline_exists
-------- ---- ----- ------------- --------------
 |        |    |     |                  |             
 entity   | version  |                 name
       standard    rule-set
```

Regarding the validation for the schema, **only the the emanating entity and
the name are required**. Third party entities are not required to declare
standard, definition settings, and rule-set. Separation characters for omitted
parts cannot be omitted.

ASAM rules shall always define all concepts. For third parties it is highly
recommended to define all concepts, if this is reasonably possible.

> Example: If `example.com` entity defines its own rules, it may be something
> like: `example.com:::custom_rule` or `example.com:qc::custom_rule` and the
> UID is considered valid. However, if the rule for example is related to ASAM
> OpenDRIVE starting from version 1.6.0 forward, then is recommended that
> the entity use a UID like `example.com:xodr:1.6.0:custom_rule`, or where rule
> sets make sense, like `example.com:xodr:1.6.0:custom_ruleset.custom_rule`.

## Query rules UID

The proposed formalism allow to perform query on rules (or set of rules) using
UNIX style wildcards notation (i.e., python `fnmatch` module).

Example: Query all the OpenDRIVE geometrical rules that were created in version 1.4.0
1.4.1, or 1.5.0 of the standard: `asam.net:xodr:1.[45].[01]:*geometry.*` matches
`asam.net:xodr:1.4.0:road.geometry.refline_exists` but does not
match `asam.net:xodr:1.8.0:road.geometry.refline_exists`.

## Remarks on Rule Full names

There are different alternatives to define a rule full name. Those alternatives
are domain specific and this document does not restrict the strategies applied
to define them.

The format of the full name uses a well defined notation, compatible with the
following pattern:

```pcre
(?P<RULESET>((\p{L}[\w_]*)\.)*)(?P<NAME>\p{L}[\w_]*)
```

Which can be explained as follows:

* the string for rule sets and names are separated by dot `.`, and it is the
  rightmost dot of the full string
* splitting each element of the string by `.`, the first character for the
  ruleset elements or the name must be a letter character.
  The subsequent character may be any letter, digit, or the character `_`.
  This is applied on both rulesets and names
* ruleset may be empty (not defined), but if defined the minimum length of an
  element of the ruleset is 1 letter character
* the name is always the last element of the dot separated string. The minimal
  name is 1 letter character
