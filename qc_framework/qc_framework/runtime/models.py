# SPDX-License-Identifier: MPL-2.0
# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
import os
import enum

from typing import List
from pydantic import BaseModel, field_validator


class ExecType(str, enum.Enum):
    EXECUTABLE = "executable"


class ModuleType(str, enum.Enum):
    CHECKER_BUNDLE = "checker_bundle"
    REPORT_MODULE = "report_module"
    RESULT_POOLING = "result_pooling"


class FrameworkManifest(BaseModel):
    manifest_file_path: List[str]

    @field_validator("manifest_file_path")
    @classmethod
    def file_path_must_exist(cls, v: List[str]) -> List[str]:
        for raw_file_path in v:
            resolved_file_path = os.path.expandvars(raw_file_path)
            if not os.path.isfile(resolved_file_path):
                raise ValueError(f"File path '{resolved_file_path}' must exist.")
        return v


class Module(BaseModel):
    name: str
    exec_type: ExecType
    module_type: ModuleType
    exec_command: str


class ModuleManifest(BaseModel):
    module: List[Module]
