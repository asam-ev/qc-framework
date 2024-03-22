<!---
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# Rule UID Schema

## Introduction

The document describes the composition of the unique identifier (UID) to be
associated with quality checker rules.

The document describes composition of rule UIDs, declares some of the concepts
to be used for ASAM rules, and provides an example for an UID for a rule of the
ASAM OpenDRIVE standard.

## Rule UID Concepts

The UID is a string which encapsulate a sequence of **concepts** that allow to
identify immediately a rule across the different domains. The concept are
ordered and separated via the separation character **`:`**.

The concepts for the rule UID are:

 * **Emanating Entity**: a domain name for the entity (organization or company)
   that declares the rule UID
 * **Standard**: a short string that represents the standard or the
   domain to which the rule is applied
 * **Definition Setting**: the version of the standard or the domain to which
   the rule appears or is applied **for the first time**
 * **Rule Full Name**: the full name of the rule, as dot separated, snake lower
   case string. The full name of a rule is composed by the **rule set**, a
   categorization for the rule, and the rule **name**, a unique string inside
   the categorization. The rule set can be nested (meaning that can be defined
   as an arbitrary sequence of dot separated names, while the name is the snake
   case string after the last dot of the full name)

To provide a visual description for a rule UID:

```
<emanating-entity>:<standard>:x.y.z:rule_set.for_rules.rule_name
```

> Third party rule UID creators (i.e., emanating entities different than ASAM)
> should still fill all the concepts above. If that is not possible, concepts
> shall be left blank, but separation `:` is still required (i.e.,
> `example.com:::rulename`is valid) 

UID are designed to be queried, e.g., implementations may use UNIX pattern
matching.

### Full Pattern

```
^
 (?P<ENTITY>(\w+(\.\w+)+)):
 (?P<STANDARD>([a-z]+))?:
 (?P<STD_VERSION>([0-9]+(\.[0-9]+)+))?:
 (?P<RULESET>(([a-z][\w_]*)\.)*)(?P<NAME>[a-z][\w_]*)
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
   * OpenSCENARIO DSL: `asam.net:osc`
     * v2.0.0: `asam.net:osc:2.0.0`

Rule full names are defined in domain-specific Quality Checker subgroups. Third
party emanating entities should use the standard and definition setting
concepts defined by ASAM. 

## Example

Lets start with an example for a rule UID, and lets break down its concepts. The
example references a rule listed in the subsection **Rules** of one of the ASAM
Standards for OpenDRIVE. The Rules subsections were introduced for the first
time in revision 1.6 of the standard. **Please notice that the full name of the
rule may change in the future**.

> [ASAM OpenDRIVE Standard 1.6.0, Chapter 7.2 - Road Reference
> Line](https://releases.asam.net/OpenDRIVE/1.6.0/ASAM_OpenDRIVE_BS_V1-6-0.html#_road_reference_line),
> subsection Rule, first point: 
> > **Each road shall have a reference line**

we can identify the following concepts:

 * **emanating entity**: ASAM `asam.net`
 * **standard**: OpenDRIVE `xodr`
 * **definition settings**: the rule firstly appeared in `1.6.0`, the first
   revision to introduce the subsections rules
 * **Full name**:
    * **rule-set**: Road Planar Geometry definitions `road.planview.geometry`
    * **name**: summarizes the content of the rule, e.g. `ref_line_exists`. It
      is always the last group in dot-notation strings.

The complete UID would be something like:

```
asam.net:xodr:1.6.0:road.planview.geometry.ref_line_exists
-------- ---- ----- ---------------------- ---------------
 |        |    |     |                      |             
 entity   | version  |                    name
       standard    rule-set
```

Regarding the validation for the schema, **only the the emanating entity and
the name are required**. Third party entities are not required to declare
standard, definition settings, and rule-set. Still, separation
characters cannot be omitted. ASAM rules shall always define all concepts.

> Example: if `example.com` entity defines it's own rules, it may be something
> like: `example.com:::custom_rule` or `example.com:qc::custom_rule` and the
> UID should be considered valid. **However, entity should use ASAM standard and
> definition setting concepts in its custom rules.**

## Query rules UID

The proposed formalism allow to perform query on rules (or set of rules) using
UNIX style wildcards notation (i.e., python `fnmatch` module).

Example: query all the OpenDRIVE geometrical rules that were created in version 1.6 or
1.7 of the standard: `asam.net:xodr:1.[67].0:*geometry.*` matches
`asam.net:xodr:1.6.0:road.planview.geometry.ref_line_exists` but does not
match `asam.net:xodr:1.8.0:road.planview.geometry.ref_line_exists`

## Remarks on Rule Full names

There are different alternatives to define a rule full name. Those alternatives
are domain specific and this document does not restrict the strategies applied
to define them. 

The format of the full name uses a well defined notation, compatible with the
following pattern:

```
(?P<RULESET>(([a-z][\w_]*)\.)*)(?P<NAME>[a-z][\w_]*)
```

Which can be explained as follows:

 * the string for rule sets and names are separated by dot `.`, and it is the
   rightmost dot of the full string
 * splitting each element of the string by `.`, the first character for the
   ruleset elements or the name must be an ascii character in the range `a-z`.
   The subsequent character may be any character in the range `a-z`, `0-9` or
   the character `_`. This is applied on both rulesets and names
 * ruleset may be empty (not defined), but if defined the minimum length of an
   element of the ruleset is 1 character in range `a-z`
 * the name is always the last element of the dot separated string. The minimal
   name is 1 character long, in range `a-z`
