# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


################################################################################
#
# QC4OpenX library Config File. 
#
################################################################################

# execute only once
if (QC4OPENX_DBQA_FRAMEWORK_GENERIC_FOUND)
    return()
endif (QC4OPENX_DBQA_FRAMEWORK_GENERIC_FOUND)

set(QC4OPENX_DBQA_FRAMEWORK_GENERIC_INCLUDE_DIR ${QC4OPENX_DBQA_FRAMEWORK_GENERIC_DIR}/include)
set(QC4OPENX_DBQA_FRAMEWORK_GENERIC_COMMON_DIR ${QC4OPENX_DBQA_FRAMEWORK_GENERIC_DIR}/src/common)
