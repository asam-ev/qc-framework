<!---
Copyright 2023 CARIAD SE.
 
This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# SchemaChecker

This bundles checks if the XML is valid against the schema file.

The following schemas are supported for OpenDRIVE:

- OpenDRIVE 1.1
- OpenDRIVE 1.2
- OpenDRIVE 1.3D
- OpenDRIVE 1.4H
- OpenDRIVE 1.5M
- OpenDRIVE 1.6.1
- OpenDRIVE 1.7.0

... and for OpenSCENARIO:

- OpenSCENARIO 0.9.1
- OpenSCENARIO 1.0.0
- OpenSCENARIO 1.1.0

The schema files are located under `bin/xsd`.

The bundle consists of two separate executables: `bin/XodrSchemaChecker(.exe)`
and `bin/XoscSchemaChecker(.exe)`
